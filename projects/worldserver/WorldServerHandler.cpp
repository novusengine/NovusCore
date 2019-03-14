#include "WorldServerHandler.h"
#include <Utils/Timer.h>
#include <thread>
#include <iostream>
#include <Networking/Opcode/Opcode.h>

// Systems
#include "ECS/Systems/ConnectionSystem.h"
#include "ECS/Systems/ClientUpdateSystem.h"
#include "ECS/Systems/PlayerUpdateDataSystem.h"

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
	_componentRegistry = new entt::registry();

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
			if (remaining < 0.002f)
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
	_outputQueue.enqueue(exitMessage);
}

bool WorldServerHandler::Update(f32 deltaTime)
{
    /* Uncomment to test if Update is being called
	Message updateMessage;
	updateMessage.code = MSG_OUT_PRINT;
	updateMessage.message = new std::string("Updated!");
	_outputQueue.push(updateMessage);*/

	Message message;
	while (_inputQueue.try_dequeue(message))
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
			_outputQueue.enqueue(pongMessage);
		}

        if (message.code == MSG_IN_FOWARD_PACKET)
        {
            // Create Entity if it doesn't exist, otherwise add 
            if (Common::Opcode((u16)message.opcode) == Common::Opcode::CMSG_PLAYER_LOGIN)
            {
                u64 playerGuid = 0;
                message.packet.Read<u64>(playerGuid);

                u32 entity = _componentRegistry->create();
                _componentRegistry->assign<ConnectionComponent>(entity, u32(message.account), playerGuid, false);
                _componentRegistry->assign<PlayerUpdateDataComponent>(entity);
                _componentRegistry->assign<PositionComponent>(entity, 0u, -8949.950195f, -132.492996f, 83.531197f, 0.f);

                ConnectionComponent& connection = _componentRegistry->get<ConnectionComponent>(entity);
                connection.packets.push_back({ u32(message.opcode), false, message.packet });

                _accountToEntityMap[message.account] = entity;
            }
            else
            {
                auto itr = _accountToEntityMap.find(message.account);
                if (itr != _accountToEntityMap.end())
                {
                    ConnectionComponent& connection = _componentRegistry->get<ConnectionComponent>(itr->second);
                    connection.packets.push_back({ u32(message.opcode), false, message.packet });
                }
            }
        }
	}

	UpdateSystems(deltaTime);
	return true;
}

void WorldServerHandler::UpdateSystems(f32 deltaTime)
{
    /* No Multi threading on this call (Parallel is fine) */
    ConnectionSystem::Update(deltaTime, *_novusConnection, *_componentRegistry);

    /* Run Gameplay Systems */

    
    /* Run PlayerUpdateDataSystem to fill out PlayerUpdateData */
    PlayerUpdateDataSystem::Update(deltaTime, *this, *_novusConnection, *_componentRegistry);
    /* Run ClientUpdateSystem to send built PlayerUpdateData to players who need it */
    ClientUpdateSystem::Update(deltaTime, *_componentRegistry);
}
