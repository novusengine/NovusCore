#pragma once
#include "../WorldServerHandler.h"
#include "../Message.h"

void QuitCommand(WorldServerHandler& worldServerHandler, std::vector<std::string> subCommands)
{
	Message exitMessage;
	exitMessage.code = MSG_IN_EXIT;
	worldServerHandler.PassMessage(exitMessage);
}