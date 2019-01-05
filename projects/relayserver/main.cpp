/*
# MIT License

# Copyright(c) 2018 NovusCore

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

#include "asio.hpp"
#include "RelaySocketHandler.h"
#include "Socket\RelayNodeConnection.h"
#include "Networking\ByteBuffer.h"
#include <Config\ConfigHandler.h>
#include <Database\DatabaseConnector.h>

int main()
{
    if (!ConfigHandler::Setup("relayserver_configuration.json"))
    {
        std::getchar();
        return 0;
    }
	DatabaseConnector::Setup("127.0.0.1", "root", "");

    // Seed Rand
    srand(time(NULL));

    asio::io_service io_service(2);
    RelaySocketHandler server(io_service, ConfigHandler::GetOption<uint16_t>("port", 8085));
    server.Start();
    
    RelayNodeConnection nodeConnection(new asio::ip::tcp::socket(io_service), ConfigHandler::GetOption<std::string>("authserverip", "127.0.0.1"), ConfigHandler::GetOption<uint16_t>("authserverport", 3724));
    if (!nodeConnection.Start())
    {
        std::getchar();
        return 0;
    }
    std::cout << "Connected to authserver" << std::endl;

    /*asio::error_code error;
    asio::write(socket, asio::buffer(buffer.GetReadPointer(), buffer.GetActualSize()), error);

    if (!error)
    {
        std::cout << "Sent message to authserver" << std::endl;
    }
    else
    {
        std::cout << "Failed to sent message to authserver" << std::endl;
    }*/

    std::thread run_thread([&]
    {
        io_service.run();
    });

    std::getchar();
	return 0;
}