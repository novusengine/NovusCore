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

#include "ConnectionHandlers/AuthConnectionHandler.h"

#ifdef _WIN32
#include <Windows.h>
#endif

//The name of the console window.
#define WINDOWNAME "Authentication Server"

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
    DatabaseConnectionDetails dbConnections[DATABASE_TYPE::COUNT];
    dbConnections[DATABASE_TYPE::AUTHSERVER] = DatabaseConnectionDetails(ConfigHandler::GetJsonObjectByKey("auth_database"));

    /* Pass Database Information to Setup */
    DatabaseConnector::Setup(dbConnections);

    /* Load Config Handler for server */
    if (!ConfigHandler::Load("authserver.json"))
    {
        std::getchar();
        return 0;
    }

    asio::io_service io_service(2);
    AuthConnectionHandler authConnectionHandler(io_service, ConfigHandler::GetOption<u16>("port", 3724));
    authConnectionHandler.Start();

    srand(static_cast<u32>(time(NULL)));
    std::thread run_thread([&]
    {
        io_service.run();
    });

    NC_LOG_SUCCESS("Authserver running on port: %u", authConnectionHandler.GetPort());

    std::getchar();
    return 0;
}
