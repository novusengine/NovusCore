#include "WorldServerHandler.h"
#include <Utils/Timer.h>
#include <thread>
#include <iostream>


// Systems
#include "ECS/Systems/ClientUpdateSystem.h"

WorldServerHandler::WorldServerHandler(f32 targetTickRate)
	: _isRunning(false)
	, _inputQueue(256)
	, _outputQueue(256)
{
    _targetTickRate = targetTickRate;
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

	Timer timer;
	while (true)
	{
		f32 deltaTime = timer.GetDeltaTime();
		timer.Tick();
		
		if (!Update(deltaTime))
			break;

		// Wait for tick rate, this might be an overkill implementation but it has the least amount of stuttering I've ever found - MPursche
		f32 targetDelta = 1.0f / _targetTickRate;
		for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
		{
			f32 remaining = targetDelta - deltaTime;
			if (remaining < 2.0f)
			{
				std::this_thread::yield();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	// Clean up stuff here

	Message exitMessage;
	exitMessage.code = MSG_OUT_EXIT_CONFIRM;
	_outputQueue.push(exitMessage);
}

bool WorldServerHandler::Update(f32 deltaTime)
{
    /* Uncomment to test if Update is being called
	Message updateMessage;
	updateMessage.code = MSG_OUT_PRINT;
	updateMessage.message = new std::string("Updated!");
	_outputQueue.push(updateMessage);*/

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
