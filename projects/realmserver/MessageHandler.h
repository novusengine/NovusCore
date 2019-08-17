#pragma once
#include <NovusTypes.h>
#include <Networking/Opcode/Opcode.h>

class RealmConnection;
typedef bool (*MessageHandlerFunc)(RealmConnection*);
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
    bool CallHandler(Opcode opcode, RealmConnection* connection);

private:
    static MessageHandler* _instance;
    MessageHandlerFunc handlers[Opcode::NUM_MSG_TYPES];
};