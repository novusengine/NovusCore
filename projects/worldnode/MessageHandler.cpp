#include <NovusTypes.h>
#include "MessageHandler.h"

#include "ECS/Components/PlayerConnectionComponent.h"

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

bool MessageHandler::CallHandler(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    return handlers[packet->opcode] ? handlers[packet->opcode](packet, playerConnectionComponent) : true;
}
