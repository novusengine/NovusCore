#include "WorldServerHandler.h"
#include <Utils/Timer.h>
#include <thread>
#include <iostream>
#include <Networking/Opcode/Opcode.h>
#include <tracy/Tracy.hpp>

// Systems
#include "ECS/Systems/PlayerConnectionSystem.h"
#include "ECS/Systems/PlayerInitializeSystem.h"
#include "ECS/Systems/ItemInitializeSystem.h"
#include "ECS/Systems/CommandParserSystem.h"
#include "ECS/Systems/PlayerCreateDataSystem.h"
#include "ECS/Systems/ItemCreateDataSystem.h"
#include "ECS/Systems/PlayerUpdateDataSystem.h"
#include "ECS/Systems/ClientUpdateSystem.h"
#include "ECS/Systems/PlayerCreateSystem.h"
#include "ECS/Systems/ItemCreateSystem.h"
#include "ECS/Systems/PlayerDeleteSystem.h"

#include "ECS/Components/Singletons/SingletonComponent.h"
#include "ECS/Components/Singletons/PlayerCreateQueueSingleton.h"
#include "ECS/Components/Singletons/PlayerUpdatesQueueSingleton.h"
#include "ECS/Components/Singletons/PlayerDeleteQueueSingleton.h"
#include "ECS/Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "ECS/Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "ECS/Components/Singletons/CommandDataSingleton.h"
#include "ECS/Components/Singletons/PlayerPacketQueueSingleton.h"
#include "ECS/Components/Singletons/ItemCreateQueueSingleton.h"
#include "ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"
#include "Connections/NovusConnection.h"

// Game
#include "Game/Commands/Commands.h"
#include "Game/ObjectGuid/ObjectGuid.h"

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
    if (!_mapLoader.Load(_updateFramework.registry))
    {
        /*Message exitMessage;
        exitMessage.code = MSG_OUT_EXIT_CONFIRM;
        _outputQueue.enqueue(exitMessage);
        return;*/
    }

    SetupUpdateFramework();

    _updateFramework.registry.create();
    SingletonComponent& singletonComponent = _updateFramework.registry.set<SingletonComponent>();
    PlayerCreateQueueSingleton& playerCreateQueueComponent = _updateFramework.registry.set<PlayerCreateQueueSingleton>();
    PlayerUpdatesQueueSingleton& playerUpdatesQueueSingleton = _updateFramework.registry.set<PlayerUpdatesQueueSingleton>();
    PlayerDeleteQueueSingleton& playerDeleteQueueSingleton = _updateFramework.registry.set<PlayerDeleteQueueSingleton>();
    PlayerPacketQueueSingleton& playerPacketQueueSingleton = _updateFramework.registry.set<PlayerPacketQueueSingleton>();
    ItemCreateQueueSingleton& itemCreateQueueComponent = _updateFramework.registry.set<ItemCreateQueueSingleton>();

	WorldDatabaseCacheSingleton& worldDatabaseCacheSingleton = _updateFramework.registry.set<WorldDatabaseCacheSingleton>();
	CharacterDatabaseCacheSingleton& characterDatabaseCacheSingleton = _updateFramework.registry.set<CharacterDatabaseCacheSingleton>();
	DBCDatabaseCacheSingleton& dbcDatabaseCacheSingleton = _updateFramework.registry.set<DBCDatabaseCacheSingleton>();
   
    singletonComponent.worldServerHandler = this;
    singletonComponent.connection = _novusConnection;
    singletonComponent.deltaTime = 1.0f;

    playerCreateQueueComponent.newPlayerQueue = new moodycamel::ConcurrentQueue<Message>(256);
    playerDeleteQueueSingleton.expiredEntityQueue = new moodycamel::ConcurrentQueue<ExpiredPlayerData>(256);
    playerPacketQueueSingleton.packetQueue = new moodycamel::ConcurrentQueue<Common::ByteBuffer>(256);

    itemCreateQueueComponent.newItemQueue = new moodycamel::ConcurrentQueue<ItemCreationInformation>(256);

    characterDatabaseCacheSingleton.cache = new CharacterDatabaseCache();
    characterDatabaseCacheSingleton.cache->Load();

    worldDatabaseCacheSingleton.cache = new WorldDatabaseCache();
    worldDatabaseCacheSingleton.cache->Load();

	dbcDatabaseCacheSingleton.cache = new DBCDatabaseCache();
	dbcDatabaseCacheSingleton.cache->Load();

    Commands::LoadCommands(_updateFramework.registry);

    Timer timer;
    while (true)
    {
        f32 deltaTime = timer.GetDeltaTime();
        timer.Tick();

        singletonComponent.lifeTimeInS = timer.GetLifeTime();
        singletonComponent.lifeTimeInMS = singletonComponent.lifeTimeInS * 1000;
        singletonComponent.deltaTime = deltaTime;

        if (!Update())
            break;

        {
            ZoneScopedNC("WaitForTickRate", tracy::Color::AntiqueWhite1)

            // Wait for tick rate, this might be an overkill implementation but it has the even tickrate I've seen - MPursche
            f32 targetDelta = 1.0f / _targetTickRate;
            {
                ZoneScopedNC("Sleep", tracy::Color::AntiqueWhite1)
                for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta - 0.0025f; deltaTime = timer.GetDeltaTime())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            {
                ZoneScopedNC("Yield", tracy::Color::AntiqueWhite1)
                for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
                {
                    std::this_thread::yield();
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
        ZoneScopedNC("HandleMessages", tracy::Color::Green3)
        Message message;

        while (_inputQueue.try_dequeue(message))
        {
            if (message.code == -1)
                assert(false);

            if (message.code == MSG_IN_EXIT)
                return false;

            if (message.code == MSG_IN_PING)
            {
                ZoneScopedNC("Ping", tracy::Color::Green3)
                Message pongMessage;
                pongMessage.code = MSG_OUT_PRINT;
                pongMessage.message = new std::string("PONG!");
                _outputQueue.enqueue(pongMessage);
            }

            if (message.code == MSG_IN_SET_CONNECTION)
            {
                SingletonComponent& singletonComponent = _updateFramework.registry.ctx<SingletonComponent>();
                singletonComponent.connection = _novusConnection;
            }

            if (message.code == MSG_IN_FOWARD_PACKET)
            {
                // Create Entity if it doesn't exist, otherwise add 
                if (static_cast<Common::Opcode>(static_cast<u16>(message.opcode)) == Common::Opcode::CMSG_PLAYER_LOGIN)
                {
                    ZoneScopedNC("LoginMessage", tracy::Color::Green3)
                    _updateFramework.registry.ctx<PlayerCreateQueueSingleton>().newPlayerQueue->enqueue(message);
                }
                else
                {
                    ZoneScopedNC("ForwardMessage", tracy::Color::Green3)
                    SingletonComponent& singletonComponent = _updateFramework.registry.ctx<SingletonComponent>();

                    auto itr = singletonComponent.accountToEntityMap.find(static_cast<u32>(message.account));
                    if (itr != singletonComponent.accountToEntityMap.end())
                    {
                        PlayerConnectionComponent& connection = _updateFramework.registry.get<PlayerConnectionComponent>(itr->second);
                        connection.packets.push_back({ static_cast<u32>(message.opcode), false, message.packet });
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
    tf::Framework& framework = _updateFramework.framework;
    entt::registry& registry = _updateFramework.registry;
    
    // ConnectionSystem
    tf::Task connectionSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("ConnectionSystem", tracy::Color::Orange2)
        ConnectionSystem::Update(registry);
    });

    // CommandParserSystem
    tf::Task commandParserSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("CommandParserSystem", tracy::Color::Blue2)
        CommandParserSystem::Update(registry);
    });
    commandParserSystemTask.gather(connectionSystemTask);

    // PlayerInitializeSystem
    tf::Task playerInitializeSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("PlayerInitializeSystem", tracy::Color::Blue2)
            PlayerInitializeSystem::Update(registry);
    });
    playerInitializeSystemTask.gather(commandParserSystemTask);

    // ItemInitializeSystem
    tf::Task itemInitializeSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("ItemInitializeSystem", tracy::Color::Blue2)
            ItemInitializeSystem::Update(registry);
    });
    itemInitializeSystemTask.gather(playerInitializeSystemTask);

    // PlayerCreateDataSystem
    tf::Task playerCreateDataSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("PlayerCreateDataSystem", tracy::Color::Blue2)
        PlayerCreateDataSystem::Update(registry);
    });
    playerCreateDataSystemTask.gather(itemInitializeSystemTask);

    // ItemCreateDataSystem
    tf::Task itemCreateDataSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("ItemCreateDataSystem", tracy::Color::Blue2)
            ItemCreateDataSystem::Update(registry);
    });
    itemCreateDataSystemTask.gather(playerCreateDataSystemTask);

    // PlayerUpdateDataSystem
    tf::Task playerUpdateDataSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("PlayerUpdateDataSystem", tracy::Color::Yellow2)
        PlayerUpdateDataSystem::Update(registry);
    });
    playerUpdateDataSystemTask.gather(itemCreateDataSystemTask);

    // ClientUpdateSystem
    tf::Task clientUpdateSystemTask = framework.emplace([&registry]() 
    {
        ZoneScopedNC("clientUpdateSystemTask", tracy::Color::Blue2)
        ClientUpdateSystem::Update(registry);
    });
    clientUpdateSystemTask.gather(playerUpdateDataSystemTask);

    // PlayerCreateSystem
    tf::Task playerCreateSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("PlayerCreateSystem", tracy::Color::Blue2)
            PlayerCreateSystem::Update(registry);
    });
    playerCreateSystemTask.gather(clientUpdateSystemTask);

    // ItemCreateSystem
    tf::Task itemCreateSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("ItemCreateSystem", tracy::Color::Blue2)
            ItemCreateSystem::Update(registry);
    });
    itemCreateSystemTask.gather(playerCreateSystemTask);

    // PlayerDeleteSystem
    tf::Task playerDeleteSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("PlayerDeleteSystem", tracy::Color::Blue2)
        PlayerDeleteSystem::Update(registry);
    });
    playerDeleteSystemTask.gather(itemCreateSystemTask);
}

void WorldServerHandler::UpdateSystems()
{
    ZoneScopedNC("UpdateSystems", tracy::Color::Blue2)
    {
        ZoneScopedNC("Taskflow::Run", tracy::Color::Blue2)
        _updateFramework.taskflow.run(_updateFramework.framework);
    }
    {
        ZoneScopedNC("Taskflow::WaitForAll", tracy::Color::Blue2)
        _updateFramework.taskflow.wait_for_all();
    }
}