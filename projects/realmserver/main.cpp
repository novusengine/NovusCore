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

#include <Config/ConfigHandler.h>
#include <Database/DatabaseConnector.h>
#include <Utils/DebugHandler.h>

#include "DatabaseCache/CharacterDatabaseCache.h"
#include "ConnectionHandlers/RealmConnectionHandler.h"

#ifdef _WIN32
#include <Windows.h>
#endif

RealmConnectionHandler*   RealmConnectionHandler::_instance = nullptr;

//The name of the console window.
#define WINDOWNAME "Realm Server"

i32 main()
{
	/* Set up console window title */
#ifdef _WIN32  //Windows
	SetConsoleTitle(WINDOWNAME);
#endif

    /* Load Database Config Handler for server */
    if (!ConfigHandler::Load("database.json"))
    {
        std::getchar();
        return 0;
    }

    /* Load Database Information here */
    std::string hosts       [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_ip", "127.0.0.1"),         ConfigHandler::GetOption<std::string>("character_database_ip", "127.0.0.1"),        ConfigHandler::GetOption<std::string>("world_database_ip", "127.0.0.1"),        ConfigHandler::GetOption<std::string>("dbc_database_ip", "127.0.0.1") };
    u16 ports               [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<u16>("auth_database_port", 3306),                      ConfigHandler::GetOption<u16>("character_database_port", 3306),                     ConfigHandler::GetOption<u16>("world_database_port", 3306),                     ConfigHandler::GetOption<u16>("dbc_database_port", 3306) };
    std::string usernames   [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_user", "root"),            ConfigHandler::GetOption<std::string>("character_database_user", "root"),           ConfigHandler::GetOption<std::string>("world_database_user", "root"),           ConfigHandler::GetOption<std::string>("dbc_database_user", "root") };
    std::string passwords   [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_password", ""),            ConfigHandler::GetOption<std::string>("character_database_password", ""),           ConfigHandler::GetOption<std::string>("world_database_password", ""),           ConfigHandler::GetOption<std::string>("dbc_database_password", "") };
    std::string names       [DATABASE_TYPE::COUNT]  = { ConfigHandler::GetOption<std::string>("auth_database_name", "auth"),            ConfigHandler::GetOption<std::string>("character_database_name", "characters"),     ConfigHandler::GetOption<std::string>("world_database_name", "world"),          ConfigHandler::GetOption<std::string>("dbc_database_name", "dbc") };

    /* Pass Database Information to Setup */
    DatabaseConnector::Setup(hosts, ports, usernames, passwords, names);

    /* Load Config Handler for server */
    if (!ConfigHandler::Load("realmserver.json"))
    {
        std::getchar();
        return 0;
    }

    CharacterDatabaseCache characterDatabaseCache;
    characterDatabaseCache.Load();

    asio::io_service io_service(2);
    RealmConnectionHandler realmConnectionHandler(io_service,    ConfigHandler::GetOption<u16>("port", 8000), characterDatabaseCache);
    realmConnectionHandler.Start();

    srand(static_cast<u32>(time(NULL)));
    std::thread run_thread([&]
    {
        io_service.run();
    });

    NC_LOG_SUCCESS("Realmserver running on port: %u", realmConnectionHandler.GetPort());

    std::getchar();
	return 0;
}
