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
#include <Networking\ByteBuffer.h>

#pragma pack(push, 1)
struct cAuthLogonChallenge
{
    uint8_t   command;
    uint8_t   error;
    uint16_t  size;
    uint8_t   gamename[4];
    uint8_t   version1;
    uint8_t   version2;
    uint8_t   version3;
    uint16_t  build;
    uint8_t   platform[4];
    uint8_t   os[4];
    uint8_t   country[4];
    uint32_t  timezone_bias;
    uint32_t  ip;
    uint8_t   username_length;
    uint8_t   username_pointer[1];
};

struct sAuthLogonChallengeHeader
{
    uint8_t  command;
    uint8_t  error;
    uint8_t  result;

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Append((uint8_t*)this, sizeof(sAuthLogonChallengeHeader));
    }
};

struct sAuthLogonChallengeData
{
    uint8_t b[32];
    uint8_t unk1;
    uint8_t g;
    uint8_t unk2;
    uint8_t n[32];
    uint8_t s[32];
    uint8_t version_challenge[16];
    uint8_t security_flags;

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Append((uint8_t*)this, sizeof(sAuthLogonChallengeData));
    }

    void Append(uint8_t* dest, const uint8_t* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }
};

struct cAuthLogonProof
{
    uint8_t   command;
    uint8_t   A[32];
    uint8_t   M1[20];
    uint8_t   crc_hash[20];
    uint8_t   number_of_keys;
    uint8_t   securityFlags;
};

struct sAuthLogonProof
{
    uint8_t   cmd;
    uint8_t   error;
    uint8_t   M2[20];
    uint32_t  AccountFlags;
    uint32_t  SurveyId;
    uint16_t  LoginFlags;
};

struct cAuthReconnectProof
{
    uint8_t   cmd;
    uint8_t   R1[16];
    uint8_t   R2[20];
    uint8_t   R3[20];
    uint8_t   number_of_keys;
};

struct sAuthLogonGameListData
{
    uint8_t     Type;
    uint8_t     Locked;
    uint8_t     Flags;
    std::string Name;
    std::string Address;
    float       Population;
    uint8_t     Characters;
    uint8_t     Timezone;
    uint8_t     Id;

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Write<uint8_t>(Type);
        buffer.Write<uint8_t>(Locked);
        buffer.Write<uint8_t>(Flags);
        buffer.WriteString(Name);
        buffer.WriteString(Address);
        buffer.Write<float>(Population);
        buffer.Write<uint8_t>(Characters);
        buffer.Write<uint8_t>(Timezone);
        buffer.Write<uint8_t>(Id);
    }
};
#pragma pack(pop)

std::array<uint8_t, 16> VersionChallenge = { { 0xBA, 0xA3, 0x1E, 0x99, 0xA0, 0x0B, 0x21, 0x57, 0xFC, 0x37, 0x3F, 0xB3, 0x69, 0xCD, 0xD2, 0xF1 } };
#define MAX_REALM_COUNT 256

std::unordered_map<uint8_t, AuthMessageHandler> AuthConnection::InitMessageHandlers()
{
    std::unordered_map<uint8_t, AuthMessageHandler> messageHandlers;

    messageHandlers[AUTH_CHALLENGE]             = { STATUS_CHALLENGE,         4,                              1,   &AuthConnection::HandleCommandChallenge          };
    messageHandlers[AUTH_PROOF]                 = { STATUS_PROOF,             sizeof(cAuthLogonProof),        1,   &AuthConnection::HandleCommandProof              };
    messageHandlers[AUTH_RECONNECT_CHALLENGE]   = { STATUS_CHALLENGE,         4,                              1,   &AuthConnection::HandleCommandReconnectChallenge };
    messageHandlers[AUTH_RECONNECT_PROOF]       = { STATUS_RECONNECT_PROOF,   sizeof(cAuthReconnectProof),    1,   &AuthConnection::HandleCommandReconnectProof     };
    messageHandlers[AUTH_GAMESERVER_LIST]       = { STATUS_AUTHED,            5,                              3,   &AuthConnection::HandleCommandGameServerList     };

    return messageHandlers;
}
std::unordered_map<uint8_t, AuthMessageHandler> const MessageHandlers = AuthConnection::InitMessageHandlers();

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
        uint8_t command = byteBuffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            byteBuffer.Clean();
            break;
        }

        // Client attempted incorrect auth step
        if (_status != itr->second.status)
        {
            _socket->close();
            return;
        }

        if (packetsReadThisRead[command] == itr->second.maxPacketsPerRead)
        {
            _socket->close();
            return;
        }
        else
        {
            ++packetsReadThisRead[command];
        }

        uint16_t size = uint16_t(itr->second.packetSize);
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
    std::string login((char const*)logonChallenge->username_pointer, logonChallenge->username_length);
    username = login;

    PreparedStatement stmt("SELECT guid, salt, verifier FROM accounts WHERE username={s};");
    stmt.Bind(username);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this](amy::result_set& results, DatabaseConnector& connector) { HandleCommandChallengeCallback(results); });

    return true;
}
void AuthConnection::HandleCommandChallengeCallback(amy::result_set& results)
{
    Common::ByteBuffer response;

    sAuthLogonChallengeHeader header;
    header.command = AUTH_CHALLENGE;
    header.error = 0;

    // Make sure the account exist.
    if (results.affected_rows() != 1)
    {
        header.result = AUTH_FAIL_UNKNOWN_ACCOUNT;
        header.AddTo(response);
        Send(response);
        return;
    }

    accountGuid = results[0][0].as<amy::sql_int_unsigned>();
    std::string dbSalt = results[0][1].as<amy::sql_varchar>();
    std::string dbVerifier = results[0][2].as<amy::sql_varchar>();

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
    header.result = AUTH_SUCCESS;
    header.AddTo(response);

    sAuthLogonChallengeData data;
    data.Append(data.b, B.BN2BinArray(32).get(), 32);
    data.unk1 = 1;
    data.g = g.BN2BinArray(32).get()[0];
    data.unk2 = 32;
    data.Append(data.n, N.BN2BinArray(32).get(), 32);
    data.Append(data.s, s.BN2BinArray(32).get(), 32); // 32 bytes (SRP_6_S)
    data.Append(data.version_challenge, VersionChallenge.data(), VersionChallenge.size());
    data.security_flags = 0;
    data.AddTo(response);

    /*
        We should check here if we need to handle security flags
    */

    Send(response);
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

    uint8_t t[32];
    uint8_t t1[16];
    memcpy(t, S.BN2BinArray(32).get(), 32);

    for (int i = 0; i < 16; ++i)
        t1[i] = t[i * 2];

    sha.Init();
    sha.UpdateHash(t1, 16);
    sha.Finish();

    uint8_t vK[40];
    for (int i = 0; i < 20; ++i)
        vK[i * 2] = sha.GetData()[i];

    for (int i = 0; i < 16; ++i)
        t1[i] = t[i * 2 + 1];

    sha.Init();
    sha.UpdateHash(t1, 16);
    sha.Finish();

    for (int i = 0; i < 20; ++i)
        vK[i * 2 + 1] = sha.GetData()[i];
    K.Bin2BN(vK, 40);

    sha.Init();
    sha.UpdateHashForBn(1, &N);
    sha.Finish();

    uint8_t hash[20];
    memcpy(hash, sha.GetData(), 20);
    sha.Init();
    sha.UpdateHashForBn(1, &g);
    sha.Finish();

    for (int i = 0; i < 20; ++i)
        hash[i] ^= sha.GetData()[i];

    sha.Init();
    sha.UpdateHash(username);
    sha.Finish();

    BigNumber t3;
    t3.Bin2BN(hash, 20);
    uint8_t t4[SHA_DIGEST_LENGTH];
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

        uint8_t proofM2[20];
        memcpy(proofM2, sha.GetData(), 20);

        // Update Database with SessionKey
        PreparedStatement stmt("UPDATE accounts SET sessionkey={s} WHERE username={s};");
        stmt.Bind(K.BN2Hex());
        stmt.Bind(username);
        DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this, proofM2](amy::result_set& results, DatabaseConnector& connector)
        {
            Common::ByteBuffer packet;
            sAuthLogonProof proof;

            memcpy(proof.M2, proofM2, 20);
            proof.cmd = AUTH_PROOF;
            proof.error = 0;
            proof.AccountFlags = 0x00;    // 0x01 = GM, 0x08 = Trial, 0x00800000 = Pro pass (arena tournament)
            proof.SurveyId = 0;
            proof.LoginFlags = 0x00;

            packet.Resize(sizeof(proof));
            std::memcpy(packet.data(), &proof, sizeof(proof));
            packet.WriteBytes(sizeof(proof));

            Send(packet);
            _status = STATUS_AUTHED;
        });
    }
    else
    {
        Common::ByteBuffer byteBuffer;
        byteBuffer.Write<uint8_t>(AUTH_PROOF);
        byteBuffer.Write<uint8_t>(AUTH_FAIL_UNKNOWN_ACCOUNT); // error
        byteBuffer.Write<uint16_t>(0); // AccountFlag
        Send(byteBuffer);
    }

    return true;
}

bool AuthConnection::HandleCommandReconnectChallenge()
{
    return false;
    /*Common::ByteBuffer pkt;
    pkt.Write<uint8_t>(AUTH_RECONNECT_CHALLENGE);

    // Check if account exists
    if (login != username)
    {
        pkt.Write<uint8_t>(uint8_t(0x03)); //WOW_FAIL_UNKNOWN_ACCOUNT);
        Send(pkt);
        return true;
    }

    _reconnectProof.Rand(16 * 8);
    _status = STATUS_RECONNECT_PROOF;

    pkt.Write<uint8_t>(0); // WOW_SUCCESS
    pkt.Append(_reconnectProof.BN2BinArray(16).get(), 16);  // 16 bytes random
    uint64_t zeros = 0x00;
    pkt.Append((uint8_t*)&zeros, sizeof(zeros));                 // 8 bytes zeros
    pkt.Append((uint8_t*)&zeros, sizeof(zeros));                 // 8 bytes zeros

    Send(pkt);*/
}
bool AuthConnection::HandleCommandReconnectProof()
{
    _status = STATUS_CLOSED;
    cAuthReconnectProof* reconnectLogonProof = reinterpret_cast<cAuthReconnectProof*>(GetByteBuffer().GetReadPointer());

    if (!_reconnectProof.GetBytes() || !K.GetBytes())
        return false;

    BigNumber t1;
    t1.Bin2BN(reconnectLogonProof->R1, 16);

    SHA1Hasher sha;
    sha.Init();
    sha.UpdateHash(username);
    sha.UpdateHashForBn(3, &t1, &_reconnectProof, &K);
    sha.Finish();

    if (!memcmp(sha.GetData(), reconnectLogonProof->R2, SHA_DIGEST_LENGTH))
    {
        // Sending response
        Common::ByteBuffer pkt;
        pkt.Write<uint8_t>(AUTH_RECONNECT_PROOF);
        pkt.Write<uint8_t>(0x00);
        uint16_t unk1 = 0x00;
        pkt.Append((uint8_t*)&unk1, sizeof(unk1));  // 2 bytes zeros
        Send(pkt);
        _status = STATUS_AUTHED;
        return true;
    }

    return false;
}

bool AuthConnection::HandleCommandGameServerList()
{
    _status = STATUS_WAITING_FOR_GAMESERVER;

    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, "SELECT id, name, address, type, flags, timezone, population FROM realms;", [this](amy::result_set& results, DatabaseConnector& connector)
    {
        std::vector<uint8_t> realmCharacterData(MAX_REALM_COUNT);
        std::fill(realmCharacterData.begin(), realmCharacterData.end(), 0);

        std::shared_ptr<DatabaseConnector> borrowedConnector;
        DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, borrowedConnector);
        amy::result_set realmCharacterCountResult;

        PreparedStatement realmCharacterCount("SELECT realmid, characters FROM realm_characters WHERE account={u};");
        realmCharacterCount.Bind(accountGuid);
        if (borrowedConnector->Query(realmCharacterCount, realmCharacterCountResult))
        {
            for (auto row : realmCharacterCountResult)
            {
                realmCharacterData[row[0].as<amy::sql_tinyint_unsigned>()] = row[1].as<amy::sql_tinyint_unsigned>();
            }
        }

        Common::ByteBuffer realmBuffer;
        for (auto row : results)
        {
            sAuthLogonGameListData realmData;
            realmData.Id = row[0].as<amy::sql_tinyint_unsigned>();
            realmData.Name = row[1].as<amy::sql_varchar>();
            realmData.Address = row[2].as<amy::sql_varchar>();
            realmData.Type = row[3].as<amy::sql_tinyint_unsigned>();
            realmData.Flags = row[4].as<amy::sql_tinyint_unsigned>();
            realmData.Timezone = row[5].as<amy::sql_tinyint_unsigned>();
            realmData.Population = row[6].as<amy::sql_float>();
            realmData.Characters = realmCharacterData[realmData.Id];
            realmData.Locked = 0;

            realmData.AddTo(realmBuffer);
        }

        // (Only needed for clients TBC+)
        realmBuffer.Write<uint8_t>(0x10); // Unk1
        realmBuffer.Write<uint8_t>(0x00); // Unk2

        Common::ByteBuffer RealmListSizeBuffer;
        RealmListSizeBuffer.Write<uint32_t>(0);
        RealmListSizeBuffer.Write<uint16_t>((uint16_t)results.affected_rows());

        Common::ByteBuffer hdr;
        hdr.Write<uint8_t>(AUTH_GAMESERVER_LIST);

        uint16_t combinedSize = realmBuffer.size() + RealmListSizeBuffer.size();
        hdr.Write<uint16_t>(combinedSize);
        hdr.Append(RealmListSizeBuffer);
        hdr.Append(realmBuffer);
        Send(hdr);

        _status = STATUS_AUTHED;
    });

    return true;
}