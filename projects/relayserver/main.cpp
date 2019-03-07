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

#include "Connections\NovusConnection.h"
#include "Connections\RelayAuthNodeConnection.h"
#include "ConnectionHandlers\ClientRelayConnectionHandler.h"
#include "ConnectionHandlers\NovusConnectionHandler.h"

NovusConnectionHandler*         NovusConnectionHandler::_instance       = nullptr;
ClientRelayConnectionHandler*   ClientRelayConnectionHandler::_instance = nullptr;
int main()
{
    if (!ConfigHandler::Setup("relayserver_configuration.json"))
    {
        std::getchar();
        return 0;
    }
	DatabaseConnector::Setup("127.0.0.1", "root", "ascent");
    srand((uint32_t)time(NULL));

    asio::io_service io_service(2);
    ClientRelayConnectionHandler clientRelayConnectionHandler(io_service, ConfigHandler::GetOption<uint16_t>("port", 8000));
    NovusConnectionHandler novusConnectionHandler(io_service, ConfigHandler::GetOption<uint16_t>("nodeport", 10000));
    
    RelayAuthNodeConnection relayAuthNodeConnection(new asio::ip::tcp::socket(io_service), ConfigHandler::GetOption<std::string>("authserverip", "127.0.0.1"), ConfigHandler::GetOption<uint16_t>("authservernodeport", 9000));

    if (!relayAuthNodeConnection.Start())
    {
        std::getchar();
        return 0;
    }
    clientRelayConnectionHandler.Start();
    novusConnectionHandler.Start();

    std::cout << "Relayserver Instance running on port: " << clientRelayConnectionHandler.GetPort() << std::endl;
    std::cout << "Relayserver Node Instance running on port: " << novusConnectionHandler.GetPort() << std::endl;
    std::cout << "Relayserver established node connection to Authserver." << std::endl;

    std::thread run_thread([&]
    {
        io_service.run();
    });

    std::getchar();
	return 0;
}