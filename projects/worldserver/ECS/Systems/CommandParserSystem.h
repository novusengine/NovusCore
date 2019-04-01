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
#include <entt.hpp>

#include "../NovusEnums.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

namespace CommandParserSystem
{
    std::vector<std::string> SplitString(std::string string)
    {
        std::istringstream iss(string);
        std::vector<std::string> results(std::istream_iterator<std::string>{iss},
            std::istream_iterator<std::string>());
        return results;
    }

	void Update(entt::registry &registry) 
    {
        SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent>();

        view.each([&singleton, &novusConnection](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
        {
            for (ChatUpdateData& command : clientUpdateData.chatUpdateData)
            {
                std::string commandMessage = command.message;

                /* Check if we are dealing with a command */
                if (commandMessage[0] == '.')
                {
                    std::vector<std::string> splitCommand = SplitString(commandMessage);

                    if (splitCommand[0] == ".level")
                    {
                        u32 playerLevel = clientUpdateData.GetFieldValue<u32>(UNIT_FIELD_LEVEL);
                        if (splitCommand.size() >= 2)
                        {
                            try
                            {
                                u32 level = std::stoi(splitCommand[1]);
                                if (level == 0)
                                    level = 1;
                                else if (level > 255)
                                    level = 255;

                                if (playerLevel != level)
                                    clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, level);

                                /* I put this in here so that we can unlock the achievement frame when reaching level 10 */
                                if (level >= 10 && playerLevel < 10)
                                {
                                    Common::ByteBuffer achivementData;
                                    NovusHeader packetHeader;
                                    packetHeader.command = NOVUS_FORWARDPACKET;
                                    packetHeader.account = clientConnection.accountGuid;
                                    packetHeader.opcode = Common::Opcode::SMSG_ACHIEVEMENT_EARNED;
                                    packetHeader.size = 2 + 4 + 4 + 4;
                                    packetHeader.AddTo(achivementData);

                                    achivementData.AppendGuid(clientConnection.characterGuid);
                                    achivementData.Write<u32>(6); // Level 10

                                    tm lt;
                                    time_t const tmpServerTime = time(nullptr);
                                    localtime_s(&lt, &tmpServerTime);
                                    achivementData.Write<u32>(((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min));
                                    achivementData.Write<u32>(0);

                                    novusConnection.SendPacket(achivementData);
                                }

                                command.handled = true;
                            }
                            catch (std::exception e)
                            {

                            }
                        }
                        else
                        {
                            if (playerLevel < 255)
                                clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, playerLevel + 1);

                            command.handled = true;
                        }
                    }
                }
            }

            if (clientUpdateData.chatUpdateData.size() > 0)
            {
                clientUpdateData.chatUpdateData.erase(std::remove_if(clientUpdateData.chatUpdateData.begin(), clientUpdateData.chatUpdateData.end(), [](ChatUpdateData& chatData)
                {
                    return chatData.handled;
                }), clientUpdateData.chatUpdateData.end());
            }
        });
	}
}
