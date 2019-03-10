#pragma once
#include <string>

struct Message
{
	Message() noexcept { code = -1; param = -1; message = nullptr; }

	Message(int inCode, int inParam, std::string* inMessage) // This is needed to use this struct in a ConcurrentQueue, 
	{
		code = inCode; param = inParam; message = inMessage;
	}

	Message& operator=(Message rhs) noexcept // This is needed to use this struct in a ConcurrentQueue
	{
		code = rhs.code;
		param = rhs.param;
		message = rhs.message;
		return *this;
	}

	int code;
	int param;
	std::string* message;
};