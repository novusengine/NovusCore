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

#include <Config\ConfigHandler.h>
#include <Database\DatabaseConnector.h>
#include <Utils/DebugHandler.h>

#include "ConnectionHandlers/ClientAuthConnectionHandler.h"
#include "ConnectionHandlers/RelayNodeConnectionHandler.h"

int main()
{
    /* Initialize Debug Handler */
    InitDebugger(PROGRAM_TYPE::Auth);

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
    if (!ConfigHandler::Load("authserver_configuration.json"))
    {
        std::getchar();
        return 0;
    }

    asio::io_service io_service(2);
    ClientAuthConnectionHandler clientAuthConnectionHandler(io_service, ConfigHandler::GetOption<u16>("port", 3724));
    RelayNodeConnectionHandler relayNodeConnectionHandler(io_service, ConfigHandler::GetOption<u16>("nodeport", 9000));
    
    clientAuthConnectionHandler.Start();
    relayNodeConnectionHandler.Start();

    srand((uint32_t)time(NULL));
    std::thread run_thread([&]
    {
        io_service.run();
    });

    std::stringstream ss;
    ss << "Authserver Instance running on port: " << clientAuthConnectionHandler.GetPort();
    NC_LOG_MESSAGE(ss.str());

    std::stringstream _ss;
    _ss << "Authserver Node Instance running on port: " << relayNodeConnectionHandler.GetPort();
    NC_LOG_MESSAGE(_ss.str());

    std::getchar();
    return 0;
}
