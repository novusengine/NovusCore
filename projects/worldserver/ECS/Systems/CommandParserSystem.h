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
#include <Utils/StringHash.h>
#include <entt.hpp>

#include "../NovusEnums.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/CommandDataSingleton.h"
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
        CommandDataSingleton& commandData = registry.ctx<CommandDataSingleton>();
        NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent>();
        view.each([&singleton, &commandData, &novusConnection](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
        {
            for (ChatUpdateData& command : clientUpdateData.chatUpdateData)
            {
                std::string commandMessage = command.message;
                if (command.message[0] == '.')
                {
                    std::vector<std::string> commandStrings = SplitString(commandMessage);

                    std::string commandStr = commandStrings[0];
                    auto itr = commandData.commandMap.find(detail::fnv1a_32(commandStr.c_str(), commandStr.length()));
                    if (itr != commandData.commandMap.end())
                    {
                        if (itr->second.handler(commandStrings, clientConnection))
                            command.handled = true;
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
