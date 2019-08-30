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

#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <functional>

#include <Utils/StringUtils.h>
#include <Utils/DebugHandler.h>

#include "ConsoleCommands/QuitCommand.h"
#include "ConsoleCommands/PingCommand.h"
#include "ClientHandler.h"

class ConsoleCommandHandler
{
public:
    ConsoleCommandHandler()
    {
        RegisterCommand("quit"_h, &QuitCommand);
        RegisterCommand("ping"_h, &PingCommand);
    }

    void HandleCommand(ClientHandler& clientHandler, std::string& command)
    {
        if (command.size() == 0)
            return;

        std::vector<std::string> splitCommand = StringUtils::SplitString(command);
        u32 hashedCommand = StringUtils::fnv1a_32(splitCommand[0].c_str(), splitCommand[0].size());

        auto commandHandler = commandHandlers.find(hashedCommand);
        if (commandHandler != commandHandlers.end())
        {
            splitCommand.erase(splitCommand.begin());
            commandHandler->second(clientHandler, splitCommand);
        }
        else
        {
            NC_LOG_WARNING("Unhandled command: " + command);
        }
    }

private:
    void RegisterCommand(u32 id, const std::function<void(ClientHandler&, std::vector<std::string>)>& handler)
    {
        commandHandlers.insert_or_assign(id, handler);
    }

    std::map<u16, std::function<void(ClientHandler&, std::vector<std::string>)>> commandHandlers = {};
};
