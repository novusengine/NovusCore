#include "AuthConnection.h"
#include <Networking/ByteBuffer.h>
#include <Cryptography/SHA1.h>

#pragma pack(push, 1)
struct ClientLogonChallenge
{
    u8 command;
    u8 error;
    u16 size;
    u8 gameName[4];
    u8 gameVersion1;
    u8 gameVersion2;
    u8 gameVersion3;
    u16 gameBuild;
    u8 clientPlatform[4];
    u8 clientOS[4];
    u8 clientCountry[4];
    u32 timezoneBias;
    u32 clientIP;
    u8 usernameLength;
    u8 usernamePointer[1];
};

struct ClientLogonProof
{
    u8 command;
    u8 ephemeralKey[32];
    u8 m1[20];
    u8 crcHash[20];
    u8 numberOfKeys;
    u8 securityFlags;
};

struct ClientReconnectProof
{
    u8 cmd;
    u8 r1[16];
    u8 r2[20];
    u8 r3[20];
    u8 numberOfKeys;
};
#pragma pack(pop)

std::array<u8, 16> versionChallenge = {{0xBA, 0xA3, 0x1E, 0x99, 0xA0, 0x0B, 0x21, 0x57, 0xFC, 0x37, 0x3F, 0xB3, 0x69, 0xCD, 0xD2, 0xF1}};
#define MAX_REALM_COUNT 256

robin_hood::unordered_map<u8, AuthMessageHandler> AuthConnection::InitMessageHandlers()
{
    robin_hood::unordered_map<u8, AuthMessageHandler> messageHandlers;

    messageHandlers[AUTH_CHALLENGE] = {STATUS_CHALLENGE, 4, 1, &AuthConnection::HandleCommandChallenge};
    messageHandlers[AUTH_PROOF] = {STATUS_PROOF, sizeof(ClientLogonProof), 1, &AuthConnection::HandleCommandProof};
    messageHandlers[AUTH_RECONNECT_CHALLENGE] = {STATUS_CHALLENGE, 4, 1, &AuthConnection::HandleCommandReconnectChallenge};
    messageHandlers[AUTH_RECONNECT_PROOF] = {STATUS_RECONNECT_PROOF, sizeof(ClientReconnectProof), 1, &AuthConnection::HandleCommandReconnectProof};
    messageHandlers[AUTH_REALMSERVER_LIST] = {STATUS_AUTHED, 5, 3, &AuthConnection::HandleCommandRealmserverList};

    return messageHandlers;
}
robin_hood::unordered_map<u8, AuthMessageHandler> const messageHandlers = AuthConnection::InitMessageHandlers();
robin_hood::unordered_map<u8, RealmServerData> AuthConnection::realmServerList;
std::mutex AuthConnection::realmServerListMutex;

bool AuthConnection::Start()
{
    AsyncRead();
    return true;
}

void AuthConnection::HandleRead()
{
    ByteBuffer& buffer = GetReceiveBuffer();
    ResetPacketsReadOfType();

    while (u32 activeSize = buffer.GetActiveSize())
    {
        u8 command = buffer.GetInternalData()[0];

        auto itr = messageHandlers.find(command);
        if (itr == messageHandlers.end() || _status != itr->second.status)
        {
            _socket->close();
            return;
        }

        if (++packetsReadOfType[command] > itr->second.maxPacketsPerRead)
        {
            _socket->close();
            return;
        }

        u16 size = static_cast<u16>(itr->second.packetSize);
        if (activeSize < size)
            break;

        if (command == AUTH_CHALLENGE || command == AUTH_RECONNECT_CHALLENGE)
        {
            ClientLogonChallenge* logonChallenge = reinterpret_cast<ClientLogonChallenge*>(buffer.GetReadPointer());
            size += logonChallenge->size;
            if (size > (sizeof(ClientLogonChallenge) + 16))
            {
                _socket->close();
                return;
            }
        }

        if (activeSize < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            _socket->close();
            return;
        }

        buffer.ReadData += size;
    }

    AsyncRead();
}

bool AuthConnection::HandleCommandChallenge()
{
    _status = STATUS_CLOSED;

    ClientLogonChallenge* logonChallenge = reinterpret_cast<ClientLogonChallenge*>(GetReceiveBuffer().GetReadPointer());
    std::string login(reinterpret_cast<char const*>(logonChallenge->usernamePointer), logonChallenge->usernameLength);
    username = login;

    PreparedStatement stmt("SELECT guid, salt, verifier FROM accounts WHERE username={s};");
    stmt.Bind(username);

    DatabaseConnector::QueryAsync(
        DATABASE_TYPE::AUTHSERVER, stmt,
        [this](amy::result_set& results, DatabaseConnector& connector) {
            HandleCommandChallengeCallback(results);
        });

    return true;
}
void AuthConnection::HandleCommandChallengeCallback(amy::result_set& results)
{
    /* Logon Challenge Data Structure

     - Type: u8,      Name: Packet Command
     - Type: u8,      Name: Error Code
     - Type: u8,      Name: Result Code
     - Type:  ?,      Name: Logon Challenge Data (See below for structure)
    */
    ByteBuffer buffer;
    buffer.PutU8(AUTH_CHALLENGE);
    buffer.PutU8(0);

    // Make sure the account exist.
    if (results.affected_rows() != 1)
    {
        buffer.PutU8(AUTH_FAIL_UNKNOWN_ACCOUNT);
        Send(buffer);
        return;
    }

    amy::row resultRow = results[0];
    accountGuid = resultRow[0].GetU32();
    std::string dbSalt = resultRow[1].GetString();
    std::string dbVerifier = resultRow[2].GetString();

    smallSalt.Hex2BN(dbSalt.c_str());
    clientVerifier.Hex2BN(dbVerifier.c_str());

    random.Rand(19 * 8);
    BigNumber gen = generator.ModExponential(random, nPrime);
    ephemeralKeyB = ((clientVerifier * 3) + gen) % nPrime;

    assert(gen.GetBytes() <= 32);

    /* Check Wow Client Build Version Here */
    {
    }

    _status = STATUS_PROOF;
    buffer.PutU8(AUTH_SUCCESS);

    /* Logon Challenge Data Structure

     - Type: u8[32],  Name: B
     - Type: u8,      Name: Lenght of G
     - Type: u8,      Name: G
     - Type: u8,      Name: Lenght of N
     - Type: u8[32],  Name: N
     - Type: u8[32],  Name: Salt
     - Type: u8[16],  Name: Version Challenge
     - Type: u8,      Name: Security Flag
     https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol
    */
    buffer.PutBytes(ephemeralKeyB.BN2BinArray(32).get(), 32);
    buffer.PutU8(1);
    buffer.PutU8(generator.BN2BinArray(1).get()[0]);
    buffer.PutU8(32);
    buffer.PutBytes(nPrime.BN2BinArray(32).get(), 32);
    buffer.PutBytes(smallSalt.BN2BinArray(32).get(), 32);
    buffer.PutBytes(versionChallenge.data(), versionChallenge.size());
    buffer.PutU8(0);

    /*
      We should check here if we need to handle security flags
    */

    Send(buffer);
}

bool AuthConnection::HandleCommandProof()
{
    _status = STATUS_CLOSED;
    ClientLogonProof* logonProof = reinterpret_cast<ClientLogonProof*>(GetReceiveBuffer().GetReadPointer());

    BigNumber ephemeralKeyA;
    ephemeralKeyA.Bin2BN(logonProof->ephemeralKey, 32);

    if ((ephemeralKeyA % nPrime).IsZero())
        return false;

    SHA1Hasher sha;
    sha.UpdateHashForBn(2, &ephemeralKeyA, &ephemeralKeyB);
    sha.Finish();

    BigNumber u;
    u.Bin2BN(sha.GetData(), 20);
    BigNumber s = (ephemeralKeyA * (clientVerifier.ModExponential(u, nPrime))).ModExponential(random, nPrime);

    u8 t[32];
    u8 t1[16];
    memcpy(t, s.BN2BinArray(32).get(), 32);

    for (i32 i = 0; i < 16; ++i)
        t1[i] = t[i * 2];

    sha.Init();
    sha.UpdateHash(t1, 16);
    sha.Finish();

    u8 vK[40];
    for (i32 i = 0; i < 20; ++i)
        vK[i * 2] = sha.GetData()[i];

    for (i32 i = 0; i < 16; ++i)
        t1[i] = t[i * 2 + 1];

    sha.Init();
    sha.UpdateHash(t1, 16);
    sha.Finish();

    for (i32 i = 0; i < 20; ++i)
        vK[i * 2 + 1] = sha.GetData()[i];
    sessionKey.Bin2BN(vK, 40);

    sha.Init();
    sha.UpdateHashForBn(1, &nPrime);
    sha.Finish();

    u8 hash[20];
    memcpy(hash, sha.GetData(), 20);
    sha.Init();
    sha.UpdateHashForBn(1, &generator);
    sha.Finish();

    for (i32 i = 0; i < 20; ++i)
        hash[i] ^= sha.GetData()[i];

    sha.Init();
    sha.UpdateHash(username);
    sha.Finish();

    BigNumber t3;
    t3.Bin2BN(hash, 20);
    u8 t4[SHA_DIGEST_LENGTH];
    memcpy(t4, sha.GetData(), SHA_DIGEST_LENGTH);

    sha.Init();
    sha.UpdateHashForBn(1, &t3);
    sha.UpdateHash(t4, SHA_DIGEST_LENGTH);
    sha.UpdateHashForBn(4, &smallSalt, &ephemeralKeyA, &ephemeralKeyB, &sessionKey);
    sha.Finish();

    BigNumber m;
    m.Bin2BN(sha.GetData(), sha.GetLength());
    if (!memcmp(m.BN2BinArray(sha.GetLength()).get(), logonProof->m1, 20))
    {
        // Finish SRP6 and send the final result to the client
        sha.Init();
        sha.UpdateHashForBn(3, &ephemeralKeyA, &m, &sessionKey);
        sha.Finish();

        u8 proofM2[20];
        memcpy(proofM2, sha.GetData(), 20);

        // Update Database with SessionKey
        PreparedStatement stmt("UPDATE accounts SET sessionkey={s} WHERE username={s};");
        stmt.Bind(sessionKey.BN2Hex());
        stmt.Bind(username);

        DatabaseConnector::QueryAsync(
            DATABASE_TYPE::AUTHSERVER, stmt,
            [this, proofM2](amy::result_set& results,
                            DatabaseConnector& connector) {
                /* Logon Proof Data Structure

                - Type: u8,      Name: Packet Command
                - Type: u8,      Name: Error Code
                - Type: u8[20],  Name: SRP6 Hash Proof
                - Type: u32,     Name: Account Flags
                - Type: u32,     Name: SurveyId
                - Type: u16,     Name: Login Flags

                https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol
                */
                ByteBuffer buffer;
                buffer.PutU8(AUTH_PROOF);
                buffer.PutU8(0);
                buffer.PutBytes(
                    const_cast<u8*>(reinterpret_cast<const u8*>(proofM2)), 20);
                buffer.PutU32(0);
                buffer.PutU32(0);
                buffer.PutU16(0);

                Send(buffer);
                _status = STATUS_AUTHED;
            });
    }
    else
    {
        /* Logon Proof Data Structure

        - Type: u8,      Name: Packet Command
        - Type: u8,      Name: Error Code
        - Type: u16,     Name: Login Flags
        */
        ByteBuffer buffer;
        buffer.PutU8(AUTH_PROOF);
        buffer.PutU8(AUTH_FAIL_UNKNOWN_ACCOUNT);
        buffer.PutU16(0);

        Send(buffer);
    }

    return true;
}

bool AuthConnection::HandleCommandReconnectChallenge()
{
    _status = STATUS_CLOSED;

    ClientLogonChallenge* logonChallenge = reinterpret_cast<ClientLogonChallenge*>(GetReceiveBuffer().GetReadPointer());
    if (logonChallenge->size - (sizeof(ClientLogonChallenge) - 4 - 1) != logonChallenge->usernameLength)
        return false;

    std::string login(reinterpret_cast<char const*>(logonChallenge->usernamePointer), logonChallenge->usernameLength);
    username = login;

    PreparedStatement stmt("SELECT guid, sessionKey FROM accounts WHERE username={s};");
    stmt.Bind(username);

    DatabaseConnector::QueryAsync(
        DATABASE_TYPE::AUTHSERVER, stmt,
        [this](amy::result_set& results, DatabaseConnector& connector) {
            HandleCommandReconnectChallengeCallback(results);
        });

    return true;
}
void AuthConnection::HandleCommandReconnectChallengeCallback(amy::result_set& results)
{
    /* Logon Proof Data Structure

     - Type: u8,      Name: Packet Command
     - Type: u8,      Name: Error Code
     - Type: u8[20],  Name: Reconnect Seed
     - Type: u8[20],  Name: Version Challenge

     https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol
    */
    ByteBuffer buffer;
    buffer.PutU8(AUTH_RECONNECT_CHALLENGE);

    // Make sure the account exist.
    if (results.affected_rows() != 1)
    {
        buffer.PutU8(AUTH_FAIL_UNKNOWN_ACCOUNT);
        Send(buffer);
        return;
    }

    amy::row resultRow = results[0];
    accountGuid = resultRow[0].GetU32();
    sessionKey.Hex2BN(resultRow[1].GetString().c_str());

    _reconnectSeed.Rand(16 * 8);
    buffer.PutU8(0);
    buffer.PutBytes(_reconnectSeed.BN2BinArray(16).get(), 16);
    buffer.PutBytes(versionChallenge.data(), versionChallenge.size());

    Send(buffer);
    _status = STATUS_RECONNECT_PROOF;
}
bool AuthConnection::HandleCommandReconnectProof()
{
    _status = STATUS_CLOSED;
    ClientReconnectProof* reconnectLogonProof = reinterpret_cast<ClientReconnectProof*>(GetReceiveBuffer().GetReadPointer());
    if (username.length() == 0 || !_reconnectSeed.GetBytes() || !sessionKey.GetBytes())
        return false;

    BigNumber t1;
    t1.Bin2BN(reconnectLogonProof->r1, 16);

    SHA1Hasher sha;
    sha.Init();
    sha.UpdateHash(username);
    sha.UpdateHashForBn(3, &t1, &_reconnectSeed, &sessionKey);
    sha.Finish();

    if (!memcmp(sha.GetData(), reconnectLogonProof->r2, SHA_DIGEST_LENGTH))
    {
        /* Logon Proof Data Structure

       - Type: u8,      Name: Packet Command
       - Type: u8,      Name: Error Code
       - Type: u16,     Name: Login Flags
    */
        ByteBuffer buffer;
        buffer.PutU8(AUTH_RECONNECT_PROOF);
        buffer.PutU8(0);
        buffer.PutU16(0);

        Send(buffer);
        _status = STATUS_AUTHED;
        return true;
    }

    return false;
}

bool AuthConnection::HandleCommandRealmserverList()
{
    _status = STATUS_WAITING_FOR_REALMSERVER_LIST;

    PreparedStatement realmCharacterCount("SELECT realmId, characters FROM realm_characters WHERE account={u};");
    realmCharacterCount.Bind(accountGuid);

    DatabaseConnector::QueryAsync(
        DATABASE_TYPE::AUTHSERVER, realmCharacterCount,
        [this](amy::result_set& result, DatabaseConnector& connector) {
            std::vector<u8> realmCharacterData(MAX_REALM_COUNT);
            std::fill(realmCharacterData.begin(), realmCharacterData.end(), 0);

            for (auto row : result)
            {
                realmCharacterData[row[0].GetU8()] = row[1].GetU8();
            }

            realmServerListMutex.lock();
            /* Logon Proof Data Structure

            - Type: u8,      Name: Packet Command
            - Type: u16,     Name: Packet Payload Size (Excluding command + self)
            - Type: u32,     Name: Unknown (I've been unable to figure out what this does so far)
            - Type: u16,     Name: Count of available realms

            - Type: ?,       Name: Realm Data (See Below for structure information)

            - Type: u8,      Name: Unknown (This value depends on game version)
            - Type: u8,      Name: Unknown (This value depends on game version)
            */
            ByteBuffer buffer(nullptr, 32768);
            buffer.PutU8(AUTH_REALMSERVER_LIST);

            // Calculate expected payload size. Realm Strings are accounted for
            // later.
            size_t dataStoreSize = 6 + (realmServerList.size() * 10) + 2;

            // Store WritePos to later write Payload Size. Reserve 2 bytes.
            size_t dataStoreSizeWrittenPos = buffer.WrittenData;
            buffer.PutU16(0);

            buffer.PutU32(0);
            buffer.PutU16(static_cast<u16>(realmServerList.size()));

            for (auto realmItr : realmServerList)
            {
                /*
                - Type: u8,      Name: Realm Type
                - Type: u8,      Name: Is Realm Locked
                - Type: u8,      Name: Realm Flags
                - Type: string,  Name: Realm Name
                - Type: string,  Name: Realm Address
                - Type: f32,     Name: Realm Population (Valid values are 0, 1, 2)
                - Type: u8,      Name: Count of characters for the player on the given realm
                - Type: u8,      Name: Realm Timezone
                - Type: u8,      Name: Realm Id
                */
                RealmServerData realmData = realmItr.second;
                buffer.PutU8(realmData.type);
                buffer.PutU8(0);
                buffer.PutU8(realmData.flags);
                size_t realmNameSize = buffer.PutString(realmData.realmName);
                size_t realmAddressSize = buffer.PutString(realmData.realmAddress);
                buffer.PutF32(realmData.population);
                buffer.PutU8(realmCharacterData[realmItr.first]);
                buffer.PutU8(realmData.timeZone);
                buffer.PutU8(realmItr.first);

                // Add Realm String Sizes to Payload Size
                dataStoreSize += realmNameSize + realmAddressSize;
            }
            realmServerListMutex.unlock();

            // (Only needed for clients TBC+)
            buffer.PutU8(0x10); // Unk1
            buffer.PutU8(0x00); // Unk2

            // Store WritePos. Write Payload Size. Restore WritePos
            size_t dataStoreFinalWritePos = buffer.WrittenData;
            buffer.WrittenData = dataStoreSizeWrittenPos;
            buffer.PutU16(static_cast<u16>(dataStoreSize));
            buffer.WrittenData = dataStoreFinalWritePos;

            Send(buffer);
            _status = STATUS_AUTHED;
        });

    return true;
}