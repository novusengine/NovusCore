#include <NovusTypes.h>
#include "MessageHandler.h"

MessageHandler* MessageHandler::_instance = nullptr;
MessageHandler::MessageHandler()
{
    for (i32 i = 0; i < OpcodeCount; i++)
    {
        handlers[i] = nullptr;
    }
}

void MessageHandler::SetMessageHandler(u32 opcode, MessageHandlerFunc func)
{
    handlers[opcode] = func;
}

bool MessageHandler::CallHandler(NetPacket* packet)
{
    return handlers[packet->opcode] ? handlers[packet->opcode](packet) : true;
}
