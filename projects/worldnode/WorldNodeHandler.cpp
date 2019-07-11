#include "WorldNodeHandler.h"
#include <thread>
#include <iostream>

#include <Utils/Timer.h>
#include <Networking/Opcode/Opcode.h>
#include <tracy/Tracy.hpp>
#include <Config/ConfigHandler.h>

// Systems
#include "ECS/Systems/NetworkPacketSystem.h"
#include "ECS/Systems/PlayerInitializeSystem.h"
#include "ECS/Systems/EntityInitializeSystem.h"
//#include "ECS/Systems/CommandParserSystem.h"
#include "ECS/Systems/PlayerBuildDataSystem.h"
#include "ECS/Systems/EntityCreateDataSystem.h"
#include "ECS/Systems/PlayerUpdateSystem.h"
#include "ECS/Systems/PlayerAddSystem.h"
#include "ECS/Systems/EntityAddSystem.h"
#include "ECS/Systems/PlayerDeleteSystem.h"
#include "ECS/Systems/ScriptTransactionSystem.h"

#include "ECS/Components/Singletons/SingletonComponent.h"
#include "ECS/Components/Singletons/CommandDataSingleton.h"
#include "ECS/Components/Singletons/GuidLookupSingleton.h"
#include "ECS/Components/Singletons/PlayerCreateQueueSingleton.h"
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
    : _isRunning(false), _inputQueue(256), _outputQueue(256)
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
    SetupUpdateFramework();
    _updateFramework.registry.create();

    json networkJson = ConfigHandler::GetJsonObjectByKey("network");
    std::string realmserverIp = networkJson["realmserver"]["ip"];
    realmserverPort = networkJson["realmserver"]["port"];
    realmserverAddress = inet_addr(realmserverIp.c_str());
    realmserverPort += 1;

    std::string scriptDirectory = ConfigHandler::GetOption<std::string>("path", "scripts");
    ScriptHandler::SetRegistry(&_updateFramework.registry);
    ScriptHandler::LoadScriptDirectory(scriptDirectory);

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

    /*GuidLookupSingleton& guidLookupSingleton = */ _updateFramework.registry.set<GuidLookupSingleton>();
    PlayerCreateQueueSingleton& playerCreateQueueComponent = _updateFramework.registry.set<PlayerCreateQueueSingleton>();
    playerCreateQueueComponent.newPlayerQueue = new moodycamel::ConcurrentQueue<Message>(256);

    PlayerDeleteQueueSingleton& playerDeleteQueueSingleton = _updateFramework.registry.set<PlayerDeleteQueueSingleton>();
    playerDeleteQueueSingleton.expiredEntityQueue = new moodycamel::ConcurrentQueue<ExpiredPlayerData>(256);

    PlayerPacketQueueSingleton& playerPacketQueueSingleton = _updateFramework.registry.set<PlayerPacketQueueSingleton>();
    playerPacketQueueSingleton.packetQueue = new moodycamel::ConcurrentQueue<PacketQueueData>(256);

    EntityCreateQueueSingleton& entityCreateQueueSingleton = _updateFramework.registry.set<EntityCreateQueueSingleton>();
    entityCreateQueueSingleton.newEntityQueue = new moodycamel::ConcurrentQueue<EntityCreationRequest>(4096);

    _updateFramework.registry.set<ScriptSingleton>();

    //Commands::LoadCommands(_updateFramework.registry);

    Message setupCompleteMessage;
    setupCompleteMessage.code = MSG_OUT_SETUP_COMPLETE;

    _outputQueue.enqueue(setupCompleteMessage);

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
                ZoneScopedNC("Sleep", tracy::Color::AntiqueWhite1) for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta - 0.0025f; deltaTime = timer.GetDeltaTime())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            {
                ZoneScopedNC("Yield", tracy::Color::AntiqueWhite1) for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
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
            {
                return false;
            }
            else if (message.code == MSG_IN_PING)
            {
                ZoneScopedNC("Ping", tracy::Color::Green3)
                    Message pongMessage;
                pongMessage.code = MSG_OUT_PRINT;
                pongMessage.message = new std::string("PONG!");
                _outputQueue.enqueue(pongMessage);
            }
            else if (message.code == MSG_IN_RELOAD_SCRIPTS)
            {
                ScriptHandler::ReloadScripts();
            }
            else if (message.code == MSG_IN_PLAYER_DISCONNECT)
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
                    expiredPlayerData.entityId = playerConnection.entityId;
                    expiredPlayerData.accountGuid = playerConnection.accountGuid;
                    expiredPlayerData.characterGuid = playerConnection.characterGuid;
                    playerDeleteQueue.expiredEntityQueue->enqueue(expiredPlayerData);

                    CharacterInfo characterInfo;
                    if (!characterDatabase.cache->GetCharacterInfo(playerConnection.characterGuid, characterInfo))
                        continue;

                    characterInfo.mapId = playerPositionData.mapId;
                    characterInfo.position = playerPositionData.movementData.position;
                    characterInfo.orientation = playerPositionData.movementData.orientation;
                    characterInfo.online = 0;
                    characterInfo.UpdateCache(playerConnection.characterGuid);

                    characterDatabase.cache->SaveAndUnloadCharacter(playerConnection.characterGuid);
                }
            }
            else if (message.code == MSG_IN_PLAYER_CONNECTED)
            {
                ZoneScopedNC("LoginMessage", tracy::Color::Green3)
                    _updateFramework.registry.ctx<PlayerCreateQueueSingleton>()
                        .newPlayerQueue->enqueue(message);
            }
            else if (message.code == MSG_IN_NET_PACKET)
            {
                ZoneScopedNC("ForwardMessage", tracy::Color::Green3)
                    SingletonComponent& singletonComponent = _updateFramework.registry.ctx<SingletonComponent>();

                auto itr = singletonComponent.accountToEntityMap.find(static_cast<u32>(message.account));
                if (itr != singletonComponent.accountToEntityMap.end())
                {
                    PlayerConnectionComponent& connection = _updateFramework.registry.get<PlayerConnectionComponent>(itr->second);
                    connection.packets.push_back({static_cast<u16>(message.opcode), false, message.packet});
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
    tf::Task networkPacketSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("networkPacketSystem", tracy::Color::Orange2)
            NetworkPacketSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });

    /* CommandParserSystem
    tf::Task commandParserSystemTask = framework.emplace([&registry]()
    {
        ZoneScopedNC("CommandParserSystem", tracy::Color::Blue2)
        CommandParserSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    commandParserSystemTask.gather(connectionSystemTask);*/

    // PlayerInitializeSystem
    tf::Task playerInitializeSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("PlayerInitializeSystem", tracy::Color::Blue2)
            PlayerInitializeSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    playerInitializeSystemTask.gather(networkPacketSystemTask);

    // EntityInitializeSystem
    tf::Task entityInitializeSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("EntityInitializeSystem", tracy::Color::Blue2)
            EntityInitializeSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    entityInitializeSystemTask.gather(playerInitializeSystemTask);

    // PlayerBuildDataSystem
    tf::Task playerBuildDataSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("PlayerBuildDataSystem", tracy::Color::Blue2)
            PlayerBuildDataSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    playerBuildDataSystemTask.gather(entityInitializeSystemTask);

    // EntityCreateDataSystem
    tf::Task entityCreateDataSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("EntityCreateDataSystem", tracy::Color::Blue2)
            EntityCreateDataSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    entityCreateDataSystemTask.gather(playerBuildDataSystemTask);

    // PlayerUpdateSystem
    tf::Task playerUpdateDataSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("PlayerUpdateSystem", tracy::Color::Yellow2)
            PlayerUpdateSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    playerUpdateDataSystemTask.gather(entityCreateDataSystemTask);

    // PlayerAddSystem
    tf::Task playerAddSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("PlayerAddSystem", tracy::Color::Blue2)
            PlayerAddSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    playerAddSystemTask.gather(playerUpdateDataSystemTask);

    // ItemCreateSystem
    tf::Task entityAddSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("EntityAddSystem", tracy::Color::Blue2)
            EntityAddSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    entityAddSystemTask.gather(playerAddSystemTask);

    // PlayerDeleteSystem
    tf::Task playerDeleteSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("PlayerDeleteSystem", tracy::Color::Blue2)
            PlayerDeleteSystem::Update(registry);
        registry.ctx<ScriptSingleton>().CompleteSystem();
    });
    playerDeleteSystemTask.gather(entityAddSystemTask);

    // ScriptTransactionSystem - Always process this last, this is where all the changes from scripts gets applied back into the ECS
    tf::Task scriptTransactionSystemTask = framework.emplace([&registry]() {
        ZoneScopedNC("ScriptTransactionSystem", tracy::Color::Blue2)
            ScriptTransactionSystem::Update(registry);
        registry.ctx<ScriptSingleton>().ResetCompletedSystems();
    });
    scriptTransactionSystemTask.gather(playerDeleteSystemTask);
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
