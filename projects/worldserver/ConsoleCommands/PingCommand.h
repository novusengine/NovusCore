#pragma once
#include "../WorldServerHandler.h"
#include "../Message.h"

void PingCommand(WorldServerHandler& worldServerHandler, std::vector<std::string> subCommands)
{
	Message pingMessage;
	pingMessage.code = MSG_IN_PING;
	worldServerHandler.PassMessage(pingMessage);
}