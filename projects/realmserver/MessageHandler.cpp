#include <NovusTypes.h>
#include "MessageHandler.h"

MessageHandler* MessageHandler::_instance = nullptr;
MessageHandler::MessageHandler()
{
    for (i32 i = 0; i < Opcode::NUM_MSG_TYPES; i++)
    {
        handlers[i] = nullptr;
    }
}

void MessageHandler::SetMessageHandler(Opcode opcode, MessageHandlerFunc func)
{
    handlers[opcode] = func;
}

bool MessageHandler::CallHandler(Opcode opcode, RealmConnection* connection)
{
    return handlers[opcode] ? handlers[opcode](connection) : true;
}
