#include "WorldServerHandler.h"
#include <thread>


// Systems
#include "ECS/Systems/ClientUpdateSystem.h"

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
	_componentRegistry = new entt::registry<u32>();
	// Temporary for now, don't worry
	for (u32 i = 0; i < 10; ++i) {
		auto entity = _componentRegistry->create();
		_componentRegistry->assign<PositionComponent>(entity, 0u, i * 1.f, i * 1.f, i * 1.f, i * 1.f);
	}

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

bool WorldServerHandler::Update(f32 deltaTime)
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

	UpdateSystems(deltaTime);
	return true;
}

void WorldServerHandler::UpdateSystems(f32 deltaTime)
{
	ClientUpdateSystem::Update(deltaTime, *_componentRegistry);
}
