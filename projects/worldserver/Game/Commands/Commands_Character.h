#pragma once
#include <NovusTypes.h>
#include <Utils/StringHash.h>
#include <../ECS/Components/ConnectionComponent.h>
#include <../ECS/Components/Singletons/CommandDataSingleton.h>

namespace Commands_Character
{
    static robin_hood::unordered_map<u32, CommandEntry> characterCommandMap;
    
    bool _Level(std::vector<std::string> commandStrings, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
    {
        u32 playerLevel = clientUpdateData.GetFieldValue<u32>(UNIT_FIELD_LEVEL);
        if (commandStrings.size() >= 3)
        {
            try
            {
                u32 level = std::stoi(commandStrings[2]);
                if (level <= 0)
                    level = 1;
                else if (level > 255)
                    level = 255;

                if (playerLevel != level)
                {
                    clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, level);

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
                        clientUpdateData.packetUpdateData.push_back(header.BuildHeaderPacket(achievementData));
                    }
                }
            }
            catch (std::exception) { }
        }
        else
        {
            if (playerLevel < 255)
                clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, playerLevel + 1);
        }

        return true;
    }
    bool _Speed(std::vector<std::string> commandStrings, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
    {
        if (commandStrings.size() >= 3)
        {
            try
            {
                f32 speed = std::stof(commandStrings[2]);
                if (speed <= 0)
                    speed = 1;
                else if (speed > 50)
                    speed = 50;

                Common::ByteBuffer speedChange;
                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_WALK_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                CharacterUtils::BuildSpeedChangePacket(clientConnection.accountGuid, clientConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, speedChange);
                clientUpdateData.packetUpdateData.push_back(speedChange);

                return true;
            }
            catch (std::exception) {}
        }

        return false;
    }
    bool _Fly(std::vector<std::string> commandStrings, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
    {
        if (commandStrings.size() >= 3)
        {
            try
            {
                bool canFly = std::stoi(commandStrings[2]);

                Common::ByteBuffer setFly;
                CharacterUtils::BuildFlyModePacket(clientConnection.accountGuid, clientConnection.characterGuid, canFly, setFly);
                clientUpdateData.packetUpdateData.push_back(setFly);

                return true;
            }
            catch (std::exception) {}
        }

        return false;
    }

    bool CharacterCommand(std::vector<std::string> commandStrings, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
    {
        std::string subCommand = commandStrings[1];

        auto itr = characterCommandMap.find(detail::fnv1a_32(subCommand.c_str(), subCommand.length()));
        if (itr != characterCommandMap.end())
        {
            return itr->second.handler(commandStrings, clientConnection, clientUpdateData);
        }

        return false;
    }
    void LoadCharacterCommands(entt::registry& registry, CommandDataSingleton& commandData)
    {
        commandData.commandMap[".char"_h] = CommandEntry(CharacterCommand);
        commandData.commandMap[".character"_h] = CommandEntry(CharacterCommand);

        characterCommandMap["level"_h] = CommandEntry(_Level);
        characterCommandMap["speed"_h] = CommandEntry(_Speed);
        characterCommandMap["fly"_h] = CommandEntry(_Fly);
    }
}