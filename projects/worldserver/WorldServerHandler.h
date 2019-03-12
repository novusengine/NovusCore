#pragma once
#include <NovusTypes.h>
#include "Utils/ConcurrentQueue.h"
#include "Message.h"
#include <entt.hpp>

enum InputMessages
{
	MSG_IN_EXIT,
	MSG_IN_PING,
    MSG_IN_FOWARD_PACKET
};

enum OutputMessages
{
	MSG_OUT_EXIT_CONFIRM,
	MSG_OUT_PRINT
};

class NovusConnection;
class WorldServerHandler
{
public:
	WorldServerHandler(f32 targetTickRate);
	~WorldServerHandler();

	void Start();
	void Stop();

	void PassMessage(Message& message);
	bool TryGetMessage(Message& message);

    void SetNovusConnection(NovusConnection* novusConnection) { _novusConnection = novusConnection; }
private:
	void Run();
	bool Update(f32 deltaTime);
	void UpdateSystems(f32 deltaTime);

private:
	bool _isRunning;
    f32 _targetTickRate;

    NovusConnection* _novusConnection;
	ConcurrentQueue<Message> _inputQueue;
	ConcurrentQueue<Message> _outputQueue;
	entt::registry<u32>* _componentRegistry;
};