#include <iostream>
#include <chrono>
#include <future>
#include <algorithm>

#include <Config/ConfigHandler.h>
#include <Database/DatabaseConnector.h>
#include <Utils/DebugHandler.h>

#include "ConnectionHandlers/WorldConnectionHandler.h"

#include "WorldNodeHandler.h"
#include "Message.h"

#include "ConsoleCommands.h"

#ifdef _WIN32
#include <Windows.h>
#endif

std::string GetLineFromCin()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

WorldConnectionHandler* WorldConnectionHandler::_instance = nullptr;
//The name of the console window.
#define WINDOWNAME "World Node"

i32 main()
{
    /* Set up console window title */
#ifdef _WIN32 //Windows
    SetConsoleTitle(WINDOWNAME);
#endif

    /* Load Database Config Handler for server */
    if (!ConfigHandler::Load("database.json"))
    {
        std::getchar();
        return 0;
    }

    /* Load Database Information here */
    DatabaseConnectionDetails dbConnections[DATABASE_TYPE::COUNT];
    dbConnections[DATABASE_TYPE::AUTHSERVER] = DatabaseConnectionDetails(ConfigHandler::GetJsonObjectByKey("auth_database"));
    dbConnections[DATABASE_TYPE::CHARSERVER] = DatabaseConnectionDetails(ConfigHandler::GetJsonObjectByKey("character_database"));
    dbConnections[DATABASE_TYPE::WORLDSERVER] = DatabaseConnectionDetails(ConfigHandler::GetJsonObjectByKey("world_database"));
    dbConnections[DATABASE_TYPE::DBC] = DatabaseConnectionDetails(ConfigHandler::GetJsonObjectByKey("dbc_database"));

    /* Pass Database Information to Setup */
    DatabaseConnector::Setup(dbConnections);

    /* Load Config Handler for server */
    if (!ConfigHandler::Load("worldnode.json"))
    {
        std::getchar();
        return 0;
    }

    WorldNodeHandler worldNodeHandler(ConfigHandler::GetOption<f32>("tickRate", 30));
    worldNodeHandler.Start();

    asio::io_service io_service(2);
    WorldConnectionHandler WorldConnectionHandler(io_service, ConfigHandler::GetOption<u16>("port", 8001), &worldNodeHandler);

    std::thread* run_thread;

    ConsoleCommandHandler consoleCommandHandler;
    auto future = std::async(std::launch::async, GetLineFromCin);
    while (true)
    {
        Message message;
        bool shouldExit = false;
        while (worldNodeHandler.TryGetMessage(message))
        {
            if (message.code == MSG_OUT_EXIT_CONFIRM)
            {
                shouldExit = true;
                break;
            }
            else if (message.code == MSG_OUT_PRINT)
            {
                NC_LOG_MESSAGE(*message.message);
                //std::cout <<  << std::endl;
                delete message.message;
            }
            else if (message.code == MSG_OUT_SETUP_COMPLETE)
            {
                WorldConnectionHandler.Start();

                srand(static_cast<u32>(time(NULL)));
                run_thread = new std::thread([&] {
                    io_service.run();
                });
                NC_LOG_SUCCESS("Worldnode running on port: %u", WorldConnectionHandler.GetPort());
            }
        }

        if (shouldExit)
            break;

        if (future.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready)
        {
            std::string command = future.get();
            std::transform(command.begin(), command.end(), command.begin(), ::tolower); // Convert command to lowercase

            consoleCommandHandler.HandleCommand(worldNodeHandler, command);
            future = std::async(std::launch::async, GetLineFromCin);
        }
    }

    std::string message = "--- Thank you for flying with NovusCore, press enter to exit --- ";
    for (i32 i = 0; i < message.size() - 1; i++)
        std::cout << "-";
    std::cout << std::endl
              << message << std::endl;
    for (i32 i = 0; i < message.size() - 1; i++)
        std::cout << "-";
    std::cout << std::endl;

    io_service.stop();
    while (!io_service.stopped())
    {
        std::this_thread::yield();
    }
    return 0;
}