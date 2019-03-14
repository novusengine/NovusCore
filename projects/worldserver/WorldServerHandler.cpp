#include "WorldServerHandler.h"
#include <Utils/Timer.h>
#include <thread>
#include <iostream>
#include <Networking/Opcode/Opcode.h>
#include <tracy/Tracy.hpp>
#include <taskflow/taskflow.hpp>

// Systems
#include "ECS/Systems/ConnectionSystem.h"
#include "ECS/Systems/ClientUpdateSystem.h"
#include "ECS/Systems/PlayerUpdateDataSystem.h"
#include "ECS/Systems/CreatePlayerSystem.h"

#include "ECS/Components/SingletonComponent.h"

#include "Connections/NovusConnection.h"

using namespace std::chrono;
static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

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

void WorldServerHandler::SetNovusConnection(NovusConnection* connection)
{
	_novusConnection = connection;
	SingletonComponent& singletonComponent = _updateFramework.registry->get<SingletonComponent>(0);
	singletonComponent.connection = _novusConnection;
}

void WorldServerHandler::PassMessage(Message& message)
{
	_inputQueue.enqueue(message);
}

bool WorldServerHandler::TryGetMessage(Message& message)
{
	return _outputQueue.try_dequeue(message);
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
	SetupUpdateFramework();

	_updateFramework.registry->create();
	SingletonComponent& singletonComponent = _updateFramework.registry->assign<SingletonComponent>(0);
    CreatePlayerQueueComponent& createPlayerQueueComponent = _updateFramework.registry->assign<CreatePlayerQueueComponent>(0);
	singletonComponent.worldServerHandler = this;
	singletonComponent.connection = _novusConnection;
	singletonComponent.deltaTime = 1.0f;
    createPlayerQueueComponent.newEntityQueue = new ConcurrentQueue<Message>(256);

	Timer timer;
	while (true)
	{
		f32 deltaTime = timer.GetDeltaTime();
		timer.Tick();

		singletonComponent.lifeTime = timer.GetLifeTime();
		singletonComponent.deltaTime = deltaTime;
		
		if (!Update())
			break;

		{
			ZoneScopedNC("WaitForTickRate", tracy::Color::AntiqueWhite1)

			// Wait for tick rate, this might be an overkill implementation but it has the least amount of stuttering I've ever found - MPursche
			f32 targetDelta = 1.0f / _targetTickRate;
			for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
			{
				f32 remaining = targetDelta - deltaTime;
				if (remaining < 0.025f)
				{
					ZoneScopedNC("Yield", tracy::Color::AntiqueWhite1)
					std::this_thread::yield();
				}
				else
				{
					ZoneScopedNC("Sleep", tracy::Color::AntiqueWhite1)
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
		
		FrameMark
	}

	// Clean up stuff here

	Message exitMessage;
	exitMessage.code = MSG_OUT_EXIT_CONFIRM;
	_outputQueue.enqueue(exitMessage);
}

bool WorldServerHandler::Update()
{
	ZoneScopedNC("Update", tracy::Color::Blue2)
	{
		ZoneScopedNC("HandleMessages", tracy::Color::Blue2)
		Message message;
		while (_inputQueue.try_dequeue(message))
		{
			if (message.code == -1)
				assert(false);

			if (message.code == MSG_IN_EXIT)
				return false;

			if (message.code == MSG_IN_PING)
			{
				ZoneScopedNC("Ping", tracy::Color::Blue2)
				Message pongMessage;
				pongMessage.code = MSG_OUT_PRINT;
				pongMessage.message = new std::string("PONG!");
				_outputQueue.enqueue(pongMessage);
			}

			if (message.code == MSG_IN_FOWARD_PACKET)
			{
                // Forward player login to later in the frame.
				if (Common::Opcode((u16)message.opcode) == Common::Opcode::CMSG_PLAYER_LOGIN)
				{
                    ZoneScopedNC("LoginMessage", tracy::Color::Blue2)
                    _updateFramework.registry->get<CreatePlayerQueueComponent>(0).newEntityQueue->enqueue(message);
				}
				else
				{
					ZoneScopedNC("ForwardMessage", tracy::Color::Blue2)
                    SingletonComponent & singletonComponent = _updateFramework.registry->get<SingletonComponent>(0);
                    auto itr = singletonComponent.accountToEntityMap.find(u32(message.account));
                    if (itr != singletonComponent.accountToEntityMap.end())
					{
						ConnectionComponent& connection = _updateFramework.registry->get<ConnectionComponent>(itr->second);
						connection.packets.push_back({ u32(message.opcode), false, message.packet });
					}
				}
			}
		}
	}
	
	UpdateSystems();
	return true;
}

void WorldServerHandler::SetupUpdateFramework()
{
	_updateFramework.registry = new entt::registry();
	_updateFramework.framework = new tf::Framework();

	tf::Framework& framework = *_updateFramework.framework;
	entt::registry& registry = *_updateFramework.registry;

	tf::Task connectionSystemTask = framework.emplace([&registry]() {
		ConnectionSystem::Update(registry);
	});

	tf::Task playerUpdateDataSystemTask = framework.emplace([&registry]() {
		PlayerUpdateDataSystem::Update(registry);
	});
	playerUpdateDataSystemTask.gather(connectionSystemTask);

	tf::Task clientUpdateSystemTask = framework.emplace([&registry]() {
		ClientUpdateSystem::Update(registry);
	});
	clientUpdateSystemTask.gather(playerUpdateDataSystemTask);

    tf::Task createPlayerSystemTask = framework.emplace([&registry]() {
        CreatePlayerSystem::Update(registry);
    });
    createPlayerSystemTask.gather(clientUpdateSystemTask);
}

void WorldServerHandler::UpdateSystems()
{
	ZoneScopedNC("UpdateSystems", tracy::Color::Blue2)
	tf::Taskflow taskflow;
	taskflow.run(*_updateFramework.framework);
	taskflow.wait_for_all();
	
    /* No Multi threading on this call (Parallel is fine) */
    //ConnectionSystem::Update(deltaTime, *_novusConnection, *_componentRegistry);

    /* Run Gameplay Systems */

    
    /* Run PlayerUpdateDataSystem to fill out PlayerUpdateData */
    //PlayerUpdateDataSystem::Update(deltaTime, *this, *_novusConnection, *_componentRegistry);
    /* Run ClientUpdateSystem to send built PlayerUpdateData to players who need it */
    //ClientUpdateSystem::Update(deltaTime, *_componentRegistry);
}
