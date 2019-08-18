#pragma once
#include <NovusTypes.h>
#include <Networking/Opcode/Opcode.h>

struct NetPacket;
struct PlayerConnectionComponent;
typedef bool (*MessageHandlerFunc)(NetPacket*, PlayerConnectionComponent*);
class MessageHandler
{
public:
    MessageHandler();

    static MessageHandler* Instance()
    {
        if (!_instance)
            _instance = new MessageHandler();

        return _instance;
    }

    void SetMessageHandler(Opcode opcode, MessageHandlerFunc func);
    bool CallHandler(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);

private:
    static MessageHandler* _instance;
    MessageHandlerFunc handlers[Opcode::NUM_MSG_TYPES];
};