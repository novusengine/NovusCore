#pragma once

#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <functional>

#include <Utils/StringHash.h>
#include <Utils/DebugHandler.h>

#include "ConsoleCommands/QuitCommand.h"
#include "ConsoleCommands/PingCommand.h"

std::vector<std::string> SplitString(std::string string)
{
	std::istringstream iss(string);
	std::vector<std::string> results(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());
	return results;
}

class ConsoleCommandHandler
{
public:
	ConsoleCommandHandler()
	{
		RegisterCommand("quit"_h, &QuitCommand);
		RegisterCommand("ping"_h, &PingCommand);
	}

	void HandleCommand(WorldServerHandler& worldServerHandler, std::string& command)
	{
		if (command.size() == 0)
			return;

		std::vector<std::string> splitCommand = SplitString(command);
		uint32_t hashedCommand = detail::fnv1a_32(splitCommand[0].c_str(), splitCommand[0].size());

		auto commandHandler = commandHandlers.find(hashedCommand);
		if (commandHandler != commandHandlers.end())
		{
			splitCommand.erase(splitCommand.begin());
			commandHandler->second(worldServerHandler, splitCommand);
		}
		else
			NC_LOG_WARNING("Unhandled command: " + splitCommand[0]);
	}
private:
	void RegisterCommand(uint32_t id, const std::function<void(WorldServerHandler&, std::vector<std::string>)>& handler)
	{
		commandHandlers.insert_or_assign(id, handler);
	}

	std::map<uint16_t, std::function<void(WorldServerHandler&, std::vector<std::string>)>> commandHandlers = {};
};
