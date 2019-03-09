#pragma once
#include "Utils/ConcurrentQueue.h"

enum InputMessages
{
	MSG_IN_EXIT,
	MSG_IN_PING
};

enum OutputMessages
{
	MSG_OUT_EXIT_CONFIRM,
	MSG_OUT_PRINT
};

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

class WorldServerHandler
{
public:
	WorldServerHandler();
	~WorldServerHandler();

	void Start();
	void Stop();

	void PassMessage(Message& message);
	bool TryGetMessage(Message& message);

private:
	void Run();
	bool Update(float deltaTime);

private:
	bool _isRunning;

	ConcurrentQueue<Message> _inputQueue;
	ConcurrentQueue<Message> _outputQueue;
};