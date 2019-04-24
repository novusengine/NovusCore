/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/

#include "AuthConnection.h"
#include <Networking/ByteBuffer.h>
#include <Networking/DataStore.h>

#pragma pack(push, 1)
struct cAuthLogonChallenge
{
    u8   command;
    u8   error;
    u16  size;
    u8   gamename[4];
    u8   version1;
    u8   version2;
    u8   version3;
    u16  build;
    u8   platform[4];
    u8   os[4];
    u8   country[4];
    u32  timezone_bias;
    u32  ip;
    u8   username_length;
    u8   username_pointer[1];
};

struct cAuthLogonProof
{
    u8   command;
    u8   A[32];
    u8   M1[20];
    u8   crc_hash[20];
    u8   number_of_keys;
    u8   securityFlags;
};

struct cAuthReconnectProof
{
    u8   cmd;
    u8   R1[16];
    u8   R2[20];
    u8   R3[20];
    u8   number_of_keys;
};
#pragma pack(pop)

std::array<u8, 16> VersionChallenge = { { 0xBA, 0xA3, 0x1E, 0x99, 0xA0, 0x0B, 0x21, 0x57, 0xFC, 0x37, 0x3F, 0xB3, 0x69, 0xCD, 0xD2, 0xF1 } };
#define MAX_REALM_COUNT 256

robin_hood::unordered_map<u8, AuthMessageHandler> AuthConnection::InitMessageHandlers()
{
    robin_hood::unordered_map<u8, AuthMessageHandler> messageHandlers;

    messageHandlers[AUTH_CHALLENGE] = { STATUS_CHALLENGE, 4, 1, &AuthConnection::HandleCommandChallenge };
    messageHandlers[AUTH_PROOF] = { STATUS_PROOF, sizeof(cAuthLogonProof), 1, &AuthConnection::HandleCommandProof };
    messageHandlers[AUTH_RECONNECT_CHALLENGE] = { STATUS_CHALLENGE, 4, 1, &AuthConnection::HandleCommandReconnectChallenge };
    messageHandlers[AUTH_RECONNECT_PROOF] = { STATUS_RECONNECT_PROOF, sizeof(cAuthReconnectProof), 1, &AuthConnection::HandleCommandReconnectProof };
    messageHandlers[AUTH_REALMSERVER_LIST] = { STATUS_AUTHED, 5, 3, &AuthConnection::HandleCommandRealmserverList };

    return messageHandlers;
}
robin_hood::unordered_map<u8, AuthMessageHandler> const MessageHandlers = AuthConnection::InitMessageHandlers();

bool AuthConnection::Start()
{
    AsyncRead();
    return true;
}

void AuthConnection::HandleRead()
{
    Common::ByteBuffer& byteBuffer = GetByteBuffer();
    ResetPacketsReadThisRead();

    while (byteBuffer.GetActualSize())
    {
        u8 command = byteBuffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        // Client sent wrong command
        if (itr == MessageHandlers.end() || _status != itr->second.status)
        {
            _socket->close();
            return;
        }
        
        if (command < AUTH_REALMSERVER_LIST)
        {
            packetsReadOfType = packetsReadThisRead[command]++;
        }
        else if (command == AUTH_REALMSERVER_LIST)
        {
            packetsReadOfType = packetsReadThisRead[4]++;
        }

        if (packetsReadOfType == itr->second.maxPacketsPerRead)
        {
            _socket->close();
            return;
        }

        u16 size = static_cast<u16>(itr->second.packetSize);
        if (byteBuffer.GetActualSize() < size)
            break;

        if (command == AUTH_CHALLENGE || command == AUTH_RECONNECT_CHALLENGE)
        {
            cAuthLogonChallenge* logonChallenge = reinterpret_cast<cAuthLogonChallenge*>(byteBuffer.GetReadPointer());
            size += logonChallenge->size;
            if (size > (sizeof(cAuthLogonChallenge) + 16))
            {
                _socket->close();
                return;
            }
        }

        if (byteBuffer.GetActualSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            _socket->close();
            return;
        }

        byteBuffer.ReadBytes(size);
    }

    AsyncRead();
}

bool AuthConnection::HandleCommandChallenge()
{
    _status = STATUS_CLOSED;

    cAuthLogonChallenge* logonChallenge = reinterpret_cast<cAuthLogonChallenge*>(GetByteBuffer().GetReadPointer());
    std::string login(reinterpret_cast<char const*>(logonChallenge->username_pointer), logonChallenge->username_length);
    username = login;

    PreparedStatement stmt("SELECT guid, salt, verifier FROM accounts WHERE username={s};");
    stmt.Bind(username);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this](amy::result_set& results, DatabaseConnector& connector) { HandleCommandChallengeCallback(results); });

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
    DataStore dataStore;
    dataStore.PutU8(AUTH_CHALLENGE);
    dataStore.PutU8(0);

    // Make sure the account exist.
    if (results.affected_rows() != 1)
    {
        dataStore.PutU8(AUTH_FAIL_UNKNOWN_ACCOUNT);
        Send(dataStore);
        return;
    }

    amy::row resultRow = results[0];
    accountGuid = resultRow[0].GetU32();
    std::string dbSalt = resultRow[1].GetString();
    std::string dbVerifier = resultRow[2].GetString();

    s.Hex2BN(dbSalt.c_str());
    v.Hex2BN(dbVerifier.c_str());

    b.Rand(19 * 8);
    BigNumber gen = g.ModExponential(b, N);
    B = ((v * 3) + gen) % N;

    assert(gen.GetBytes() <= 32);

    /* Check Wow Client Build Version Here */
    {

    }

    _status = STATUS_PROOF;
    dataStore.PutU8(AUTH_SUCCESS);

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
    dataStore.PutBytes(B.BN2BinArray(32).get(), 32);
    dataStore.PutU8(1);
    dataStore.PutU8(g.BN2BinArray(1).get()[0]);
    dataStore.PutU8(32);
    dataStore.PutBytes(N.BN2BinArray(32).get(), 32);
    dataStore.PutBytes(s.BN2BinArray(32).get(), 32);
    dataStore.PutBytes(VersionChallenge.data(), VersionChallenge.size());
    dataStore.PutU8(0);

    /*
        We should check here if we need to handle security flags
    */

    Send(dataStore);
}

bool AuthConnection::HandleCommandProof()
{
    _status = STATUS_CLOSED;
    cAuthLogonProof* logonProof = reinterpret_cast<cAuthLogonProof*>(GetByteBuffer().GetReadPointer());

    BigNumber A;
    A.Bin2BN(logonProof->A, 32);

    // SRP safeguard: abort if A == 0
    if ((A % N).IsZero())
        return false;

    SHA1Hasher sha;
    sha.UpdateHashForBn(2, &A, &B);
    sha.Finish();

    BigNumber u;
    u.Bin2BN(sha.GetData(), 20);
    BigNumber S = (A * (v.ModExponential(u, N))).ModExponential(b, N);

    u8 t[32];
    u8 t1[16];
    memcpy(t, S.BN2BinArray(32).get(), 32);

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
    K.Bin2BN(vK, 40);

    sha.Init();
    sha.UpdateHashForBn(1, &N);
    sha.Finish();

    u8 hash[20];
    memcpy(hash, sha.GetData(), 20);
    sha.Init();
    sha.UpdateHashForBn(1, &g);
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
    sha.UpdateHashForBn(4, &s, &A, &B, &K);
    sha.Finish();

    BigNumber M;
    M.Bin2BN(sha.GetData(), sha.GetLength());
    if (!memcmp(M.BN2BinArray(sha.GetLength()).get(), logonProof->M1, 20))
    {
        // Finish SRP6 and send the final result to the client
        sha.Init();
        sha.UpdateHashForBn(3, &A, &M, &K);
        sha.Finish();

        u8 proofM2[20];
        memcpy(proofM2, sha.GetData(), 20);

        // Update Database with SessionKey
        PreparedStatement stmt("UPDATE accounts SET sessionkey={s} WHERE username={s};");
        stmt.Bind(K.BN2Hex());
        stmt.Bind(username);
        DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this, proofM2](amy::result_set& results, DatabaseConnector& connector)
        {
            /* Logon Proof Data Structure

               - Type: u8,      Name: Packet Command
               - Type: u8,      Name: Error Code
               - Type: u8[20],  Name: SRP6 Hash Proof
               - Type: u32,     Name: Account Flags
               - Type: u32,     Name: SurveyId
               - Type: u16,     Name: Login Flags

               https://en.wikipedia.org/wiki/Secure_Remote_Password_protocol
            */
            DataStore dataStore;
            dataStore.PutU8(AUTH_PROOF);
            dataStore.PutU8(0);
            dataStore.PutBytes(const_cast<u8*>(reinterpret_cast<const u8*>(proofM2)), 20);
            dataStore.PutU32(0);
            dataStore.PutU32(0);
            dataStore.PutU16(0);

            Send(dataStore);
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
        DataStore dataStore;
        dataStore.PutU8(AUTH_PROOF);
        dataStore.PutU8(AUTH_FAIL_UNKNOWN_ACCOUNT);
        dataStore.PutU16(0);

        Send(dataStore);
    }

    return true;
}

bool AuthConnection::HandleCommandReconnectChallenge()
{
    _status = STATUS_CLOSED;

    cAuthLogonChallenge* logonChallenge = reinterpret_cast<cAuthLogonChallenge*>(GetByteBuffer().GetReadPointer());
    if (logonChallenge->size - (sizeof(cAuthLogonChallenge) - 4 - 1) != logonChallenge->username_length)
        return false;

    std::string login(reinterpret_cast<char const*>(logonChallenge->username_pointer), logonChallenge->username_length);
    username = login;

    PreparedStatement stmt("SELECT guid, sessionKey FROM accounts WHERE username={s};");
    stmt.Bind(username);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this](amy::result_set& results, DatabaseConnector& connector) { HandleCommandReconnectChallengeCallback(results); });

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
    DataStore dataStore;
    dataStore.PutU8(AUTH_RECONNECT_CHALLENGE);

    // Make sure the account exist.
    if (results.affected_rows() != 1)
    {
        dataStore.PutU8(AUTH_FAIL_UNKNOWN_ACCOUNT);
        Send(dataStore);
        return;
    }

    amy::row resultRow = results[0];
    accountGuid = resultRow[0].GetU32();
    K.Hex2BN(resultRow[1].GetString().c_str());

    _reconnectSeed.Rand(16 * 8);
    dataStore.PutU8(0);
    dataStore.PutBytes(_reconnectSeed.BN2BinArray(16).get(), 16);
    dataStore.PutBytes(VersionChallenge.data(), VersionChallenge.size());

    Send(dataStore);
    _status = STATUS_RECONNECT_PROOF;
}
bool AuthConnection::HandleCommandReconnectProof()
{
    _status = STATUS_CLOSED;
    cAuthReconnectProof* reconnectLogonProof = reinterpret_cast<cAuthReconnectProof*>(GetByteBuffer().GetReadPointer());
    if (username.length() == 0 || !_reconnectSeed.GetBytes() || !K.GetBytes())
        return false;

    BigNumber t1;
    t1.Bin2BN(reconnectLogonProof->R1, 16);

    SHA1Hasher sha;
    sha.Init();
    sha.UpdateHash(username);
    sha.UpdateHashForBn(3, &t1, &_reconnectSeed, &K);
    sha.Finish();

    if (!memcmp(sha.GetData(), reconnectLogonProof->R2, SHA_DIGEST_LENGTH))
    {
        /* Logon Proof Data Structure

           - Type: u8,      Name: Packet Command
           - Type: u8,      Name: Error Code
           - Type: u16,     Name: Login Flags
        */
        DataStore dataStore;
        dataStore.PutU8(AUTH_RECONNECT_PROOF);
        dataStore.PutU8(0);
        dataStore.PutU16(0);

        Send(dataStore);
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
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, realmCharacterCount, [this](amy::result_set& result, DatabaseConnector& connector)
    {
        std::vector<u8> realmCharacterData(MAX_REALM_COUNT);
        std::fill(realmCharacterData.begin(), realmCharacterData.end(), 0);

        for (auto row : result)
        {
            realmCharacterData[row[0].GetU8()] = row[1].GetU8();
        }

        amy::result_set realmserverListResultData;
        if (!connector.Query("SELECT id, name, address, type, flags, timezone, population FROM realms;", realmserverListResultData))
            return;

        /* Logon Proof Data Structure

           - Type: u8,      Name: Packet Command
           - Type: u16,     Name: Packet Payload Size (Excluding command + self)
           - Type: u32,     Name: Unknown (I've been unable to figure out what this does so far)
           - Type: u16,     Name: Count of available realms

           - Type: ?,       Name: Realm Data (See Below for structure information)

           - Type: u8,      Name: Unknown (This value depends on game version)
           - Type: u8,      Name: Unknown (This value depends on game version)

        */
        DataStore dataStore(nullptr, 32768);
        dataStore.PutU8(AUTH_REALMSERVER_LIST);

        // Calculate expected payload size. Realm Strings are accounted for later.
        size_t dataStoreSize = 6 + (realmserverListResultData.affected_rows() * 10) + 2;

        // Store WritePos to later write Payload Size. Reserve 2 bytes.
        size_t dataStoreSizeWrittenPos = dataStore.WrittenData;
        dataStore.PutU16(0);

        dataStore.PutU32(0);
        dataStore.PutU16(static_cast<u16>(realmserverListResultData.affected_rows()));

        for (auto row : realmserverListResultData)
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
            u8 realmId = row[0].GetU8();
            dataStore.PutU8(row[3].GetU8());
            dataStore.PutU8(0);
            dataStore.PutU8(row[4].GetU8());
            size_t realmNameSize = dataStore.PutString(row[1].GetString());
            size_t realmAddressSize = dataStore.PutString(row[2].GetString());
            dataStore.PutF32(row[6].GetF32());
            dataStore.PutU8(realmCharacterData[realmId]);
            dataStore.PutU8(row[5].GetU8());
            dataStore.PutU8(realmId);

            // Add Realm String Sizes to Payload Size
            dataStoreSize += realmNameSize + realmAddressSize;
        }

        // (Only needed for clients TBC+)
        dataStore.PutU8(0x10); // Unk1
        dataStore.PutU8(0x00); // Unk2

        // Store WritePos. Write Payload Size. Restore WritePos
        size_t dataStoreFinalWritePos = dataStore.WrittenData;
        dataStore.WrittenData = dataStoreSizeWrittenPos;
        dataStore.PutU16(static_cast<u16>(dataStoreSize));
        dataStore.WrittenData = dataStoreFinalWritePos;

        Send(dataStore);
        _status = STATUS_AUTHED;
    });

    return true;
}