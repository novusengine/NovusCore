/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once

#include <asio\ip\tcp.hpp>
#include <Networking\BaseSocket.h>
#include <Networking\TcpServer.h>
#include <Cryptography\BigNumber.h>
#include <Cryptography\StreamCrypto.h>
#include <unordered_map>

enum NovusCommand
{
    NOVUS_CHALLENGE         = 0x00,
    NOVUS_PROOF             = 0x01,
    NOVUS_FOWARDPACKET      = 0x02
};
enum NovusStatus
{
    NOVUSSTATUS_CHALLENGE   = 0x0,
    NOVUSSTATUS_PROOF       = 0x1,
    NOVUSSTATUS_AUTHED      = 0x2,
    NOVUSSTATUS_CLOSED      = 0x3
};

enum EObjectFields
{
    OBJECT_FIELD_GUID                         = 0x0000, // Size: 2, Type: LONG, Flags: PUBLIC
    OBJECT_FIELD_TYPE                         = 0x0002, // Size: 1, Type: INT, Flags: PUBLIC
    OBJECT_FIELD_ENTRY                        = 0x0003, // Size: 1, Type: INT, Flags: PUBLIC
    OBJECT_FIELD_SCALE_X                      = 0x0004, // Size: 1, Type: FLOAT, Flags: PUBLIC
    OBJECT_FIELD_PADDING                      = 0x0005, // Size: 1, Type: INT, Flags: NONE
    OBJECT_END                                = 0x0006
};

enum EUnitFields
{
    UNIT_FIELD_CHARM                          = OBJECT_END + 0x0000, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_SUMMON                         = OBJECT_END + 0x0002, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_CRITTER                        = OBJECT_END + 0x0004, // Size: 2, Type: LONG, Flags: PRIVATE
    UNIT_FIELD_CHARMEDBY                      = OBJECT_END + 0x0006, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_SUMMONEDBY                     = OBJECT_END + 0x0008, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_CREATEDBY                      = OBJECT_END + 0x000A, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_TARGET                         = OBJECT_END + 0x000C, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_CHANNEL_OBJECT                 = OBJECT_END + 0x000E, // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_CHANNEL_SPELL                        = OBJECT_END + 0x0010, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_BYTES_0                        = OBJECT_END + 0x0011, // Size: 1, Type: BYTES, Flags: PUBLIC
    UNIT_FIELD_HEALTH                         = OBJECT_END + 0x0012, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER1                         = OBJECT_END + 0x0013, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER2                         = OBJECT_END + 0x0014, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER3                         = OBJECT_END + 0x0015, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER4                         = OBJECT_END + 0x0016, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER5                         = OBJECT_END + 0x0017, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER6                         = OBJECT_END + 0x0018, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER7                         = OBJECT_END + 0x0019, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXHEALTH                      = OBJECT_END + 0x001A, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER1                      = OBJECT_END + 0x001B, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER2                      = OBJECT_END + 0x001C, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER3                      = OBJECT_END + 0x001D, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER4                      = OBJECT_END + 0x001E, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER5                      = OBJECT_END + 0x001F, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER6                      = OBJECT_END + 0x0020, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER7                      = OBJECT_END + 0x0021, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER      = OBJECT_END + 0x0022, // Size: 7, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER = OBJECT_END + 0x0029, // Size: 7, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_LEVEL                          = OBJECT_END + 0x0030, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_FACTIONTEMPLATE                = OBJECT_END + 0x0031, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_VIRTUAL_ITEM_SLOT_ID                 = OBJECT_END + 0x0032, // Size: 3, Type: INT, Flags: PUBLIC
    UNIT_FIELD_FLAGS                          = OBJECT_END + 0x0035, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_FLAGS_2                        = OBJECT_END + 0x0036, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_AURASTATE                      = OBJECT_END + 0x0037, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_BASEATTACKTIME                 = OBJECT_END + 0x0038, // Size: 2, Type: INT, Flags: PUBLIC
    UNIT_FIELD_RANGEDATTACKTIME               = OBJECT_END + 0x003A, // Size: 1, Type: INT, Flags: PRIVATE
    UNIT_FIELD_BOUNDINGRADIUS                 = OBJECT_END + 0x003B, // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_FIELD_COMBATREACH                    = OBJECT_END + 0x003C, // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_FIELD_DISPLAYID                      = OBJECT_END + 0x003D, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_NATIVEDISPLAYID                = OBJECT_END + 0x003E, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MOUNTDISPLAYID                 = OBJECT_END + 0x003F, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MINDAMAGE                      = OBJECT_END + 0x0040, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_MAXDAMAGE                      = OBJECT_END + 0x0041, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_MINOFFHANDDAMAGE               = OBJECT_END + 0x0042, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_MAXOFFHANDDAMAGE               = OBJECT_END + 0x0043, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_BYTES_1                        = OBJECT_END + 0x0044, // Size: 1, Type: BYTES, Flags: PUBLIC
    UNIT_FIELD_PETNUMBER                      = OBJECT_END + 0x0045, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_PET_NAME_TIMESTAMP             = OBJECT_END + 0x0046, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_PETEXPERIENCE                  = OBJECT_END + 0x0047, // Size: 1, Type: INT, Flags: OWNER
    UNIT_FIELD_PETNEXTLEVELEXP                = OBJECT_END + 0x0048, // Size: 1, Type: INT, Flags: OWNER
    UNIT_DYNAMIC_FLAGS                        = OBJECT_END + 0x0049, // Size: 1, Type: INT, Flags: DYNAMIC
    UNIT_MOD_CAST_SPEED                       = OBJECT_END + 0x004A, // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_CREATED_BY_SPELL                     = OBJECT_END + 0x004B, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_NPC_FLAGS                            = OBJECT_END + 0x004C, // Size: 1, Type: INT, Flags: DYNAMIC
    UNIT_NPC_EMOTESTATE                       = OBJECT_END + 0x004D, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_STAT0                          = OBJECT_END + 0x004E, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT1                          = OBJECT_END + 0x004F, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT2                          = OBJECT_END + 0x0050, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT3                          = OBJECT_END + 0x0051, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT4                          = OBJECT_END + 0x0052, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT0                       = OBJECT_END + 0x0053, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT1                       = OBJECT_END + 0x0054, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT2                       = OBJECT_END + 0x0055, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT3                       = OBJECT_END + 0x0056, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT4                       = OBJECT_END + 0x0057, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT0                       = OBJECT_END + 0x0058, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT1                       = OBJECT_END + 0x0059, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT2                       = OBJECT_END + 0x005A, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT3                       = OBJECT_END + 0x005B, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT4                       = OBJECT_END + 0x005C, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RESISTANCES                    = OBJECT_END + 0x005D, // Size: 7, Type: INT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE     = OBJECT_END + 0x0064, // Size: 7, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE     = OBJECT_END + 0x006B, // Size: 7, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_BASE_MANA                      = OBJECT_END + 0x0072, // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_BASE_HEALTH                    = OBJECT_END + 0x0073, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_BYTES_2                        = OBJECT_END + 0x0074, // Size: 1, Type: BYTES, Flags: PUBLIC
    UNIT_FIELD_ATTACK_POWER                   = OBJECT_END + 0x0075, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_ATTACK_POWER_MODS              = OBJECT_END + 0x0076, // Size: 1, Type: TWO_SHORT, Flags: PRIVATE, OWNER
    UNIT_FIELD_ATTACK_POWER_MULTIPLIER        = OBJECT_END + 0x0077, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER            = OBJECT_END + 0x0078, // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MODS       = OBJECT_END + 0x0079, // Size: 1, Type: TWO_SHORT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = OBJECT_END + 0x007A, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_MINRANGEDDAMAGE                = OBJECT_END + 0x007B, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_MAXRANGEDDAMAGE                = OBJECT_END + 0x007C, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POWER_COST_MODIFIER            = OBJECT_END + 0x007D, // Size: 7, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POWER_COST_MULTIPLIER          = OBJECT_END + 0x0084, // Size: 7, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_MAXHEALTHMODIFIER              = OBJECT_END + 0x008B, // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_HOVERHEIGHT                    = OBJECT_END + 0x008C, // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_FIELD_PADDING                        = OBJECT_END + 0x008D, // Size: 1, Type: INT, Flags: NONE
    UNIT_END                                  = OBJECT_END + 0x008E,

    PLAYER_DUEL_ARBITER                       = UNIT_END + 0x0000, // Size: 2, Type: LONG, Flags: PUBLIC
    PLAYER_FLAGS                              = UNIT_END + 0x0002, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_GUILDID                            = UNIT_END + 0x0003, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_GUILDRANK                          = UNIT_END + 0x0004, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_BYTES                              = UNIT_END + 0x0005, // Size: 1, Type: BYTES, Flags: PUBLIC
    PLAYER_BYTES_2                            = UNIT_END + 0x0006, // Size: 1, Type: BYTES, Flags: PUBLIC
    PLAYER_BYTES_3                            = UNIT_END + 0x0007, // Size: 1, Type: BYTES, Flags: PUBLIC
    PLAYER_DUEL_TEAM                          = UNIT_END + 0x0008, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_GUILD_TIMESTAMP                    = UNIT_END + 0x0009, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_QUEST_LOG_1_1                      = UNIT_END + 0x000A, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_1_2                      = UNIT_END + 0x000B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_1_3                      = UNIT_END + 0x000C, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_1_4                      = UNIT_END + 0x000E, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_2_1                      = UNIT_END + 0x000F, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_2_2                      = UNIT_END + 0x0010, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_2_3                      = UNIT_END + 0x0011, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_2_5                      = UNIT_END + 0x0013, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_3_1                      = UNIT_END + 0x0014, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_3_2                      = UNIT_END + 0x0015, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_3_3                      = UNIT_END + 0x0016, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_3_5                      = UNIT_END + 0x0018, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_4_1                      = UNIT_END + 0x0019, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_4_2                      = UNIT_END + 0x001A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_4_3                      = UNIT_END + 0x001B, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_4_5                      = UNIT_END + 0x001D, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_5_1                      = UNIT_END + 0x001E, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_5_2                      = UNIT_END + 0x001F, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_5_3                      = UNIT_END + 0x0020, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_5_5                      = UNIT_END + 0x0022, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_6_1                      = UNIT_END + 0x0023, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_6_2                      = UNIT_END + 0x0024, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_6_3                      = UNIT_END + 0x0025, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_6_5                      = UNIT_END + 0x0027, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_7_1                      = UNIT_END + 0x0028, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_7_2                      = UNIT_END + 0x0029, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_7_3                      = UNIT_END + 0x002A, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_7_5                      = UNIT_END + 0x002C, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_8_1                      = UNIT_END + 0x002D, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_8_2                      = UNIT_END + 0x002E, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_8_3                      = UNIT_END + 0x002F, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_8_5                      = UNIT_END + 0x0031, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_9_1                      = UNIT_END + 0x0032, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_9_2                      = UNIT_END + 0x0033, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_9_3                      = UNIT_END + 0x0034, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_9_5                      = UNIT_END + 0x0036, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_10_1                     = UNIT_END + 0x0037, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_10_2                     = UNIT_END + 0x0038, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_10_3                     = UNIT_END + 0x0039, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_10_5                     = UNIT_END + 0x003B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_11_1                     = UNIT_END + 0x003C, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_11_2                     = UNIT_END + 0x003D, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_11_3                     = UNIT_END + 0x003E, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_11_5                     = UNIT_END + 0x0040, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_12_1                     = UNIT_END + 0x0041, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_12_2                     = UNIT_END + 0x0042, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_12_3                     = UNIT_END + 0x0043, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_12_5                     = UNIT_END + 0x0045, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_13_1                     = UNIT_END + 0x0046, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_13_2                     = UNIT_END + 0x0047, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_13_3                     = UNIT_END + 0x0048, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_13_5                     = UNIT_END + 0x004A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_14_1                     = UNIT_END + 0x004B, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_14_2                     = UNIT_END + 0x004C, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_14_3                     = UNIT_END + 0x004D, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_14_5                     = UNIT_END + 0x004F, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_15_1                     = UNIT_END + 0x0050, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_15_2                     = UNIT_END + 0x0051, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_15_3                     = UNIT_END + 0x0052, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_15_5                     = UNIT_END + 0x0054, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_16_1                     = UNIT_END + 0x0055, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_16_2                     = UNIT_END + 0x0056, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_16_3                     = UNIT_END + 0x0057, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_16_5                     = UNIT_END + 0x0059, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_17_1                     = UNIT_END + 0x005A, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_17_2                     = UNIT_END + 0x005B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_17_3                     = UNIT_END + 0x005C, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_17_5                     = UNIT_END + 0x005E, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_18_1                     = UNIT_END + 0x005F, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_18_2                     = UNIT_END + 0x0060, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_18_3                     = UNIT_END + 0x0061, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_18_5                     = UNIT_END + 0x0063, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_19_1                     = UNIT_END + 0x0064, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_19_2                     = UNIT_END + 0x0065, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_19_3                     = UNIT_END + 0x0066, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_19_5                     = UNIT_END + 0x0068, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_20_1                     = UNIT_END + 0x0069, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_20_2                     = UNIT_END + 0x006A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_20_3                     = UNIT_END + 0x006B, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_20_5                     = UNIT_END + 0x006D, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_21_1                     = UNIT_END + 0x006E, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_21_2                     = UNIT_END + 0x006F, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_21_3                     = UNIT_END + 0x0070, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_21_5                     = UNIT_END + 0x0072, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_22_1                     = UNIT_END + 0x0073, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_22_2                     = UNIT_END + 0x0074, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_22_3                     = UNIT_END + 0x0075, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_22_5                     = UNIT_END + 0x0077, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_23_1                     = UNIT_END + 0x0078, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_23_2                     = UNIT_END + 0x0079, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_23_3                     = UNIT_END + 0x007A, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_23_5                     = UNIT_END + 0x007C, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_24_1                     = UNIT_END + 0x007D, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_24_2                     = UNIT_END + 0x007E, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_24_3                     = UNIT_END + 0x007F, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_24_5                     = UNIT_END + 0x0081, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_25_1                     = UNIT_END + 0x0082, // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_25_2                     = UNIT_END + 0x0083, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_25_3                     = UNIT_END + 0x0084, // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_25_5                     = UNIT_END + 0x0086, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_VISIBLE_ITEM_1_ENTRYID             = UNIT_END + 0x0087, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_1_ENCHANTMENT         = UNIT_END + 0x0088, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_2_ENTRYID             = UNIT_END + 0x0089, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_2_ENCHANTMENT         = UNIT_END + 0x008A, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_3_ENTRYID             = UNIT_END + 0x008B, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_3_ENCHANTMENT         = UNIT_END + 0x008C, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_4_ENTRYID             = UNIT_END + 0x008D, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_4_ENCHANTMENT         = UNIT_END + 0x008E, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_5_ENTRYID             = UNIT_END + 0x008F, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_5_ENCHANTMENT         = UNIT_END + 0x0090, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_6_ENTRYID             = UNIT_END + 0x0091, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_6_ENCHANTMENT         = UNIT_END + 0x0092, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_7_ENTRYID             = UNIT_END + 0x0093, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_7_ENCHANTMENT         = UNIT_END + 0x0094, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_8_ENTRYID             = UNIT_END + 0x0095, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_8_ENCHANTMENT         = UNIT_END + 0x0096, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_9_ENTRYID             = UNIT_END + 0x0097, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_9_ENCHANTMENT         = UNIT_END + 0x0098, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_10_ENTRYID            = UNIT_END + 0x0099, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_10_ENCHANTMENT        = UNIT_END + 0x009A, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_11_ENTRYID            = UNIT_END + 0x009B, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_11_ENCHANTMENT        = UNIT_END + 0x009C, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_12_ENTRYID            = UNIT_END + 0x009D, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_12_ENCHANTMENT        = UNIT_END + 0x009E, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_13_ENTRYID            = UNIT_END + 0x009F, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_13_ENCHANTMENT        = UNIT_END + 0x00A0, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_14_ENTRYID            = UNIT_END + 0x00A1, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_14_ENCHANTMENT        = UNIT_END + 0x00A2, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_15_ENTRYID            = UNIT_END + 0x00A3, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_15_ENCHANTMENT        = UNIT_END + 0x00A4, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_16_ENTRYID            = UNIT_END + 0x00A5, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_16_ENCHANTMENT        = UNIT_END + 0x00A6, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_17_ENTRYID            = UNIT_END + 0x00A7, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_17_ENCHANTMENT        = UNIT_END + 0x00A8, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_18_ENTRYID            = UNIT_END + 0x00A9, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_18_ENCHANTMENT        = UNIT_END + 0x00AA, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_19_ENTRYID            = UNIT_END + 0x00AB, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_19_ENCHANTMENT        = UNIT_END + 0x00AC, // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_CHOSEN_TITLE                       = UNIT_END + 0x00AD, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_FAKE_INEBRIATION                   = UNIT_END + 0x00AE, // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_FIELD_PAD_0                        = UNIT_END + 0x00AF, // Size: 1, Type: INT, Flags: NONE
    PLAYER_FIELD_INV_SLOT_HEAD                = UNIT_END + 0x00B0, // Size: 46, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_PACK_SLOT_1                  = UNIT_END + 0x00DE, // Size: 32, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_BANK_SLOT_1                  = UNIT_END + 0x00FE, // Size: 56, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_BANKBAG_SLOT_1               = UNIT_END + 0x0136, // Size: 14, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_VENDORBUYBACK_SLOT_1         = UNIT_END + 0x0144, // Size: 24, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_KEYRING_SLOT_1               = UNIT_END + 0x015C, // Size: 64, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_CURRENCYTOKEN_SLOT_1         = UNIT_END + 0x019C, // Size: 64, Type: LONG, Flags: PRIVATE
    PLAYER_FARSIGHT                           = UNIT_END + 0x01DC, // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER__FIELD_KNOWN_TITLES                 = UNIT_END + 0x01DE, // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER__FIELD_KNOWN_TITLES1                = UNIT_END + 0x01E0, // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER__FIELD_KNOWN_TITLES2                = UNIT_END + 0x01E2, // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_KNOWN_CURRENCIES             = UNIT_END + 0x01E4, // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER_XP                                 = UNIT_END + 0x01E6, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_NEXT_LEVEL_XP                      = UNIT_END + 0x01E7, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_SKILL_INFO_1_1                     = UNIT_END + 0x01E8, // Size: 384, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_CHARACTER_POINTS1                  = UNIT_END + 0x0368, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_CHARACTER_POINTS2                  = UNIT_END + 0x0369, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_TRACK_CREATURES                    = UNIT_END + 0x036A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_TRACK_RESOURCES                    = UNIT_END + 0x036B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_BLOCK_PERCENTAGE                   = UNIT_END + 0x036C, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_DODGE_PERCENTAGE                   = UNIT_END + 0x036D, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_PARRY_PERCENTAGE                   = UNIT_END + 0x036E, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_EXPERTISE                          = UNIT_END + 0x036F, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_OFFHAND_EXPERTISE                  = UNIT_END + 0x0370, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_CRIT_PERCENTAGE                    = UNIT_END + 0x0371, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_RANGED_CRIT_PERCENTAGE             = UNIT_END + 0x0372, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_OFFHAND_CRIT_PERCENTAGE            = UNIT_END + 0x0373, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_SPELL_CRIT_PERCENTAGE1             = UNIT_END + 0x0374, // Size: 7, Type: FLOAT, Flags: PRIVATE
    PLAYER_SHIELD_BLOCK                       = UNIT_END + 0x037B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE       = UNIT_END + 0x037C, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_EXPLORED_ZONES_1                   = UNIT_END + 0x037D, // Size: 128, Type: BYTES, Flags: PRIVATE
    PLAYER_REST_STATE_EXPERIENCE              = UNIT_END + 0x03FD, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_COINAGE                      = UNIT_END + 0x03FE, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_POS          = UNIT_END + 0x03FF, // Size: 7, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_NEG          = UNIT_END + 0x0406, // Size: 7, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_PCT          = UNIT_END + 0x040D, // Size: 7, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_HEALING_DONE_POS         = UNIT_END + 0x0414, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_HEALING_PCT              = UNIT_END + 0x0415, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_FIELD_MOD_HEALING_DONE_PCT         = UNIT_END + 0x0416, // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_FIELD_MOD_TARGET_RESISTANCE        = UNIT_END + 0x0417, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE = UNIT_END + 0x0418, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BYTES                        = UNIT_END + 0x0419, // Size: 1, Type: BYTES, Flags: PRIVATE
    PLAYER_AMMO_ID                            = UNIT_END + 0x041A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_SELF_RES_SPELL                     = UNIT_END + 0x041B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_PVP_MEDALS                   = UNIT_END + 0x041C, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BUYBACK_PRICE_1              = UNIT_END + 0x041D, // Size: 12, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BUYBACK_TIMESTAMP_1          = UNIT_END + 0x0429, // Size: 12, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_KILLS                        = UNIT_END + 0x0435, // Size: 1, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_FIELD_TODAY_CONTRIBUTION           = UNIT_END + 0x0436, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_YESTERDAY_CONTRIBUTION       = UNIT_END + 0x0437, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_LIFETIME_HONORABLE_KILLS     = UNIT_END + 0x0438, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BYTES2                       = UNIT_END + 0x0439, // Size: 1, Type: 6, Flags: PRIVATE
    PLAYER_FIELD_WATCHED_FACTION_INDEX        = UNIT_END + 0x043A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_COMBAT_RATING_1              = UNIT_END + 0x043B, // Size: 25, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_ARENA_TEAM_INFO_1_1          = UNIT_END + 0x0454, // Size: 21, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_HONOR_CURRENCY               = UNIT_END + 0x0469, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_ARENA_CURRENCY               = UNIT_END + 0x046A, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MAX_LEVEL                    = UNIT_END + 0x046B, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_DAILY_QUESTS_1               = UNIT_END + 0x046C, // Size: 25, Type: INT, Flags: PRIVATE
    PLAYER_RUNE_REGEN_1                       = UNIT_END + 0x0485, // Size: 4, Type: FLOAT, Flags: PRIVATE
    PLAYER_NO_REAGENT_COST_1                  = UNIT_END + 0x0489, // Size: 3, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_GLYPH_SLOTS_1                = UNIT_END + 0x048C, // Size: 6, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_GLYPHS_1                     = UNIT_END + 0x0492, // Size: 6, Type: INT, Flags: PRIVATE
    PLAYER_GLYPHS_ENABLED                     = UNIT_END + 0x0498, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_PET_SPELL_POWER                    = UNIT_END + 0x0499, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_END                                = UNIT_END + 0x049A
};

enum UpdatefieldFlags
{
    UF_FLAG_NONE = 0x000,
    UF_FLAG_PUBLIC = 0x001,
    UF_FLAG_PRIVATE = 0x002,
    UF_FLAG_OWNER = 0x004,
    UF_FLAG_UNUSED1 = 0x008,
    UF_FLAG_ITEM_OWNER = 0x010,
    UF_FLAG_SPECIAL_INFO = 0x020,
    UF_FLAG_PARTY_MEMBER = 0x040,
    UF_FLAG_UNUSED2 = 0x080,
    UF_FLAG_DYNAMIC = 0x100
};

static u32 UnitUpdateFieldFlags[PLAYER_END] =
{
    UF_FLAG_PUBLIC,                                         // OBJECT_FIELD_GUID
    UF_FLAG_PUBLIC,                                         // OBJECT_FIELD_GUID+1
    UF_FLAG_PUBLIC,                                         // OBJECT_FIELD_TYPE
    UF_FLAG_PUBLIC,                                         // OBJECT_FIELD_ENTRY
    UF_FLAG_PUBLIC,                                         // OBJECT_FIELD_SCALE_X
    UF_FLAG_NONE,                                           // OBJECT_FIELD_PADDING
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CHARM
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CHARM+1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_SUMMON
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_SUMMON+1
    UF_FLAG_PRIVATE,                                        // UNIT_FIELD_CRITTER
    UF_FLAG_PRIVATE,                                        // UNIT_FIELD_CRITTER+1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CHARMEDBY
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CHARMEDBY+1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_SUMMONEDBY
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_SUMMONEDBY+1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CREATEDBY
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CREATEDBY+1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_TARGET
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_TARGET+1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CHANNEL_OBJECT
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_CHANNEL_OBJECT+1
    UF_FLAG_PUBLIC,                                         // UNIT_CHANNEL_SPELL
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BYTES_0
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_HEALTH
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER2
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER3
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER4
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER5
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER6
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_POWER7
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXHEALTH
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER2
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER3
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER4
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER5
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER6
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MAXPOWER7
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+6
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+6
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_LEVEL
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_FACTIONTEMPLATE
    UF_FLAG_PUBLIC,                                         // UNIT_VIRTUAL_ITEM_SLOT_ID
    UF_FLAG_PUBLIC,                                         // UNIT_VIRTUAL_ITEM_SLOT_ID+1
    UF_FLAG_PUBLIC,                                         // UNIT_VIRTUAL_ITEM_SLOT_ID+2
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_FLAGS
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_FLAGS_2
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_AURASTATE
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BASEATTACKTIME
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BASEATTACKTIME+1
    UF_FLAG_PRIVATE,                                        // UNIT_FIELD_RANGEDATTACKTIME
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BOUNDINGRADIUS
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_COMBATREACH
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_DISPLAYID
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_NATIVEDISPLAYID
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_MOUNTDISPLAYID
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_MINDAMAGE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_MAXDAMAGE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_MINOFFHANDDAMAGE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_MAXOFFHANDDAMAGE
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BYTES_1
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_PETNUMBER
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_PET_NAME_TIMESTAMP
    UF_FLAG_OWNER,                                          // UNIT_FIELD_PETEXPERIENCE
    UF_FLAG_OWNER,                                          // UNIT_FIELD_PETNEXTLEVELEXP
    UF_FLAG_DYNAMIC,                                        // UNIT_DYNAMIC_FLAGS
    UF_FLAG_PUBLIC,                                         // UNIT_MOD_CAST_SPEED
    UF_FLAG_PUBLIC,                                         // UNIT_CREATED_BY_SPELL
    UF_FLAG_DYNAMIC,                                        // UNIT_NPC_FLAGS
    UF_FLAG_PUBLIC,                                         // UNIT_NPC_EMOTESTATE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_STAT0
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_STAT1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_STAT2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_STAT3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_STAT4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POSSTAT0
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POSSTAT1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POSSTAT2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POSSTAT3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POSSTAT4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_NEGSTAT0
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_NEGSTAT1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_NEGSTAT2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_NEGSTAT3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_NEGSTAT4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER | UF_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+6
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+6
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+6
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BASE_MANA
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_BASE_HEALTH
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_BYTES_2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_ATTACK_POWER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_ATTACK_POWER_MODS
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_ATTACK_POWER_MULTIPLIER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RANGED_ATTACK_POWER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RANGED_ATTACK_POWER_MODS
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_MINRANGEDDAMAGE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_MAXRANGEDDAMAGE
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MODIFIER+6
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER+1
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER+2
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER+3
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER+4
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER+5
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_POWER_COST_MULTIPLIER+6
    UF_FLAG_PRIVATE | UF_FLAG_OWNER,                        // UNIT_FIELD_MAXHEALTHMODIFIER
    UF_FLAG_PUBLIC,                                         // UNIT_FIELD_HOVERHEIGHT
    UF_FLAG_NONE,                                           // UNIT_FIELD_PADDING
    UF_FLAG_PUBLIC,                                         // PLAYER_DUEL_ARBITER
    UF_FLAG_PUBLIC,                                         // PLAYER_DUEL_ARBITER+1
    UF_FLAG_PUBLIC,                                         // PLAYER_FLAGS
    UF_FLAG_PUBLIC,                                         // PLAYER_GUILDID
    UF_FLAG_PUBLIC,                                         // PLAYER_GUILDRANK
    UF_FLAG_PUBLIC,                                         // PLAYER_BYTES
    UF_FLAG_PUBLIC,                                         // PLAYER_BYTES_2
    UF_FLAG_PUBLIC,                                         // PLAYER_BYTES_3
    UF_FLAG_PUBLIC,                                         // PLAYER_DUEL_TEAM
    UF_FLAG_PUBLIC,                                         // PLAYER_GUILD_TIMESTAMP
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_1_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_1_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_1_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_1_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_1_4
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_2_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_2_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_2_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_2_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_2_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_3_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_3_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_3_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_3_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_3_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_4_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_4_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_4_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_4_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_4_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_5_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_5_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_5_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_5_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_5_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_6_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_6_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_6_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_6_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_6_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_7_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_7_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_7_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_7_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_7_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_8_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_8_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_8_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_8_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_8_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_9_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_9_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_9_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_9_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_9_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_10_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_10_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_10_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_10_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_10_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_11_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_11_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_11_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_11_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_11_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_12_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_12_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_12_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_12_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_12_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_13_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_13_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_13_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_13_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_13_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_14_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_14_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_14_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_14_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_14_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_15_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_15_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_15_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_15_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_15_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_16_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_16_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_16_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_16_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_16_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_17_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_17_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_17_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_17_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_17_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_18_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_18_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_18_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_18_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_18_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_19_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_19_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_19_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_19_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_19_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_20_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_20_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_20_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_20_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_20_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_21_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_21_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_21_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_21_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_21_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_22_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_22_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_22_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_22_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_22_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_23_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_23_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_23_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_23_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_23_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_24_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_24_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_24_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_24_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_24_5
    UF_FLAG_PARTY_MEMBER,                                   // PLAYER_QUEST_LOG_25_1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_25_2
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_25_3
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_25_3+1
    UF_FLAG_PRIVATE,                                        // PLAYER_QUEST_LOG_25_5
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_1_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_1_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_2_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_2_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_3_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_3_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_4_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_4_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_5_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_5_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_6_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_6_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_7_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_7_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_8_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_8_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_9_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_9_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_10_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_10_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_11_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_11_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_12_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_12_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_13_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_13_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_14_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_14_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_15_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_15_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_16_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_16_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_17_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_17_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_18_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_18_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_19_ENTRYID
    UF_FLAG_PUBLIC,                                         // PLAYER_VISIBLE_ITEM_19_ENCHANTMENT
    UF_FLAG_PUBLIC,                                         // PLAYER_CHOSEN_TITLE
    UF_FLAG_PUBLIC,                                         // PLAYER_FAKE_INEBRIATION
    UF_FLAG_NONE,                                           // PLAYER_FIELD_PAD_0
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+24
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+25
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+26
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+27
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+28
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+29
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+30
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+31
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+32
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+33
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+34
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+35
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+36
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+37
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+38
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+39
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+40
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+41
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+42
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+43
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+44
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_INV_SLOT_HEAD+45
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+25
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+26
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+27
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+28
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+29
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+30
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PACK_SLOT_1+31
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+25
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+26
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+27
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+28
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+29
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+30
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+31
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+32
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+33
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+34
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+35
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+36
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+37
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+38
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+39
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+40
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+41
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+42
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+43
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+44
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+45
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+46
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+47
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+48
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+49
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+50
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+51
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+52
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+53
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+54
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANK_SLOT_1+55
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BANKBAG_SLOT_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+25
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+26
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+27
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+28
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+29
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+30
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+31
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+32
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+33
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+34
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+35
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+36
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+37
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+38
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+39
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+40
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+41
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+42
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+43
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+44
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+45
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+46
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+47
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+48
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+49
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+50
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+51
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+52
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+53
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+54
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+55
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+56
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+57
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+58
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+59
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+60
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+61
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+62
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KEYRING_SLOT_1+63
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+25
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+26
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+27
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+28
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+29
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+30
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+31
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+32
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+33
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+34
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+35
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+36
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+37
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+38
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+39
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+40
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+41
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+42
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+43
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+44
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+45
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+46
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+47
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+48
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+49
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+50
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+51
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+52
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+53
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+54
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+55
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+56
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+57
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+58
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+59
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+60
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+61
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+62
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+63
    UF_FLAG_PRIVATE,                                        // PLAYER_FARSIGHT
    UF_FLAG_PRIVATE,                                        // PLAYER_FARSIGHT+1
    UF_FLAG_PRIVATE,                                        // PLAYER__FIELD_KNOWN_TITLES
    UF_FLAG_PRIVATE,                                        // PLAYER__FIELD_KNOWN_TITLES+1
    UF_FLAG_PRIVATE,                                        // PLAYER__FIELD_KNOWN_TITLES1
    UF_FLAG_PRIVATE,                                        // PLAYER__FIELD_KNOWN_TITLES1+1
    UF_FLAG_PRIVATE,                                        // PLAYER__FIELD_KNOWN_TITLES2
    UF_FLAG_PRIVATE,                                        // PLAYER__FIELD_KNOWN_TITLES2+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KNOWN_CURRENCIES
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KNOWN_CURRENCIES+1
    UF_FLAG_PRIVATE,                                        // PLAYER_XP
    UF_FLAG_PRIVATE,                                        // PLAYER_NEXT_LEVEL_XP
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+25
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+26
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+27
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+28
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+29
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+30
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+31
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+32
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+33
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+34
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+35
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+36
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+37
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+38
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+39
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+40
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+41
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+42
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+43
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+44
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+45
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+46
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+47
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+48
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+49
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+50
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+51
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+52
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+53
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+54
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+55
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+56
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+57
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+58
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+59
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+60
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+61
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+62
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+63
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+64
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+65
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+66
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+67
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+68
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+69
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+70
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+71
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+72
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+73
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+74
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+75
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+76
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+77
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+78
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+79
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+80
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+81
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+82
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+83
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+84
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+85
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+86
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+87
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+88
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+89
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+90
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+91
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+92
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+93
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+94
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+95
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+96
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+97
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+98
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+99
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+100
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+101
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+102
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+103
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+104
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+105
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+106
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+107
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+108
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+109
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+110
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+111
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+112
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+113
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+114
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+115
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+116
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+117
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+118
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+119
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+120
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+121
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+122
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+123
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+124
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+125
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+126
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+127
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+128
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+129
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+130
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+131
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+132
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+133
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+134
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+135
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+136
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+137
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+138
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+139
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+140
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+141
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+142
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+143
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+144
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+145
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+146
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+147
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+148
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+149
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+150
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+151
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+152
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+153
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+154
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+155
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+156
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+157
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+158
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+159
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+160
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+161
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+162
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+163
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+164
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+165
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+166
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+167
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+168
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+169
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+170
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+171
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+172
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+173
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+174
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+175
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+176
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+177
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+178
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+179
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+180
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+181
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+182
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+183
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+184
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+185
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+186
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+187
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+188
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+189
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+190
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+191
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+192
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+193
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+194
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+195
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+196
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+197
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+198
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+199
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+200
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+201
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+202
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+203
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+204
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+205
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+206
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+207
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+208
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+209
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+210
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+211
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+212
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+213
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+214
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+215
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+216
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+217
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+218
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+219
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+220
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+221
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+222
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+223
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+224
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+225
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+226
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+227
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+228
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+229
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+230
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+231
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+232
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+233
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+234
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+235
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+236
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+237
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+238
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+239
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+240
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+241
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+242
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+243
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+244
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+245
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+246
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+247
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+248
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+249
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+250
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+251
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+252
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+253
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+254
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+255
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+256
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+257
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+258
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+259
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+260
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+261
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+262
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+263
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+264
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+265
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+266
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+267
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+268
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+269
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+270
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+271
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+272
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+273
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+274
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+275
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+276
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+277
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+278
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+279
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+280
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+281
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+282
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+283
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+284
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+285
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+286
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+287
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+288
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+289
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+290
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+291
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+292
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+293
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+294
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+295
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+296
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+297
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+298
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+299
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+300
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+301
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+302
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+303
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+304
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+305
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+306
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+307
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+308
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+309
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+310
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+311
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+312
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+313
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+314
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+315
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+316
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+317
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+318
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+319
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+320
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+321
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+322
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+323
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+324
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+325
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+326
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+327
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+328
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+329
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+330
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+331
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+332
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+333
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+334
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+335
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+336
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+337
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+338
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+339
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+340
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+341
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+342
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+343
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+344
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+345
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+346
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+347
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+348
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+349
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+350
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+351
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+352
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+353
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+354
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+355
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+356
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+357
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+358
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+359
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+360
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+361
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+362
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+363
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+364
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+365
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+366
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+367
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+368
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+369
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+370
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+371
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+372
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+373
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+374
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+375
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+376
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+377
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+378
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+379
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+380
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+381
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+382
    UF_FLAG_PRIVATE,                                        // PLAYER_SKILL_INFO_1_1+383
    UF_FLAG_PRIVATE,                                        // PLAYER_CHARACTER_POINTS1
    UF_FLAG_PRIVATE,                                        // PLAYER_CHARACTER_POINTS2
    UF_FLAG_PRIVATE,                                        // PLAYER_TRACK_CREATURES
    UF_FLAG_PRIVATE,                                        // PLAYER_TRACK_RESOURCES
    UF_FLAG_PRIVATE,                                        // PLAYER_BLOCK_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_DODGE_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_PARRY_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPERTISE
    UF_FLAG_PRIVATE,                                        // PLAYER_OFFHAND_EXPERTISE
    UF_FLAG_PRIVATE,                                        // PLAYER_CRIT_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_RANGED_CRIT_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_OFFHAND_CRIT_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_SPELL_CRIT_PERCENTAGE1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_SHIELD_BLOCK
    UF_FLAG_PRIVATE,                                        // PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+25
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+26
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+27
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+28
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+29
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+30
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+31
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+32
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+33
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+34
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+35
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+36
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+37
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+38
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+39
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+40
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+41
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+42
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+43
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+44
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+45
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+46
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+47
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+48
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+49
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+50
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+51
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+52
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+53
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+54
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+55
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+56
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+57
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+58
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+59
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+60
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+61
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+62
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+63
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+64
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+65
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+66
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+67
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+68
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+69
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+70
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+71
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+72
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+73
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+74
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+75
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+76
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+77
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+78
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+79
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+80
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+81
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+82
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+83
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+84
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+85
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+86
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+87
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+88
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+89
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+90
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+91
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+92
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+93
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+94
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+95
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+96
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+97
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+98
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+99
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+100
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+101
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+102
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+103
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+104
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+105
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+106
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+107
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+108
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+109
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+110
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+111
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+112
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+113
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+114
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+115
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+116
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+117
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+118
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+119
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+120
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+121
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+122
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+123
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+124
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+125
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+126
    UF_FLAG_PRIVATE,                                        // PLAYER_EXPLORED_ZONES_1+127
    UF_FLAG_PRIVATE,                                        // PLAYER_REST_STATE_EXPERIENCE
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COINAGE
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_HEALING_DONE_POS
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_HEALING_PCT
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_HEALING_DONE_PCT
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_TARGET_RESISTANCE
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BYTES
    UF_FLAG_PRIVATE,                                        // PLAYER_AMMO_ID
    UF_FLAG_PRIVATE,                                        // PLAYER_SELF_RES_SPELL
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_PVP_MEDALS
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_PRICE_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_KILLS
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_TODAY_CONTRIBUTION
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_YESTERDAY_CONTRIBUTION
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_LIFETIME_HONORBALE_KILLS
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_BYTES2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_WATCHED_FACTION_INDEX
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_COMBAT_RATING_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_HONOR_CURRENCY
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_ARENA_CURRENCY
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_MAX_LEVEL
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+6
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+7
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+8
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+9
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+10
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+11
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+12
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+13
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+14
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+15
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+16
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+17
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+18
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+19
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+20
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+21
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+22
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+23
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_DAILY_QUESTS_1+24
    UF_FLAG_PRIVATE,                                        // PLAYER_RUNE_REGEN_1
    UF_FLAG_PRIVATE,                                        // PLAYER_RUNE_REGEN_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_RUNE_REGEN_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_RUNE_REGEN_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_NO_REAGENT_COST_1
    UF_FLAG_PRIVATE,                                        // PLAYER_NO_REAGENT_COST_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_NO_REAGENT_COST_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPH_SLOTS_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPH_SLOTS_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPH_SLOTS_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPH_SLOTS_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPH_SLOTS_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPH_SLOTS_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPHS_1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPHS_1+1
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPHS_1+2
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPHS_1+3
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPHS_1+4
    UF_FLAG_PRIVATE,                                        // PLAYER_FIELD_GLYPHS_1+5
    UF_FLAG_PRIVATE,                                        // PLAYER_GLYPHS_ENABLED
    UF_FLAG_PRIVATE,                                        // PLAYER_PET_SPELL_POWER
};


#pragma pack(push, 1)
struct sNovusChallenge
{
    u8 command;
    u8 K[32];
};

struct NovusHeader
{
    u8     command;
    u32    account;
    u16    opcode;
    u16    size;

    void Read(Common::ByteBuffer& buffer)
    {
        buffer.Read<u8>(command);
        buffer.Read<u32>(account);
        buffer.Read<u16>(opcode);
        buffer.Read<u16>(size);
    }

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Append((u8*)this, sizeof(NovusHeader));
    }
};

#pragma pack(pop)

struct NovusMessageHandler;
class NovusConnection : Common::BaseSocket
{
public:
    static std::unordered_map<u8, NovusMessageHandler> InitMessageHandlers();

    NovusConnection(asio::ip::tcp::socket* socket, std::string address, u16 port, u8 realmId) : Common::BaseSocket(socket), _status(NOVUSSTATUS_CHALLENGE), _crypto(), _address(address), _port(port), _realmId(realmId), _headerBuffer(), _packetBuffer()
    { 
        _crypto = new StreamCrypto();
        _key = new BigNumber();

        _headerBuffer.Resize(sizeof(NovusHeader));
    }

    bool Start() override;
    void HandleRead() override;

    bool HandleCommandChallenge();
    bool HandleCommandProof();
    bool HandleCommandForwardPacket();

    void SendPacket(Common::ByteBuffer& packet);

    NovusStatus _status;
private:
    std::string _address;
    u16 _port;
    u8 _realmId;

    StreamCrypto* _crypto;
    BigNumber* _key;

    Common::ByteBuffer _headerBuffer;
    Common::ByteBuffer _packetBuffer;
};

#pragma pack(push, 1)
struct NovusMessageHandler
{
    NovusStatus status;
    size_t packetSize;
    bool (NovusConnection::*handler)();
};
#pragma pack(pop)