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
    InitDebugger(PROGRAM_TYPE::World);

    /* Load Database Config Handler for server */
    if (!ConfigHandler::Load("database_configuration.json"))
    {
        std::getchar();
        return 0;
    }
    
    /* Load Database Information here */
    std::string hosts       [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_ip", "127.0.0.1"),         ConfigHandler::GetOption<std::string>("character_database_ip", "127.0.0.1"),        ConfigHandler::GetOption<std::string>("world_database_ip", "127.0.0.1"),        ConfigHandler::GetOption<std::string>("dbc_database_ip", "127.0.0.1") };   
    u16 ports               [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<u16>("auth_database_port", 3306),                      ConfigHandler::GetOption<u16>("character_database_port", 3306),                     ConfigHandler::GetOption<u16>("world_database_port", 3306),                     ConfigHandler::GetOption<u16>("dbc_database_port", 3306) };   
    std::string usernames   [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_user", "root"),            ConfigHandler::GetOption<std::string>("character_database_user", "root"),           ConfigHandler::GetOption<std::string>("world_database_user", "root"),           ConfigHandler::GetOption<std::string>("dbc_database_user", "root") };
    std::string passwords   [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_password", ""),            ConfigHandler::GetOption<std::string>("character_database_password", ""),           ConfigHandler::GetOption<std::string>("world_database_password", ""),           ConfigHandler::GetOption<std::string>("dbc_database_password", "") };
    std::string names       [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_name", "authserver"),      ConfigHandler::GetOption<std::string>("character_database_name", "charserver"),     ConfigHandler::GetOption<std::string>("world_database_name", "worldserver"),    ConfigHandler::GetOption<std::string>("dbc_database_name", "dbcdata") };
    
    /* Pass Database Information to Setup */
    DatabaseConnector::Setup(hosts, ports, usernames, passwords, names);

    /* Load Config Handler for server */
	if (!ConfigHandler::Load("worldserver_configuration.json"))
	{
		std::getchar();
		return 0;
	}

    asio::io_service io_service(2);
    NovusConnection novusConnection(new asio::ip::tcp::socket(io_service), ConfigHandler::GetOption<std::string>("relayserverip", "127.0.0.1"), ConfigHandler::GetOption<uint16_t>("relayserverport", 10000), ConfigHandler::GetOption<uint8_t>("realmId", 1));

    if (!novusConnection.Start())
    {
        std::getchar();
        return 0;
    }

    srand((uint32_t)time(NULL));
    std::thread run_thread([&]
    {
        io_service.run();
    });

    NC_LOG_MESSAGE("Worldserver established node connection to Relayserver.");

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