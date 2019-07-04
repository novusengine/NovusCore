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
#pragma once

#include <asio/ip/tcp.hpp>
#include <Networking/BaseSocket.h>
#include <Networking/Opcode/Opcode.h>
#include <Cryptography/StreamEncryption.h>
#include <Cryptography/BigNumber.h>
#include <Cryptography/SHA1.h>
#include <random>

#include "../DatabaseCache/AuthDatabaseCache.h"
#include "../DatabaseCache/CharacterDatabaseCache.h"

#pragma pack(push, 1)
struct cAuthSessionData
{
    u32 build;
    u32 loginServerId;
    std::string accountName;
    u32 loginServerType;
    u32 localChallenge;
    u32 regionId;
    u32 battlegroupId;
    u32 realmId;
    u64 dosResponse;
    u8 digest[SHA_DIGEST_LENGTH];

    void Read(DataStore& buffer)
    {
        buffer.GetU32(build);
        buffer.GetU32(loginServerId);
        buffer.GetString(accountName);
        buffer.GetU32(loginServerType);
        buffer.GetU32(localChallenge);
        buffer.GetU32(regionId);
        buffer.GetU32(battlegroupId);
        buffer.GetU32(realmId);
        buffer.GetU64(dosResponse);
        buffer.GetBytes(digest, 20);
    }
};

struct cCharacterCreateData
{
    std::string charName;
    u8 charRace;
    u8 charClass;
    u8 charGender;
    u8 charSkin;
    u8 charFace;
    u8 charHairStyle;
    u8 charHairColor;
    u8 charFacialStyle;
    u8 charOutfitId;

    void Read(DataStore& buffer)
    {
        buffer.GetString(charName);
        buffer.GetU8(charRace);
        buffer.GetU8(charClass);
        buffer.GetU8(charGender);
        buffer.GetU8(charSkin);
        buffer.GetU8(charFace);
        buffer.GetU8(charHairStyle);
        buffer.GetU8(charHairColor);
        buffer.GetU8(charFacialStyle);
        buffer.GetU8(charOutfitId);
    }
};
#pragma pack(pop)

class RealmConnection : public BaseSocket
{
public:
    RealmConnection(asio::ip::tcp::socket* socket, AuthDatabaseCache& authCache, CharacterDatabaseCache& charCache, bool resumeConnection) : BaseSocket(socket), account(0), _headerBuffer(nullptr, sizeof(ClientPacketHeader)), _packetBuffer(nullptr, 4096), _authCache(authCache), _charCache(charCache)
    {
        _resumeConnection = resumeConnection;
        _seed = static_cast<u32>(rand());
        sessionKey = new BigNumber();
    }

    bool Start() override;
    void HandleRead() override;
    void SendPacket(DataStore& buffer, Opcode opcode);

    bool HandleNewHeader();
    bool HandleNewPacket();
    void HandleAuthSession();

    template <size_t T>
    inline void convert(char* val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    inline void apply(u16* val)
    {
        convert<sizeof(u16)>(reinterpret_cast<char*>(val));
    }
    inline void EndianConvertReverse(u16& val) { apply(&val); }

    u32 account;
    u64 characterGuid;
    cAuthSessionData sessionData;
    BigNumber* sessionKey;

private:
    DataStore _headerBuffer;
    DataStore _packetBuffer;

    bool _resumeConnection;
    u32 _seed;
    StreamEncryption _streamEncryption;

    AuthDatabaseCache& _authCache;
    CharacterDatabaseCache& _charCache;
};

template <>
inline void RealmConnection::convert<0>(char*) {}
template <>
inline void RealmConnection::convert<1>(char*) {} // ignore central byte
