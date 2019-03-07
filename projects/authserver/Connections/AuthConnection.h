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

#include <asio\ip\tcp.hpp>
#include <Networking\BaseSocket.h>
#include <Cryptography\BigNumber.h>
#include <Cryptography\SHA1.h>
#include <Database\DatabaseConnector.h>
#include <unordered_map>

enum AuthCommand
{
    AUTH_CHALLENGE              = 0x00,
    AUTH_PROOF                  = 0x01,
    AUTH_RECONNECT_CHALLENGE    = 0x02,
    AUTH_RECONNECT_PROOF        = 0x03,
    AUTH_GAMESERVER_LIST        = 0x10,
    /*
    TRANSFER_INITIATE           = 0x30,
    TRANSFER_DATA               = 0x31,
    TRANSFER_ACCEPT             = 0x32,
    TRANSFER_RESUME             = 0x33,
    TRANSFER_CANCEL             = 0x34
    */
};
enum AuthStatus
{
    STATUS_CHALLENGE                = 0,
    STATUS_PROOF                    = 1,
    STATUS_RECONNECT_PROOF          = 2,
    STATUS_AUTHED                   = 3,
    STATUS_WAITING_FOR_GAMESERVER   = 4,
    STATUS_CLOSED                   = 5 
};

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
    uint16_t  unk3;
};

struct cAuthReconnectProof
{
    uint8_t   cmd;
    uint8_t   R1[16];
    uint8_t   R2[20];
    uint8_t   R3[20];
    uint8_t   number_of_keys;
};
#pragma pack(pop)

struct AuthMessageHandler;
class AuthConnection : public Common::BaseSocket
{
public:
    static std::unordered_map<uint8_t, AuthMessageHandler> InitMessageHandlers();

    AuthConnection(asio::ip::tcp::socket* socket) : Common::BaseSocket(socket), _status(STATUS_CHALLENGE), username(), packetsReadThisRead(17)
    { 
        N.Hex2BN("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
        g.SetUInt32(7);

        ResetPacketsReadThisRead();
    }

    bool Start() override;
    void HandleRead() override;

    bool HandleCommandChallenge();
    void HandleCommandChallengeCallback(amy::result_set& results);

    bool HandleCommandReconnectChallenge();
    bool HandleCommandProof();
    bool HandleCommandReconnectProof();
    bool HandleCommandGameServerList();

    BigNumber N, s, g, v;
    BigNumber b, B;
    BigNumber K;
    BigNumber _reconnectProof;
    AuthStatus _status;

    std::string username;

    void ResetPacketsReadThisRead() 
    { 
        for (uint8_t i = 0; i < 4; ++i)
        {
            packetsReadThisRead[i] = 0;

            if (i == 3)
            {
                packetsReadThisRead[AUTH_GAMESERVER_LIST] = 0;
            }
        }
    }
    std::vector<uint8_t> packetsReadThisRead;
};

#pragma pack(push, 1)
struct AuthMessageHandler
{
    AuthStatus status;
    size_t packetSize;
    uint8_t maxPacketsPerRead;
    bool (AuthConnection::*handler)();
};
#pragma pack(pop)