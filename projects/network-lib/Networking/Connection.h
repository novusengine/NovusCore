#pragma once
#include <NovusTypes.h>
#include "BaseSocket.h"

#pragma pack(push, 1)
struct cAuthLogonChallenge
{
    cAuthLogonChallenge(std::string _username) : gamename{ 'W', 'o', 'W', 0 },
        platform{ '6', '8', 'x', 0 },
        os{ 'n', 'i', 'W', 0 },
        country{ 'S', 'U', 'n', 'e' }
    {
        username_length = (u8)_username.length();
        std::memcpy(username, _username.data(), _username.length());

        command = 0x00;
        error = 8;
        size = 30 + username_length;
        version1 = 3;
        version2 = 3;
        version3 = 5;
        build = 12340;
        timezone_bias = 60;
        ip = 16777343;
    }
    u8  command;
    u8  error;
    u16 size;
    u8  gamename[4];
    u8  version1;
    u8  version2;
    u8  version3;
    u16 build;
    u8  platform[4];
    u8  os[4];
    u8  country[4];
    u32 timezone_bias;
    u32 ip;
    u8  username_length;
    char username[16];
};
#pragma pack(pop)

class Connection : public BaseSocket
{
public:
    Connection(asio::ip::tcp::socket* socket) : BaseSocket(socket) { }

    void Start() override;
    void HandleRead() override;
};