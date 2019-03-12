#pragma once
#include <string>

struct Message
{
	Message() { code = -1; param = -1; message = nullptr; }

	int code;
	int param;
	std::string* message;
};