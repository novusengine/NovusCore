#include "GamemasterPacketUtils.h"
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../WorldNodeHandler.h"
#include "../../ECS/Components/Singletons/SingletonComponent.h"
#include "../../ECS/Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../../ECS/Components/PlayerConnectionComponent.h"
#include "../../ECS/Components/PlayerPositionComponent.h"
#include "../../ECS/Components/PlayerFieldDataComponent.h"

#include "../../Utils/ServiceLocator.h"
#include <tracy/Tracy.hpp>

void GamemasterPacketUtils::RegisterGamemasterPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_SET_ALL_SPEED_CHEAT, HandleSetAllSpeedCheat);
    messageHandler->SetMessageHandler(Opcode::CMSG_MOVE_START_SWIM_CHEAT, HandleStartSwimCheat);
    messageHandler->SetMessageHandler(Opcode::CMSG_MOVE_STOP_SWIM_CHEAT, HandleStopSwimCheat);
    messageHandler->SetMessageHandler(Opcode::CMSG_QUERY_OBJECT_POSITION, HandleQueryObjectPosition);
    messageHandler->SetMessageHandler(Opcode::CMSG_LEVEL_CHEAT, HandleLevelCheat);
}

bool GamemasterPacketUtils::HandleSetAllSpeedCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    f32 speed = 1;
    packet->data->GetF32(speed);

    entt::registry* registry = ServiceLocator::GetMainRegistry();
    std::shared_ptr<ByteBuffer> speedChange = ByteBuffer::Borrow<12>();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_WALK_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_WALK_SPEED_CHANGE);
    speedChange->Reset();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_RUN_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_RUN_SPEED_CHANGE);
    speedChange->Reset();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE);
    speedChange->Reset();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE);
    speedChange->Reset();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE);
    speedChange->Reset();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE);
    speedChange->Reset();

    CharacterUtils::BuildSpeedChangePacket(playerConnectionComponent->characterGuid, speed, Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, speedChange);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, speedChange, Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE);

    playerConnectionComponent->SendChatNotification("Speed Updated: %f", speed);
    return true;
}
bool GamemasterPacketUtils::HandleStartSwimCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    SingletonComponent& singletonComponent = registry->ctx<SingletonComponent>();
    PlayerPositionComponent& playerPositionComponent = registry->get<PlayerPositionComponent>(playerConnectionComponent->entityId);

    std::shared_ptr<ByteBuffer> flyMode = ByteBuffer::Borrow<12>();
    CharacterUtils::BuildFlyModePacket(playerConnectionComponent->characterGuid, flyMode);
    playerConnectionComponent->socket->SendPacket(flyMode.get(), Opcode::SMSG_MOVE_SET_CAN_FLY);

    std::shared_ptr<ByteBuffer> movementBuffer = ByteBuffer::Borrow<97>();
    movementBuffer->PutGuid(playerConnectionComponent->characterGuid);
    playerPositionComponent.WriteMovementData(movementBuffer, static_cast<u32>(singletonComponent.lifeTimeInMS));
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, movementBuffer, Opcode::MSG_MOVE_UPDATE_CAN_FLY, true);
    
    return true;
}
bool GamemasterPacketUtils::HandleStopSwimCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    SingletonComponent& singletonComponent = registry->ctx<SingletonComponent>();
    PlayerPositionComponent& playerPositionComponent = registry->get<PlayerPositionComponent>(playerConnectionComponent->entityId);

    std::shared_ptr<ByteBuffer> flyMode = ByteBuffer::Borrow<12>();
    CharacterUtils::BuildFlyModePacket(playerConnectionComponent->characterGuid, flyMode);
    playerConnectionComponent->socket->SendPacket(flyMode.get(), Opcode::SMSG_MOVE_UNSET_CAN_FLY);

    std::shared_ptr<ByteBuffer> movementBuffer = ByteBuffer::Borrow<97>();
    movementBuffer->PutGuid(playerConnectionComponent->characterGuid);
    playerPositionComponent.WriteMovementData(movementBuffer, static_cast<u32>(singletonComponent.lifeTimeInMS));
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, movementBuffer, Opcode::MSG_MOVE_UPDATE_CAN_FLY, true);

    return true;
}

bool GamemasterPacketUtils::HandleQueryObjectPosition(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerPositionComponent& playerPositionComponent = registry->get<PlayerPositionComponent>(playerConnectionComponent->entityId);

    std::shared_ptr<ByteBuffer> objectPosition = ByteBuffer::Borrow<12>();
    objectPosition->Put<Vector3>(playerPositionComponent.movementData.position);

    playerConnectionComponent->socket->SendPacket(objectPosition.get(), Opcode::SMSG_QUERY_OBJECT_POSITION);
    return true;
}

bool GamemasterPacketUtils::HandleLevelCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    u32 level = 0;
    packet->data->GetU32(level);

    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldDataComponent = registry->get<PlayerFieldDataComponent>(playerConnectionComponent->entityId);

    if (level != playerFieldDataComponent.GetFieldValue<u32>(UNIT_FIELD_LEVEL))
        playerFieldDataComponent.SetFieldValue<u32>(UNIT_FIELD_LEVEL, level);

    return true;
}
