#include "WorldServerHandler.h"
#include <Utils/Timer.h>
#include <thread>
#include <iostream>
#include <Networking/Opcode/Opcode.h>

// Systems
#include "ECS/Systems/ConnectionSystem.h"
#include "ECS/Systems/ClientUpdateSystem.h"

#include "Connections/NovusConnection.h"

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
            switch (Common::Opcode((u16)message.opcode))
            {
                case Common::Opcode::CMSG_SET_ACTIVE_MOVER:
                {
                    NovusHeader packetHeader;
                    packetHeader.command = NOVUS_FOWARDPACKET;
                    packetHeader.account = message.account;
                    packetHeader.opcode = Common::Opcode::SMSG_TIME_SYNC_REQ;
                    packetHeader.size = 4;

                    Common::ByteBuffer timeSync(9 + 4);
                    packetHeader.AddTo(timeSync);

                    timeSync.Write<u32>(0);
                    _novusConnection->SendPacket(timeSync);
                    break;
                }
                case Common::Opcode::CMSG_PLAYER_LOGIN:
                {
                    u64 playerGuid = 0;
                    message.packet.Read<u64>(playerGuid);

                    auto entity = _componentRegistry->create();
                    _componentRegistry->assign<ConnectionComponent>(entity, (u32)message.account, playerGuid, false);
                    _componentRegistry->assign<PlayerConnectionData>(entity);
                    _componentRegistry->assign<PositionComponent>(entity, 0u, 1.f, 1.f, 1.f, 1.f);

                    break;
                }
                case Common::Opcode::CMSG_LOGOUT_REQUEST:
                {

                    Message defaultOpcodeMessage;
                    defaultOpcodeMessage.code = MSG_OUT_PRINT;
                    defaultOpcodeMessage.message = new std::string("Received LOGOUT REQUEST Opcode\n");
                    _outputQueue.enqueue(defaultOpcodeMessage);

                    NovusHeader packetHeader;
                    packetHeader.command = NOVUS_FOWARDPACKET;
                    packetHeader.account = message.account;
                    packetHeader.opcode = Common::Opcode::SMSG_LOGOUT_COMPLETE;
                    packetHeader.size = 0;

                    Common::ByteBuffer logoutRequest(0);
                    packetHeader.AddTo(logoutRequest);

                    _novusConnection->SendPacket(logoutRequest);
                    
                    break;
                }

                default:
                {
                    Message defaultOpcodeMessage;
                    defaultOpcodeMessage.code = MSG_OUT_PRINT;
                    defaultOpcodeMessage.message = new std::string("Received Unhandled Opcode");
                    _outputQueue.enqueue(defaultOpcodeMessage);
                    break;
                }

            }
        }
	}

	UpdateSystems(deltaTime);
	return true;
}

void WorldServerHandler::UpdateSystems(f32 deltaTime)
{
	ClientUpdateSystem::Update(deltaTime, *_componentRegistry);
    
    // Call last, Don't multithread this
    ConnectionSystem::Update(deltaTime, *_novusConnection, *_componentRegistry);
}
