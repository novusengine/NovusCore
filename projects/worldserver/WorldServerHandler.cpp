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

	u32 entity = _updateFramework.registry->create();
	SingletonComponent& singletonComponent = _updateFramework.registry->assign<SingletonComponent>(0);
	singletonComponent.worldServerHandler = this;
	singletonComponent.connection = _novusConnection;
	singletonComponent.deltaTime = 1.0f;

	Timer timer;
	while (true)
	{
		f32 deltaTime = timer.GetDeltaTime();
		timer.Tick();
		
		if (!Update(deltaTime))
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

bool WorldServerHandler::Update(f32 deltaTime)
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
				
				// Create Entity if it doesn't exist, otherwise add 
				if (Common::Opcode((u16)message.opcode) == Common::Opcode::CMSG_PLAYER_LOGIN)
				{
					ZoneScopedNC("LoginMessage", tracy::Color::Blue2)
					u64 playerGuid = 0;
					message.packet.Read<u64>(playerGuid);

					
					u32 entity = _updateFramework.registry->create();
					ConnectionComponent& connection = _updateFramework.registry->assign<ConnectionComponent>(entity, u32(message.account), playerGuid, false);
					connection.packets.push_back({ u32(message.opcode), false, message.packet });

					_updateFramework.registry->assign<PlayerUpdateDataComponent>(entity);
					_updateFramework.registry->assign<PositionComponent>(entity, 0u, -8949.950195f, -132.492996f, 83.531197f, 0.f);

					_accountToEntityMap[message.account] = entity;
				}
				else
				{
					ZoneScopedNC("ForwardMessage", tracy::Color::Blue2)
					auto itr = _accountToEntityMap.find(message.account);
					if (itr != _accountToEntityMap.end())
					{
						ConnectionComponent& connection = _updateFramework.registry->get<ConnectionComponent>(itr->second);
						connection.packets.push_back({ u32(message.opcode), false, message.packet });
					}
				}
			}
		}
	}
	
	UpdateSystems(deltaTime);
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
}

void WorldServerHandler::UpdateSystems(f32 deltaTime)
{
	_updateFramework.registry->get<SingletonComponent>(0).deltaTime = deltaTime;

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
