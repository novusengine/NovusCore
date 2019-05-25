#include "WorldNodeHandler.h"
#include <thread>
#include <iostream>

#include <Utils/Timer.h>
#include <Networking/Opcode/Opcode.h>
#include <tracy/Tracy.hpp>
#include <Config/ConfigHandler.h>

// Systems
#include "ECS/Systems/PlayerConnectionSystem.h"
#include "ECS/Systems/PlayerInitializeSystem.h"
#include "ECS/Systems/ItemInitializeSystem.h"
//#include "ECS/Systems/CommandParserSystem.h"
#include "ECS/Systems/PlayerCreateDataSystem.h"
#include "ECS/Systems/ItemCreateDataSystem.h"
#include "ECS/Systems/PlayerUpdateDataSystem.h"
#include "ECS/Systems/ClientUpdateSystem.h"
#include "ECS/Systems/PlayerCreateSystem.h"
#include "ECS/Systems/EntityCreateSystem.h"
#include "ECS/Systems/PlayerDeleteSystem.h"

#include "ECS/Components/Singletons/SingletonComponent.h"
#include "ECS/Components/Singletons/CommandDataSingleton.h"
#include "ECS/Components/Singletons/GuidLookupSingleton.h"
#include "ECS/Components/Singletons/PlayerCreateQueueSingleton.h"
#include "ECS/Components/Singletons/PlayerUpdatesQueueSingleton.h"
#include "ECS/Components/Singletons/PlayerDeleteQueueSingleton.h"
#include "ECS/Components/Singletons/PlayerPacketQueueSingleton.h"
#include "ECS/Components/Singletons/EntityCreateQueueSingleton.h"
#include "ECS/Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "ECS/Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"

// Game
#include "Game/Commands/Commands.h"
#include "Game/ObjectGuid/ObjectGuid.h"

// Scripting
#include "Scripting/ScriptHandler.h"

WorldNodeHandler::WorldNodeHandler(f32 targetTickRate)
    : _isRunning(false)
    , _inputQueue(256)
    , _outputQueue(256)
{
    _targetTickRate = targetTickRate;
}

WorldNodeHandler::~WorldNodeHandler()
{
}

void WorldNodeHandler::PassMessage(Message& message)
{
    _inputQueue.enqueue(message);
}

bool WorldNodeHandler::TryGetMessage(Message& message)
{
    return _outputQueue.try_dequeue(message);
}

void WorldNodeHandler::Start()
{
    if (_isRunning)
        return;

    std::thread thread = std::thread(&WorldNodeHandler::Run, this);
    thread.detach();
}

void WorldNodeHandler::Stop()
{
    if (!_isRunning)
        return;

    Message message;
    message.code = MSG_IN_EXIT;
    PassMessage(message);
}

void WorldNodeHandler::Run()
{
    std::string scriptDirectory = ConfigHandler::GetOption<std::string>("path", "scripts");
    ScriptHandler::LoadScriptDirectory(scriptDirectory);

    SetupUpdateFramework();
    _updateFramework.registry.create();

    CharacterDatabaseCacheSingleton& characterDatabaseCacheSingleton = _updateFramework.registry.set<CharacterDatabaseCacheSingleton>();
    characterDatabaseCacheSingleton.cache = new CharacterDatabaseCache();
    characterDatabaseCacheSingleton.cache->Load();

    WorldDatabaseCacheSingleton& worldDatabaseCacheSingleton = _updateFramework.registry.set<WorldDatabaseCacheSingleton>();
    worldDatabaseCacheSingleton.cache = new WorldDatabaseCache();
    worldDatabaseCacheSingleton.cache->Load();

	DBCDatabaseCacheSingleton& dbcDatabaseCacheSingleton = _updateFramework.registry.set<DBCDatabaseCacheSingleton>();
	dbcDatabaseCacheSingleton.cache = new DBCDatabaseCache();
	dbcDatabaseCacheSingleton.cache->Load();

    if (!_mapLoader.Load(_updateFramework.registry))
    {
        /*Message exitMessage;
        exitMessage.code = MSG_OUT_EXIT_CONFIRM;
        _outputQueue.enqueue(exitMessage);
        return;*/
    }

    SingletonComponent& singletonComponent = _updateFramework.registry.set<SingletonComponent>();
    singletonComponent.worldNodeHandler = this;
    singletonComponent.deltaTime = 1.0f;

    /*GuidLookupSingleton& guidLookupSingleton = */_updateFramework.registry.set<GuidLookupSingleton>();
    PlayerCreateQueueSingleton& playerCreateQueueComponent = _updateFramework.registry.set<PlayerCreateQueueSingleton>();
    playerCreateQueueComponent.newPlayerQueue = new moodycamel::ConcurrentQueue<Message>(256);

    /*PlayerUpdatesQueueSingleton& playerUpdatesQueueSingleton =*/ _updateFramework.registry.set<PlayerUpdatesQueueSingleton>();
    PlayerDeleteQueueSingleton& playerDeleteQueueSingleton = _updateFramework.registry.set<PlayerDeleteQueueSingleton>();
    playerDeleteQueueSingleton.expiredEntityQueue = new moodycamel::ConcurrentQueue<ExpiredPlayerData>(256);

    PlayerPacketQueueSingleton& playerPacketQueueSingleton = _updateFramework.registry.set<PlayerPacketQueueSingleton>();
    playerPacketQueueSingleton.packetQueue = new moodycamel::ConcurrentQueue<PacketQueueData>(256);

    EntityCreateQueueSingleton& entityCreateQueueSingleton = _updateFramework.registry.set<EntityCreateQueueSingleton>();
    entityCreateQueueSingleton.newEntityQueue = new moodycamel::ConcurrentQueue<EntityCreationRequest>(4096);

    //Commands::LoadCommands(_updateFramework.registry);

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

bool WorldNodeHandler::Update()
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

            if (message.code == MSG_IN_RELOAD_SCRIPTS)
            {
                ScriptHandler::ReloadScripts();
            }

            if (message.code == MSG_IN_PLAYER_DISCONNECT)
            {
                SingletonComponent& singletonComponent = _updateFramework.registry.ctx<SingletonComponent>();

                auto itr = singletonComponent.accountToEntityMap.find(static_cast<u32>(message.account));
                if (itr != singletonComponent.accountToEntityMap.end())
                {
                    CharacterDatabaseCacheSingleton& characterDatabase = _updateFramework.registry.ctx<CharacterDatabaseCacheSingleton>();
                    PlayerDeleteQueueSingleton& playerDeleteQueue = _updateFramework.registry.ctx<PlayerDeleteQueueSingleton>();
                    PlayerConnectionComponent& playerConnection = _updateFramework.registry.get<PlayerConnectionComponent>(itr->second);
                    PlayerPositionComponent& playerPositionData = _updateFramework.registry.get<PlayerPositionComponent>(itr->second);
                    
                    ExpiredPlayerData expiredPlayerData;
                    expiredPlayerData.entityGuid = playerConnection.entityGuid;
                    expiredPlayerData.accountGuid = playerConnection.accountGuid;
                    expiredPlayerData.characterGuid = playerConnection.characterGuid;
                    playerDeleteQueue.expiredEntityQueue->enqueue(expiredPlayerData);

                    CharacterInfo characterInfo;
                    characterDatabase.cache->GetCharacterInfo(playerConnection.characterGuid, characterInfo);

                    characterInfo.mapId = playerPositionData.mapId;
                    characterInfo.coordinateX = playerPositionData.x;
                    characterInfo.coordinateY = playerPositionData.y;
                    characterInfo.coordinateZ = playerPositionData.z;
                    characterInfo.orientation = playerPositionData.orientation;
                    characterInfo.online = 0;
                    characterInfo.UpdateCache(playerConnection.characterGuid);

                    characterDatabase.cache->SaveAndUnloadCharacter(playerConnection.characterGuid);
                }
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
                        connection.packets.push_back({ static_cast<u16>(message.opcode), false, message.packet });
                    }
                }
            }
        }
    }

    UpdateSystems();
    return true;
}

void WorldNodeHandler::SetupUpdateFramework()
{
    tf::Framework& framework = _updateFramework.framework;
    entt::registry& registry = _updateFramework.registry;

    // ConnectionSystem
    tf::Task connectionSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("ConnectionSystem", tracy::Color::Orange2)
        ConnectionSystem::Update(registry);
    });

    /* CommandParserSystem
    tf::Task commandParserSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("CommandParserSystem", tracy::Color::Blue2)
        CommandParserSystem::Update(registry);
    });
    commandParserSystemTask.gather(connectionSystemTask);*/

    // PlayerInitializeSystem
    tf::Task playerInitializeSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("PlayerInitializeSystem", tracy::Color::Blue2)
            PlayerInitializeSystem::Update(registry);
    });
    playerInitializeSystemTask.gather(connectionSystemTask);

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
            EntityCreateSystem::Update(registry);
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

void WorldNodeHandler::UpdateSystems()
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
