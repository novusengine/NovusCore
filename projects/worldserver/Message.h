#pragma once
#include <string>
#include <Networking/ByteBuffer.h>

struct Message
{
    Message() { code = -1; opcode = -1; account = -1; message = nullptr; }

	int code;
    i16 opcode;
    i32 account;
    Common::ByteBuffer packet;
	std::string* message;
};