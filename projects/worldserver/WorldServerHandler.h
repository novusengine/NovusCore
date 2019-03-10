#pragma once
#include "Utils/ConcurrentQueue.h"
#include "Message.h"

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