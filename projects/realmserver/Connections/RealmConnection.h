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
#include <Cryptography/StreamCrypto.h>
#include <Cryptography/BigNumber.h>
#include <Cryptography/SHA1.h>
#include <random>

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
    u8  digest[SHA_DIGEST_LENGTH];

    void Read(Common::ByteBuffer& buffer)
    {
        buffer.Read<u32>(build);
        buffer.Read<u32>(loginServerId);
        buffer.Read(accountName);
        buffer.Read<u32>(loginServerType);
        buffer.Read<u32>(localChallenge);
        buffer.Read<u32>(regionId);
        buffer.Read<u32>(battlegroupId);
        buffer.Read<u32>(realmId);
        buffer.Read<u64>(dosResponse);
        buffer.Read(&digest, 20);
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

    void Read(Common::ByteBuffer& buffer)
    {
        buffer.Read(charName);
        buffer.Read<u8>(charRace);
        buffer.Read<u8>(charClass);
        buffer.Read<u8>(charGender);
        buffer.Read<u8>(charSkin);
        buffer.Read<u8>(charFace);
        buffer.Read<u8>(charHairStyle);
        buffer.Read<u8>(charHairColor);
        buffer.Read<u8>(charFacialStyle);
        buffer.Read<u8>(charOutfitId);
    }
};
#pragma pack(pop)

class RealmConnection : public Common::BaseSocket
{
public:
    RealmConnection(asio::ip::tcp::socket* socket, CharacterDatabaseCache& cache) : Common::BaseSocket(socket), account(0), _headerBuffer(), _packetBuffer(), _cache(cache)
    {
        _seed = static_cast<u32>(rand());
        _headerBuffer.Resize(sizeof(Common::ClientPacketHeader));
        sessionKey = new BigNumber();
    }

    bool Start() override;
    void HandleRead() override;
    void SendPacket(Common::ByteBuffer& buffer, Common::Opcode opcode);

    bool HandleHeaderRead();
    bool HandlePacketRead();

    void HandleAuthSession();
    void HandleContinueAuthSession();

    template<size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    template<> inline void convert<0>(char *) { }
    template<> inline void convert<1>(char *) { } // ignore central byte

    inline void apply(u16 *val)
    {
        convert<sizeof(u16)>(reinterpret_cast<char *>(val));
    }
    inline void EndianConvertReverse(u16& val) { apply(&val); }

    u32 account;
    u64 characterGuid;
    cAuthSessionData sessionData;
    BigNumber* sessionKey;

    Common::ByteBuffer _headerBuffer;
    Common::ByteBuffer _packetBuffer;

    u32 _seed;
    StreamCrypto _streamCrypto;
    CharacterDatabaseCache& _cache;
};