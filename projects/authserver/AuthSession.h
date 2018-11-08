#include <asio\ip\tcp.hpp>
#include "../common/BaseSocket.h"

class AuthSession : public Common::BaseSocket
{
public:
    AuthSession(asio::ip::tcp::socket* socket) : Common::BaseSocket(socket)
    { 
    }

    void Start() override;
    void HandleRead() override;
    void HandleWrite() override;
};
