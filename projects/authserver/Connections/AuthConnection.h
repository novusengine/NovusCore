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

#include <NovusTypes.h>
#include <Networking/BaseSocket.h>
#include <Cryptography/BigNumber.h>
#include <Database/DatabaseConnector.h>
#include <robin_hood.h>
#include "../AuthEnums.h"

#pragma pack(push, 1)
class AuthConnection;
struct AuthMessageHandler
{
    AuthStatus status;
    size_t packetSize;
    u8 maxPacketsPerRead;
    bool (AuthConnection::*handler)();
};

struct RealmServerData
{
    std::string realmName;
    std::string realmAddress;
    u8 type;
    u8 flags;
    u8 timeZone;
    f32 population;
};
#pragma pack(pop)

class AuthConnection : public BaseSocket
{
public:
    static robin_hood::unordered_map<u8, AuthMessageHandler> InitMessageHandlers();
    static robin_hood::unordered_map<u8, RealmServerData> realmServerList;
    static std::mutex realmServerListMutex;

    AuthConnection(asio::ip::tcp::socket* socket)
        : BaseSocket(socket), _status(STATUS_CHALLENGE), username()
    {
        nPrime.Hex2BN("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7");
        generator.SetUInt32(7);

        ResetPacketsReadOfType();
    }

    bool Start() override;
    void HandleRead() override;

    bool HandleCommandChallenge();
    void HandleCommandChallengeCallback(amy::result_set& results);

    bool HandleCommandReconnectChallenge();
    void HandleCommandReconnectChallengeCallback(amy::result_set& results);
    bool HandleCommandProof();
    bool HandleCommandReconnectProof();
    bool HandleCommandRealmserverList();

    BigNumber nPrime, smallSalt, generator, clientVerifier;
    BigNumber random, ephemeralKeyB;
    BigNumber sessionKey;
    BigNumber _reconnectSeed;
    AuthStatus _status;

    std::string username;
    u32 accountGuid;

    void ResetPacketsReadOfType()
    {
        packetsReadOfType[AUTH_CHALLENGE] = 0;
        packetsReadOfType[AUTH_PROOF] = 0;
        packetsReadOfType[AUTH_RECONNECT_CHALLENGE] = 0;
        packetsReadOfType[AUTH_RECONNECT_PROOF] = 0;
        packetsReadOfType[AUTH_REALMSERVER_LIST] = 0;
    }
    u8 packetsReadForType = 0;
    robin_hood::unordered_map<u8, u8> packetsReadOfType;
};