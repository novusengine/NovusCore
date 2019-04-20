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
#include <Utils/StringUtils.h>
#include <entt.hpp>

#include "../NovusEnums.h"
#include "../Components/PlayerConnectionComponent.h"
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

        auto view = registry.view<PlayerConnectionComponent, PlayerUpdateDataComponent>();
        view.each([&singleton, &commandData](const auto, PlayerConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
        {
            for (ChatUpdateData& command : clientUpdateData.chatUpdateData)
            {
                std::string commandMessage = command.message;
                if (command.message[0] == '.')
                {
                    std::vector<std::string> commandStrings = SplitString(command.message.substr(1));
                    auto itr = commandData.commandMap.find(StringUtils::fnv1a_32(commandStrings[0].c_str(), commandStrings[0].length()));
                    if (itr != commandData.commandMap.end())
                    {
						/* 
							We might want to look at how we can improve this, I'm not sure if this is the best way
							This is however a very cold path, so performance shouldn't be much of a worry.
							In short we extract the parameters based on how many there are and the depth of the command
						*/

						i32 parameters = itr->second.parameters;
						if (parameters == -1)
						{
							if (itr->second.handler(commandStrings, clientConnection))
								command.handled = true;
						}
						else
						{
							std::vector<std::string> paramStrings;
							if (parameters > 0)
							{
								// Skip if an insufficient amount of parameters were specified
								if (commandStrings.size() - 1 < parameters)
									continue;

								for (i32 i = 1; i < parameters + 1; i++)
								{
									paramStrings.push_back(commandStrings[i]);
								}
							}

							if (itr->second.handler(paramStrings, clientConnection))
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
