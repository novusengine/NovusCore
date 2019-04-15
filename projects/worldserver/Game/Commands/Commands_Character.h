/*
	MIT License

	Copyright (c) 2018-2019 NovusCore

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once
#include <NovusTypes.h>
#include <Utils/StringUtils.h>
#include <Math/Math.h>
#include <Math/Vector2.h>
#include "../ECS/Components/PlayerConnectionComponent.h"
#include "../ECS/Components/PlayerFieldDataComponent.h"
#include "../ECS/Components/Singletons/CommandDataSingleton.h"
#include "../ECS/Components/Singletons/PlayerPacketQueueSingleton.h"
#include "../ECS/Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "../ECS/Components/Singletons/MapSingleton.h"

namespace Commands_Character
{
    static robin_hood::unordered_map<u32, CommandEntry> characterCommandMap;
    static entt::registry* _registry = nullptr;

	bool _Level(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{
		try
		{
			u32 level = std::stoi(commandStrings[0]);
			if (level <= 0)
				level = 1;
			else if (level > 255)
				level = 255;

			PlayerPacketQueueSingleton & playerPacketQueue = _registry->ctx<PlayerPacketQueueSingleton>();
			PlayerFieldDataComponent & clientFieldData = _registry->get<PlayerFieldDataComponent>(clientConnection.entityGuid);
			u32 playerLevel = clientFieldData.GetFieldValue<u32>(UNIT_FIELD_LEVEL);
			if (playerLevel != level)
			{
				clientFieldData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, level);

				//I put this in here so that we can unlock the achievement frame when reaching level 10
				if (level >= 10 && playerLevel < 10)
				{
					Common::ByteBuffer achievementData;

					achievementData.AppendGuid(clientConnection.characterGuid);
					achievementData.Write<u32>(6); // Level 10

					tm lt;
					time_t const tmpServerTime = time(nullptr);
					localtime_s(&lt, &tmpServerTime);
					achievementData.Write<u32>(((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min));
					achievementData.Write<u32>(0);

					NovusHeader header;
					header.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::SMSG_ACHIEVEMENT_EARNED, achievementData.GetActualSize());
					playerPacketQueue.packetQueue->enqueue(header.BuildHeaderPacket(achievementData));
				}
			}

			return true;
		}
		catch (std::exception) {}

		return false;
	}
	bool _Speed(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{
		try
		{
			f32 speed = std::stof(commandStrings[0]);
			if (speed <= 0)
				speed = 1;
			else if (speed > 50)
				speed = 50;

			PlayerPacketQueueSingleton & playerPacketQueue = _registry->ctx<PlayerPacketQueueSingleton>();
			PlayerFieldDataComponent & clientFieldData = _registry->get<PlayerFieldDataComponent>(clientConnection.entityGuid);

			Common::ByteBuffer speedChange;
			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_WALK_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, speedChange);
			playerPacketQueue.packetQueue->enqueue(speedChange);

			clientConnection.SendNotification("Speed Updated: %f", speed);
			return true;
		}
		catch (std::exception) {}

		return false;
	}
	bool _Fly(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{
		try
		{
			std::string flightState = commandStrings[0];
			if (flightState == "on" || flightState == "off")
			{
				PlayerPacketQueueSingleton& playerPacketQueue = _registry->ctx<PlayerPacketQueueSingleton>();
				PlayerFieldDataComponent& clientFieldData = _registry->get<PlayerFieldDataComponent>(clientConnection.entityGuid);

				Common::ByteBuffer setFly;
				CharacterUtils::BuildFlyModePacket(clientConnection.accountGuid, clientConnection.characterGuid, flightState == "on", setFly);
				playerPacketQueue.packetQueue->enqueue(setFly);

				clientConnection.SendNotification("Flight Mode: %s", flightState.c_str());
				return true;
			}
		}
		catch (std::exception) {}

		return false;
	}
	bool _Tele(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{

		try
		{
			f32 x = std::stof(commandStrings[0]);
			f32 y = std::stof(commandStrings[1]);
			f32 z = std::stof(commandStrings[2]);

			SingletonComponent& singletonData = _registry->ctx<SingletonComponent>();
			PlayerPacketQueueSingleton& playerPacketQueue = _registry->ctx<PlayerPacketQueueSingleton>();
			PlayerFieldDataComponent& clientFieldData = _registry->get<PlayerFieldDataComponent>(clientConnection.entityGuid);
			PlayerPositionComponent& clientPositionData = _registry->get<PlayerPositionComponent>(clientConnection.entityGuid);

			Common::ByteBuffer buffer;
			buffer.AppendGuid(clientConnection.characterGuid);
			buffer.Write<u32>(0); // Teleport Count

			/* Movement */
			buffer.Write<u32>(0);
			buffer.Write<u16>(0);
			buffer.Write<u32>(static_cast<u32>(singletonData.lifeTimeInMS));

			buffer.Write<f32>(x);
			buffer.Write<f32>(y);
			buffer.Write<f32>(z);
			buffer.Write<f32>(clientPositionData.orientation);

			buffer.Write<u32>(0);

			NovusHeader header;
			header.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::MSG_MOVE_TELEPORT_ACK, buffer.GetActualSize());
			playerPacketQueue.packetQueue->enqueue(header.BuildHeaderPacket(buffer));

			return true;
		}
		catch (std::exception) {}

		return false;
	}
    bool _TeleToMap(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
    {
        /*
            try
            {
                u32 mapId = std::stoi(commandStrings[0]);
                f32 x = std::stof(commandStrings[1]);
                f32 y = std::stof(commandStrings[2]);
                f32 z = std::stof(commandStrings[3]);

                NovusHeader header;
                Common::ByteBuffer transfer;
                transfer.Write<u32>(mapId);

                header.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::SMSG_TRANSFER_PENDING, transfer.GetActualSize());
                clientFieldData.packetUpdateData.push_back(header.BuildHeaderPacket(transfer));

                Common::ByteBuffer newWorld;
                newWorld.Write<u32>(mapId);
                newWorld.Write<f32>(x);
                newWorld.Write<f32>(y);
                newWorld.Write<f32>(z);
                newWorld.Write<f32>(0);

                header.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::SMSG_NEW_WORLD, newWorld.GetActualSize());
                clientFieldData.packetUpdateData.push_back(header.BuildHeaderPacket(newWorld));

                return true;
            }
            catch (std::exception) {}
        */
        return false;
    }
	bool _AddItem(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{
		try
		{
			u32 itemEntry = std::stoi(commandStrings[0]);
			WorldDatabaseCacheSingleton& worldDatabaseCache = _registry->ctx<WorldDatabaseCacheSingleton>();

			ItemTemplate itemTemplate;
			if (!worldDatabaseCache.cache->GetItemTemplate(itemEntry, itemTemplate))
			{
				return false;
			}

			PlayerPacketQueueSingleton& playerPacketQueue = _registry->ctx<PlayerPacketQueueSingleton>();
			PlayerFieldDataComponent& playerFieldData = _registry->get<PlayerFieldDataComponent>(clientConnection.entityGuid);

			ItemCreationInformation itemCreationInformation;
			itemCreationInformation.itemEntry = itemTemplate.entry;
			itemCreationInformation.clientEntityGuid = clientConnection.entityGuid;
			itemCreationInformation.accountGuid = clientConnection.accountGuid;
			itemCreationInformation.characterGuid = clientConnection.characterGuid;

			u64 itemGuid = (static_cast<u64>(2) | (static_cast<u64>(itemTemplate.entry) << 24) | (static_cast<u64>(0x4000) << 48));
			playerFieldData.SetGuidValue(PLAYER_FIELD_PACK_SLOT_1, itemGuid);
			_registry->ctx<ItemCreateQueueSingleton>().newItemQueue->enqueue(itemCreationInformation);

			clientConnection.SendNotification("[AddItem]: %u", itemTemplate.entry);
			return true;
		}
		catch (std::exception) {}

		return false;
	}
	bool _DebugItem(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{
		try
		{
			u32 itemEntry = std::stoi(commandStrings[0]);
			WorldDatabaseCacheSingleton& worldDatabaseCache = _registry->ctx<WorldDatabaseCacheSingleton>();

			ItemTemplate itemTemplate;
			if (!worldDatabaseCache.cache->GetItemTemplate(itemEntry, itemTemplate))
			{
				return false;
			}

			PlayerFieldDataComponent& playerFieldData = _registry->get<PlayerFieldDataComponent>(clientConnection.entityGuid);

			u64 itemGuid = (static_cast<u64>(2) | (static_cast<u64>(itemTemplate.entry) << 24) | (static_cast<u64>(0x4000) << 48));
			playerFieldData.SetGuidValue(PLAYER_FIELD_PACK_SLOT_1, itemGuid);
			return true;
		}
		catch (std::exception) {}

		return false;
	}

	bool _GPS(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
	{
		PlayerPositionComponent& playerPos = _registry->get<PlayerPositionComponent>(clientConnection.entityGuid);
		MapSingleton& mapSingleton = _registry->ctx<MapSingleton>();
		PlayerPacketQueueSingleton& playerPacketQueue = _registry->ctx<PlayerPacketQueueSingleton>();

		u16 mapId = playerPos.mapId;
		f32 x = playerPos.x;
		f32 y = playerPos.y;
		f32 z = playerPos.z;

		f32 height = mapSingleton.maps[mapId].GetHeight(Vector2(x, y));
		clientConnection.SendNotification("MapID: %u (%f, %f, %f) Height: %f", mapId, x, y, z, height);

		return true;
	}

    bool CharacterCommand(std::vector<std::string> commandStrings, PlayerConnectionComponent& clientConnection)
    {
        std::string subCommand = commandStrings[1];

        auto itr = characterCommandMap.find(StringUtils::fnv1a_32(subCommand.c_str(), subCommand.length()));
        if (itr != characterCommandMap.end())
        {
			i32 parameters = itr->second.parameters;
			if (parameters == -1)
			{
				return itr->second.handler(commandStrings, clientConnection);
			}
			else
			{
				std::vector<std::string> paramStrings;
				if (parameters > 0)
				{
					if (commandStrings.size() - 2 < parameters)
						return false;

					for (i32 i = 2; i < parameters + 2; i++)
					{
						paramStrings.push_back(commandStrings[i]);
					}
				}

				return itr->second.handler(paramStrings, clientConnection);
			}
        }



        return false;
    }
    void LoadCharacterCommands(entt::registry& registry, CommandDataSingleton& commandData)
    {
        _registry = &registry;

        commandData.commandMap["char"_h] = CommandEntry(CharacterCommand, -1);
        commandData.commandMap["character"_h] = CommandEntry(CharacterCommand, -1);

        characterCommandMap["level"_h] = CommandEntry(_Level, 1);
        characterCommandMap["speed"_h] = CommandEntry(_Speed, 1);
        characterCommandMap["fly"_h] = CommandEntry(_Fly, 1);
        characterCommandMap["tele"_h] = CommandEntry(_Tele, 3);
        characterCommandMap["teletomap"_h] = CommandEntry(_TeleToMap, 4);
        characterCommandMap["additem"_h] = CommandEntry(_AddItem, 1);
        characterCommandMap["debugitem"_h] = CommandEntry(_DebugItem, 1);
		characterCommandMap["gps"_h] = CommandEntry(_GPS, 0);
    }
}