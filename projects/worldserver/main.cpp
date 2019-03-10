#include <iostream>
#include <chrono>
#include <future>
#include <algorithm>

#include <Config\ConfigHandler.h>
#include <Database/DatabaseConnector.h>
#include <Utils/DebugHandler.h>

#include "Connections\NovusConnection.h"

#include "WorldServerHandler.h"
#include "Message.h"

#include "ConsoleCommands.h"

std::string GetLineFromCin() {
	std::string line;
	std::getline(std::cin, line);
	return line;
}

int main()
{
	if (!ConfigHandler::Setup("worldserver_configuration.json"))
	{
		std::getchar();
		return 0;
	}
	InitDebugger(PROGRAM_TYPE::World);

	/*NC_LOG_MESSAGE("This is a message");
	NC_LOG_DEPRECATED("This is a deprecation");
	NC_LOG_WARNING("This is a warning");
	NC_LOG_ERROR("This is a error");
	NC_LOG_FATAL("This is a fatal error");*/

	DatabaseConnector::Setup("127.0.0.1", "root", "ascent");

    asio::io_service io_service(2);
    NovusConnection novusConnection(new asio::ip::tcp::socket(io_service), ConfigHandler::GetOption<std::string>("relayserverip", "127.0.0.1"), ConfigHandler::GetOption<uint16_t>("relayserverport", 10000), ConfigHandler::GetOption<uint8_t>("realmId", 1));

    if (!novusConnection.Start())
    {
        std::getchar();
        return 0;
    }
    std::cout << "Worldserver established node connection to Relayserver." << std::endl;

    std::thread run_thread([&]
    {
        io_service.run();
    });

	ConsoleCommandHandler consoleCommandHandler;

	WorldServerHandler worldServerHandler;
	worldServerHandler.Start();

	auto future = std::async(std::launch::async, GetLineFromCin);
	while (true)
	{
		Message message;
		bool shouldExit = false;
		while (worldServerHandler.TryGetMessage(message))
		{
			if (message.code == MSG_OUT_EXIT_CONFIRM)
			{
				shouldExit = true;
				break;
			}
			if (message.code == MSG_OUT_PRINT)
			{
				NC_LOG_MESSAGE(*message.message);
				//std::cout <<  << std::endl;
				delete message.message;
			}
		}

		if (shouldExit)
			break;

		if (future.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready) 
		{
			std::string command = future.get();
			std::transform(command.begin(), command.end(), command.begin(), ::tolower); // Convert command to lowercase

			consoleCommandHandler.HandleCommand(worldServerHandler, command);
			future = std::async(std::launch::async, GetLineFromCin);
		}
	}

	std::string message = "--- Thank you for flying with NovusCore, press enter to exit --- ";
	for (int i = 0; i < message.size()-1; i++)
		std::cout << "-";
	std::cout << std::endl << message << std::endl;
	for (int i = 0; i < message.size()-1; i++)
		std::cout << "-";
	std::cout << std::endl;
	return 0;
}