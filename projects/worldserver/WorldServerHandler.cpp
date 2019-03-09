#include "WorldServerHandler.h"

#include <thread>

WorldServerHandler::WorldServerHandler()
	: _isRunning(false)
	, _inputQueue(256)
	, _outputQueue(256)
{

}

WorldServerHandler::~WorldServerHandler()
{

}

void WorldServerHandler::PassMessage(Message& message)
{
	_inputQueue.push(message);
}

bool WorldServerHandler::TryGetMessage(Message& message)
{
	return _outputQueue.try_pop(message);
}

void WorldServerHandler::Start()
{
	if (_isRunning)
		return;

	std::thread thread = std::thread(&WorldServerHandler::Run, this);
	thread.detach();
}

void WorldServerHandler::Stop()
{
	if (!_isRunning)
		return;

	Message message;
	message.code = MSG_IN_EXIT;
	PassMessage(message);
}

void WorldServerHandler::Run()
{
	while (true)
	{
		if (!Update(1.0f))
			break;
	}

	// Clean up stuff here

	Message exitMessage;
	exitMessage.code = MSG_OUT_EXIT_CONFIRM;
	_outputQueue.push(exitMessage);
}

bool WorldServerHandler::Update(float deltaTime)
{
	Message message;
	while (_inputQueue.try_pop(message))
	{
		if (message.code == -1)
			assert(false);

		if (message.code == MSG_IN_EXIT)
			return false;

		if (message.code == MSG_IN_PING)
		{
			Message pongMessage;
			pongMessage.code = MSG_OUT_PRINT;
			pongMessage.message = new std::string("PONG!");
			_outputQueue.push(pongMessage);
		}
	}
	return true;
}
