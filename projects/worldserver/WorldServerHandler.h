#pragma once
#include <NovusTypes.h>
#include "Utils/ConcurrentQueue.h"
#include "Message.h"
#include <entt.hpp>

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
	WorldServerHandler(f32 targetTickRate);
	~WorldServerHandler();

	void Start();
	void Stop();

	void PassMessage(Message& message);
	bool TryGetMessage(Message& message);

private:
	void Run();
	bool Update(f32 deltaTime);
	void UpdateSystems(f32 deltaTime);

private:
	bool _isRunning;
    f32 _targetTickRate;

	ConcurrentQueue<Message> _inputQueue;
	ConcurrentQueue<Message> _outputQueue;
	entt::registry<u32>* _componentRegistry;
};