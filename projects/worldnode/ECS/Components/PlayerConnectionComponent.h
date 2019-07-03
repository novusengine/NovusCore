/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include <Networking/DataStore.h>
#include "../../Utils/CharacterUtils.h"
#include "../../Game/ObjectGuid/ObjectGuid.h"
#include <vector>

struct NetPacket
{
    u16 opcode;
    bool handled;
    std::shared_ptr<DataStore> data;
};

class WorldConnection;
struct PlayerConnectionComponent
{
    template <typename... Args>
    void SendChatNotification(std::string message, Args... args)
    {
        std::shared_ptr<DataStore> packet = CharacterUtils::BuildNotificationPacket(message, std::forward<Args>(args)...);
        socket->SendPacket(packet.get(), Opcode::SMSG_MESSAGECHAT);
    }
    template <typename... Args>
    void SendConsoleNotification(std::string message, Args... args)
    {
        char str[256];
        StringUtils::FormatString(str, sizeof(str), message.c_str(), args...);

        std::shared_ptr<DataStore> packet = DataStore::Borrow<256>();
        packet->PutString(str);
        socket->SendPacket(packet.get(), Opcode::SMSG_NOTIFICATION);
    }

    u32 entityId;
    u32 accountGuid;
    ObjectGuid characterGuid;
    WorldConnection* socket;
    std::vector<NetPacket> packets;
};
