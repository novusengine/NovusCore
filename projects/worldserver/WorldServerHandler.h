#pragma once
#include <NovusTypes.h>
#include "Utils/ConcurrentQueue.h"
#include "Message.h"
#include <entt.hpp>
#include <unordered_map>

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

namespace tf
{
	class Framework;
}

struct FrameworkRegistryPair
{
	entt::registry* registry;
	tf::Framework* framework;
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

	void SetNovusConnection(NovusConnection* novusConnection);

    template <typename... Args>
    void PrintMessage(std::string message, Args... args)
    {
        char str[256];
        int length = sprintf_s(str, message.c_str(), args...);
        assert(length > -1);

        Message printMessage;
        printMessage.code = MSG_OUT_PRINT;
        printMessage.message = new std::string(str);
        _outputQueue.enqueue(printMessage);
    }
private:
	void Run();
	bool Update();
	void UpdateSystems();

	void SetupUpdateFramework();

private:
	bool _isRunning;
    f32 _targetTickRate;

    NovusConnection* _novusConnection;
    std::unordered_map<u64, u32> _accountToEntityMap;
	ConcurrentQueue<Message> _inputQueue;
	ConcurrentQueue<Message> _outputQueue;
	FrameworkRegistryPair _updateFramework;
};