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

enum EntityTypeID
{
    TYPEID_OBJECT = 0,
    TYPEID_ITEM = 1,
    TYPEID_CONTAINER = 2,
    TYPEID_UNIT = 3,
    TYPEID_PLAYER = 4,
    TYPEID_GAMEOBJECT = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE = 7
};
enum EObjectFields
{
    OBJECT_FIELD_GUID = 0x0000,    // Size: 2, Type: LONG, Flags: PUBLIC
    OBJECT_FIELD_TYPE = 0x0002,    // Size: 1, Type: INT, Flags: PUBLIC
    OBJECT_FIELD_ENTRY = 0x0003,   // Size: 1, Type: INT, Flags: PUBLIC
    OBJECT_FIELD_SCALE_X = 0x0004, // Size: 1, Type: FLOAT, Flags: PUBLIC
    OBJECT_FIELD_PADDING = 0x0005, // Size: 1, Type: INT, Flags: NONE
    OBJECT_END = 0x0006
};
enum EItemFields
{
    ITEM_FIELD_OWNER = OBJECT_END + 0x0000,                // Size: 2, Type: LONG, Flags: PUBLIC
    ITEM_FIELD_CONTAINED = OBJECT_END + 0x0002,            // Size: 2, Type: LONG, Flags: PUBLIC
    ITEM_FIELD_CREATOR = OBJECT_END + 0x0004,              // Size: 2, Type: LONG, Flags: PUBLIC
    ITEM_FIELD_GIFTCREATOR = OBJECT_END + 0x0006,          // Size: 2, Type: LONG, Flags: PUBLIC
    ITEM_FIELD_STACK_COUNT = OBJECT_END + 0x0008,          // Size: 1, Type: INT, Flags: OWNER, ITEM_OWNER
    ITEM_FIELD_DURATION = OBJECT_END + 0x0009,             // Size: 1, Type: INT, Flags: OWNER, ITEM_OWNER
    ITEM_FIELD_SPELL_CHARGES = OBJECT_END + 0x000A,        // Size: 5, Type: INT, Flags: OWNER, ITEM_OWNER
    ITEM_FIELD_FLAGS = OBJECT_END + 0x000F,                // Size: 1, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_1_1 = OBJECT_END + 0x0010,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_1_3 = OBJECT_END + 0x0012,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_2_1 = OBJECT_END + 0x0013,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_2_3 = OBJECT_END + 0x0015,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_3_1 = OBJECT_END + 0x0016,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_3_3 = OBJECT_END + 0x0018,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_4_1 = OBJECT_END + 0x0019,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_4_3 = OBJECT_END + 0x001B,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_5_1 = OBJECT_END + 0x001C,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_5_3 = OBJECT_END + 0x001E,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_6_1 = OBJECT_END + 0x001F,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_6_3 = OBJECT_END + 0x0021,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_7_1 = OBJECT_END + 0x0022,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_7_3 = OBJECT_END + 0x0024,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_8_1 = OBJECT_END + 0x0025,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_8_3 = OBJECT_END + 0x0027,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_9_1 = OBJECT_END + 0x0028,      // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_9_3 = OBJECT_END + 0x002A,      // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_10_1 = OBJECT_END + 0x002B,     // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_10_3 = OBJECT_END + 0x002D,     // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_11_1 = OBJECT_END + 0x002E,     // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_11_3 = OBJECT_END + 0x0030,     // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_12_1 = OBJECT_END + 0x0031,     // Size: 2, Type: INT, Flags: PUBLIC
    ITEM_FIELD_ENCHANTMENT_12_3 = OBJECT_END + 0x0033,     // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    ITEM_FIELD_PROPERTY_SEED = OBJECT_END + 0x0034,        // Size: 1, Type: INT, Flags: PUBLIC
    ITEM_FIELD_RANDOM_PROPERTIES_ID = OBJECT_END + 0x0035, // Size: 1, Type: INT, Flags: PUBLIC
    ITEM_FIELD_DURABILITY = OBJECT_END + 0x0036,           // Size: 1, Type: INT, Flags: OWNER, ITEM_OWNER
    ITEM_FIELD_MAXDURABILITY = OBJECT_END + 0x0037,        // Size: 1, Type: INT, Flags: OWNER, ITEM_OWNER
    ITEM_FIELD_CREATE_PLAYED_TIME = OBJECT_END + 0x0038,   // Size: 1, Type: INT, Flags: PUBLIC
    ITEM_FIELD_PAD = OBJECT_END + 0x0039,                  // Size: 1, Type: INT, Flags: NONE
    ITEM_END = OBJECT_END + 0x003A
};
enum EContainerFields
{
    CONTAINER_FIELD_NUM_SLOTS = ITEM_END + 0x0000, // Size: 1, Type: INT, Flags: PUBLIC
    CONTAINER_ALIGN_PAD = ITEM_END + 0x0001,       // Size: 1, Type: BYTES, Flags: NONE
    CONTAINER_FIELD_SLOT_1 = ITEM_END + 0x0002,    // Size: 72, Type: LONG, Flags: PUBLIC
    CONTAINER_END = ITEM_END + 0x004A
};
enum EUnitFields
{
    UNIT_FIELD_CHARM = OBJECT_END + 0x0000,                                 // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_SUMMON = OBJECT_END + 0x0002,                                // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_CRITTER = OBJECT_END + 0x0004,                               // Size: 2, Type: LONG, Flags: PRIVATE
    UNIT_FIELD_CHARMEDBY = OBJECT_END + 0x0006,                             // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_SUMMONEDBY = OBJECT_END + 0x0008,                            // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_CREATEDBY = OBJECT_END + 0x000A,                             // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_TARGET = OBJECT_END + 0x000C,                                // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_FIELD_CHANNEL_OBJECT = OBJECT_END + 0x000E,                        // Size: 2, Type: LONG, Flags: PUBLIC
    UNIT_CHANNEL_SPELL = OBJECT_END + 0x0010,                               // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_BYTES_0 = OBJECT_END + 0x0011,                               // Size: 1, Type: BYTES, Flags: PUBLIC
    UNIT_FIELD_HEALTH = OBJECT_END + 0x0012,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER1 = OBJECT_END + 0x0013,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER2 = OBJECT_END + 0x0014,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER3 = OBJECT_END + 0x0015,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER4 = OBJECT_END + 0x0016,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER5 = OBJECT_END + 0x0017,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER6 = OBJECT_END + 0x0018,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER7 = OBJECT_END + 0x0019,                                // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXHEALTH = OBJECT_END + 0x001A,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER1 = OBJECT_END + 0x001B,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER2 = OBJECT_END + 0x001C,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER3 = OBJECT_END + 0x001D,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER4 = OBJECT_END + 0x001E,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER5 = OBJECT_END + 0x001F,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER6 = OBJECT_END + 0x0020,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MAXPOWER7 = OBJECT_END + 0x0021,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER = OBJECT_END + 0x0022,             // Size: 7, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER = OBJECT_END + 0x0029, // Size: 7, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_LEVEL = OBJECT_END + 0x0030,                                 // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_FACTIONTEMPLATE = OBJECT_END + 0x0031,                       // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_VIRTUAL_ITEM_SLOT_ID = OBJECT_END + 0x0032,                        // Size: 3, Type: INT, Flags: PUBLIC
    UNIT_FIELD_FLAGS = OBJECT_END + 0x0035,                                 // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_FLAGS_2 = OBJECT_END + 0x0036,                               // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_AURASTATE = OBJECT_END + 0x0037,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_BASEATTACKTIME = OBJECT_END + 0x0038,                        // Size: 2, Type: INT, Flags: PUBLIC
    UNIT_FIELD_RANGEDATTACKTIME = OBJECT_END + 0x003A,                      // Size: 1, Type: INT, Flags: PRIVATE
    UNIT_FIELD_BOUNDINGRADIUS = OBJECT_END + 0x003B,                        // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_FIELD_COMBATREACH = OBJECT_END + 0x003C,                           // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_FIELD_DISPLAYID = OBJECT_END + 0x003D,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_NATIVEDISPLAYID = OBJECT_END + 0x003E,                       // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MOUNTDISPLAYID = OBJECT_END + 0x003F,                        // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_MINDAMAGE = OBJECT_END + 0x0040,                             // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_MAXDAMAGE = OBJECT_END + 0x0041,                             // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_MINOFFHANDDAMAGE = OBJECT_END + 0x0042,                      // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_MAXOFFHANDDAMAGE = OBJECT_END + 0x0043,                      // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_BYTES_1 = OBJECT_END + 0x0044,                               // Size: 1, Type: BYTES, Flags: PUBLIC
    UNIT_FIELD_PETNUMBER = OBJECT_END + 0x0045,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_PET_NAME_TIMESTAMP = OBJECT_END + 0x0046,                    // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_PETEXPERIENCE = OBJECT_END + 0x0047,                         // Size: 1, Type: INT, Flags: OWNER
    UNIT_FIELD_PETNEXTLEVELEXP = OBJECT_END + 0x0048,                       // Size: 1, Type: INT, Flags: OWNER
    UNIT_DYNAMIC_FLAGS = OBJECT_END + 0x0049,                               // Size: 1, Type: INT, Flags: DYNAMIC
    UNIT_MOD_CAST_SPEED = OBJECT_END + 0x004A,                              // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_CREATED_BY_SPELL = OBJECT_END + 0x004B,                            // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_NPC_FLAGS = OBJECT_END + 0x004C,                                   // Size: 1, Type: INT, Flags: DYNAMIC
    UNIT_NPC_EMOTESTATE = OBJECT_END + 0x004D,                              // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_STAT0 = OBJECT_END + 0x004E,                                 // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT1 = OBJECT_END + 0x004F,                                 // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT2 = OBJECT_END + 0x0050,                                 // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT3 = OBJECT_END + 0x0051,                                 // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_STAT4 = OBJECT_END + 0x0052,                                 // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT0 = OBJECT_END + 0x0053,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT1 = OBJECT_END + 0x0054,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT2 = OBJECT_END + 0x0055,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT3 = OBJECT_END + 0x0056,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POSSTAT4 = OBJECT_END + 0x0057,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT0 = OBJECT_END + 0x0058,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT1 = OBJECT_END + 0x0059,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT2 = OBJECT_END + 0x005A,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT3 = OBJECT_END + 0x005B,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_NEGSTAT4 = OBJECT_END + 0x005C,                              // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RESISTANCES = OBJECT_END + 0x005D,                           // Size: 7, Type: INT, Flags: PRIVATE, OWNER, PARTY_LEADER
    UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE = OBJECT_END + 0x0064,            // Size: 7, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE = OBJECT_END + 0x006B,            // Size: 7, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_BASE_MANA = OBJECT_END + 0x0072,                             // Size: 1, Type: INT, Flags: PUBLIC
    UNIT_FIELD_BASE_HEALTH = OBJECT_END + 0x0073,                           // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_BYTES_2 = OBJECT_END + 0x0074,                               // Size: 1, Type: BYTES, Flags: PUBLIC
    UNIT_FIELD_ATTACK_POWER = OBJECT_END + 0x0075,                          // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_ATTACK_POWER_MODS = OBJECT_END + 0x0076,                     // Size: 1, Type: TWO_SHORT, Flags: PRIVATE, OWNER
    UNIT_FIELD_ATTACK_POWER_MULTIPLIER = OBJECT_END + 0x0077,               // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER = OBJECT_END + 0x0078,                   // Size: 1, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MODS = OBJECT_END + 0x0079,              // Size: 1, Type: TWO_SHORT, Flags: PRIVATE, OWNER
    UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = OBJECT_END + 0x007A,        // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_MINRANGEDDAMAGE = OBJECT_END + 0x007B,                       // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_MAXRANGEDDAMAGE = OBJECT_END + 0x007C,                       // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POWER_COST_MODIFIER = OBJECT_END + 0x007D,                   // Size: 7, Type: INT, Flags: PRIVATE, OWNER
    UNIT_FIELD_POWER_COST_MULTIPLIER = OBJECT_END + 0x0084,                 // Size: 7, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_MAXHEALTHMODIFIER = OBJECT_END + 0x008B,                     // Size: 1, Type: FLOAT, Flags: PRIVATE, OWNER
    UNIT_FIELD_HOVERHEIGHT = OBJECT_END + 0x008C,                           // Size: 1, Type: FLOAT, Flags: PUBLIC
    UNIT_FIELD_PADDING = OBJECT_END + 0x008D,                               // Size: 1, Type: INT, Flags: NONE
    UNIT_END = OBJECT_END + 0x008E,

    PLAYER_DUEL_ARBITER = UNIT_END + 0x0000,                         // Size: 2, Type: LONG, Flags: PUBLIC
    PLAYER_FLAGS = UNIT_END + 0x0002,                                // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_GUILDID = UNIT_END + 0x0003,                              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_GUILDRANK = UNIT_END + 0x0004,                            // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_BYTES = UNIT_END + 0x0005,                                // Size: 1, Type: BYTES, Flags: PUBLIC
    PLAYER_BYTES_2 = UNIT_END + 0x0006,                              // Size: 1, Type: BYTES, Flags: PUBLIC
    PLAYER_BYTES_3 = UNIT_END + 0x0007,                              // Size: 1, Type: BYTES, Flags: PUBLIC
    PLAYER_DUEL_TEAM = UNIT_END + 0x0008,                            // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_GUILD_TIMESTAMP = UNIT_END + 0x0009,                      // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_QUEST_LOG_1_1 = UNIT_END + 0x000A,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_1_2 = UNIT_END + 0x000B,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_1_3 = UNIT_END + 0x000C,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_1_4 = UNIT_END + 0x000E,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_2_1 = UNIT_END + 0x000F,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_2_2 = UNIT_END + 0x0010,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_2_3 = UNIT_END + 0x0011,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_2_5 = UNIT_END + 0x0013,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_3_1 = UNIT_END + 0x0014,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_3_2 = UNIT_END + 0x0015,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_3_3 = UNIT_END + 0x0016,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_3_5 = UNIT_END + 0x0018,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_4_1 = UNIT_END + 0x0019,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_4_2 = UNIT_END + 0x001A,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_4_3 = UNIT_END + 0x001B,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_4_5 = UNIT_END + 0x001D,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_5_1 = UNIT_END + 0x001E,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_5_2 = UNIT_END + 0x001F,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_5_3 = UNIT_END + 0x0020,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_5_5 = UNIT_END + 0x0022,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_6_1 = UNIT_END + 0x0023,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_6_2 = UNIT_END + 0x0024,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_6_3 = UNIT_END + 0x0025,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_6_5 = UNIT_END + 0x0027,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_7_1 = UNIT_END + 0x0028,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_7_2 = UNIT_END + 0x0029,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_7_3 = UNIT_END + 0x002A,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_7_5 = UNIT_END + 0x002C,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_8_1 = UNIT_END + 0x002D,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_8_2 = UNIT_END + 0x002E,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_8_3 = UNIT_END + 0x002F,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_8_5 = UNIT_END + 0x0031,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_9_1 = UNIT_END + 0x0032,                        // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_9_2 = UNIT_END + 0x0033,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_9_3 = UNIT_END + 0x0034,                        // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_9_5 = UNIT_END + 0x0036,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_10_1 = UNIT_END + 0x0037,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_10_2 = UNIT_END + 0x0038,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_10_3 = UNIT_END + 0x0039,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_10_5 = UNIT_END + 0x003B,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_11_1 = UNIT_END + 0x003C,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_11_2 = UNIT_END + 0x003D,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_11_3 = UNIT_END + 0x003E,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_11_5 = UNIT_END + 0x0040,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_12_1 = UNIT_END + 0x0041,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_12_2 = UNIT_END + 0x0042,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_12_3 = UNIT_END + 0x0043,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_12_5 = UNIT_END + 0x0045,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_13_1 = UNIT_END + 0x0046,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_13_2 = UNIT_END + 0x0047,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_13_3 = UNIT_END + 0x0048,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_13_5 = UNIT_END + 0x004A,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_14_1 = UNIT_END + 0x004B,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_14_2 = UNIT_END + 0x004C,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_14_3 = UNIT_END + 0x004D,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_14_5 = UNIT_END + 0x004F,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_15_1 = UNIT_END + 0x0050,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_15_2 = UNIT_END + 0x0051,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_15_3 = UNIT_END + 0x0052,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_15_5 = UNIT_END + 0x0054,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_16_1 = UNIT_END + 0x0055,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_16_2 = UNIT_END + 0x0056,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_16_3 = UNIT_END + 0x0057,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_16_5 = UNIT_END + 0x0059,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_17_1 = UNIT_END + 0x005A,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_17_2 = UNIT_END + 0x005B,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_17_3 = UNIT_END + 0x005C,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_17_5 = UNIT_END + 0x005E,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_18_1 = UNIT_END + 0x005F,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_18_2 = UNIT_END + 0x0060,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_18_3 = UNIT_END + 0x0061,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_18_5 = UNIT_END + 0x0063,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_19_1 = UNIT_END + 0x0064,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_19_2 = UNIT_END + 0x0065,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_19_3 = UNIT_END + 0x0066,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_19_5 = UNIT_END + 0x0068,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_20_1 = UNIT_END + 0x0069,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_20_2 = UNIT_END + 0x006A,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_20_3 = UNIT_END + 0x006B,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_20_5 = UNIT_END + 0x006D,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_21_1 = UNIT_END + 0x006E,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_21_2 = UNIT_END + 0x006F,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_21_3 = UNIT_END + 0x0070,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_21_5 = UNIT_END + 0x0072,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_22_1 = UNIT_END + 0x0073,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_22_2 = UNIT_END + 0x0074,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_22_3 = UNIT_END + 0x0075,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_22_5 = UNIT_END + 0x0077,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_23_1 = UNIT_END + 0x0078,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_23_2 = UNIT_END + 0x0079,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_23_3 = UNIT_END + 0x007A,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_23_5 = UNIT_END + 0x007C,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_24_1 = UNIT_END + 0x007D,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_24_2 = UNIT_END + 0x007E,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_24_3 = UNIT_END + 0x007F,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_24_5 = UNIT_END + 0x0081,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_25_1 = UNIT_END + 0x0082,                       // Size: 1, Type: INT, Flags: PARTY_MEMBER
    PLAYER_QUEST_LOG_25_2 = UNIT_END + 0x0083,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_QUEST_LOG_25_3 = UNIT_END + 0x0084,                       // Size: 2, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_QUEST_LOG_25_5 = UNIT_END + 0x0086,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_VISIBLE_ITEM_1_ENTRYID = UNIT_END + 0x0087,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_1_ENCHANTMENT = UNIT_END + 0x0088,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_2_ENTRYID = UNIT_END + 0x0089,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_2_ENCHANTMENT = UNIT_END + 0x008A,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_3_ENTRYID = UNIT_END + 0x008B,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_3_ENCHANTMENT = UNIT_END + 0x008C,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_4_ENTRYID = UNIT_END + 0x008D,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_4_ENCHANTMENT = UNIT_END + 0x008E,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_5_ENTRYID = UNIT_END + 0x008F,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_5_ENCHANTMENT = UNIT_END + 0x0090,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_6_ENTRYID = UNIT_END + 0x0091,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_6_ENCHANTMENT = UNIT_END + 0x0092,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_7_ENTRYID = UNIT_END + 0x0093,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_7_ENCHANTMENT = UNIT_END + 0x0094,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_8_ENTRYID = UNIT_END + 0x0095,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_8_ENCHANTMENT = UNIT_END + 0x0096,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_9_ENTRYID = UNIT_END + 0x0097,               // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_9_ENCHANTMENT = UNIT_END + 0x0098,           // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_10_ENTRYID = UNIT_END + 0x0099,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_10_ENCHANTMENT = UNIT_END + 0x009A,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_11_ENTRYID = UNIT_END + 0x009B,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_11_ENCHANTMENT = UNIT_END + 0x009C,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_12_ENTRYID = UNIT_END + 0x009D,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_12_ENCHANTMENT = UNIT_END + 0x009E,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_13_ENTRYID = UNIT_END + 0x009F,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_13_ENCHANTMENT = UNIT_END + 0x00A0,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_14_ENTRYID = UNIT_END + 0x00A1,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_14_ENCHANTMENT = UNIT_END + 0x00A2,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_15_ENTRYID = UNIT_END + 0x00A3,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_15_ENCHANTMENT = UNIT_END + 0x00A4,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_16_ENTRYID = UNIT_END + 0x00A5,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_16_ENCHANTMENT = UNIT_END + 0x00A6,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_17_ENTRYID = UNIT_END + 0x00A7,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_17_ENCHANTMENT = UNIT_END + 0x00A8,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_18_ENTRYID = UNIT_END + 0x00A9,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_18_ENCHANTMENT = UNIT_END + 0x00AA,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_19_ENTRYID = UNIT_END + 0x00AB,              // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_VISIBLE_ITEM_19_ENCHANTMENT = UNIT_END + 0x00AC,          // Size: 1, Type: TWO_SHORT, Flags: PUBLIC
    PLAYER_CHOSEN_TITLE = UNIT_END + 0x00AD,                         // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_FAKE_INEBRIATION = UNIT_END + 0x00AE,                     // Size: 1, Type: INT, Flags: PUBLIC
    PLAYER_FIELD_PAD_0 = UNIT_END + 0x00AF,                          // Size: 1, Type: INT, Flags: NONE
    PLAYER_FIELD_INV_SLOT_HEAD = UNIT_END + 0x00B0,                  // Size: 46, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_PACK_SLOT_1 = UNIT_END + 0x00DE,                    // Size: 32, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_BANK_SLOT_1 = UNIT_END + 0x00FE,                    // Size: 56, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_BANKBAG_SLOT_1 = UNIT_END + 0x0136,                 // Size: 14, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_VENDORBUYBACK_SLOT_1 = UNIT_END + 0x0144,           // Size: 24, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_KEYRING_SLOT_1 = UNIT_END + 0x015C,                 // Size: 64, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_CURRENCYTOKEN_SLOT_1 = UNIT_END + 0x019C,           // Size: 64, Type: LONG, Flags: PRIVATE
    PLAYER_FARSIGHT = UNIT_END + 0x01DC,                             // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER__FIELD_KNOWN_TITLES = UNIT_END + 0x01DE,                  // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER__FIELD_KNOWN_TITLES1 = UNIT_END + 0x01E0,                 // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER__FIELD_KNOWN_TITLES2 = UNIT_END + 0x01E2,                 // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER_FIELD_KNOWN_CURRENCIES = UNIT_END + 0x01E4,               // Size: 2, Type: LONG, Flags: PRIVATE
    PLAYER_XP = UNIT_END + 0x01E6,                                   // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_NEXT_LEVEL_XP = UNIT_END + 0x01E7,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_SKILL_INFO_1_1 = UNIT_END + 0x01E8,                       // Size: 384, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_CHARACTER_POINTS1 = UNIT_END + 0x0368,                    // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_CHARACTER_POINTS2 = UNIT_END + 0x0369,                    // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_TRACK_CREATURES = UNIT_END + 0x036A,                      // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_TRACK_RESOURCES = UNIT_END + 0x036B,                      // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_BLOCK_PERCENTAGE = UNIT_END + 0x036C,                     // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_DODGE_PERCENTAGE = UNIT_END + 0x036D,                     // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_PARRY_PERCENTAGE = UNIT_END + 0x036E,                     // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_EXPERTISE = UNIT_END + 0x036F,                            // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_OFFHAND_EXPERTISE = UNIT_END + 0x0370,                    // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_CRIT_PERCENTAGE = UNIT_END + 0x0371,                      // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_RANGED_CRIT_PERCENTAGE = UNIT_END + 0x0372,               // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_OFFHAND_CRIT_PERCENTAGE = UNIT_END + 0x0373,              // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_SPELL_CRIT_PERCENTAGE1 = UNIT_END + 0x0374,               // Size: 7, Type: FLOAT, Flags: PRIVATE
    PLAYER_SHIELD_BLOCK = UNIT_END + 0x037B,                         // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE = UNIT_END + 0x037C,         // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_EXPLORED_ZONES_1 = UNIT_END + 0x037D,                     // Size: 128, Type: BYTES, Flags: PRIVATE
    PLAYER_REST_STATE_EXPERIENCE = UNIT_END + 0x03FD,                // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_COINAGE = UNIT_END + 0x03FE,                        // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_POS = UNIT_END + 0x03FF,            // Size: 7, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_NEG = UNIT_END + 0x0406,            // Size: 7, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_DAMAGE_DONE_PCT = UNIT_END + 0x040D,            // Size: 7, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_HEALING_DONE_POS = UNIT_END + 0x0414,           // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_HEALING_PCT = UNIT_END + 0x0415,                // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_FIELD_MOD_HEALING_DONE_PCT = UNIT_END + 0x0416,           // Size: 1, Type: FLOAT, Flags: PRIVATE
    PLAYER_FIELD_MOD_TARGET_RESISTANCE = UNIT_END + 0x0417,          // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE = UNIT_END + 0x0418, // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BYTES = UNIT_END + 0x0419,                          // Size: 1, Type: BYTES, Flags: PRIVATE
    PLAYER_AMMO_ID = UNIT_END + 0x041A,                              // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_SELF_RES_SPELL = UNIT_END + 0x041B,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_PVP_MEDALS = UNIT_END + 0x041C,                     // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BUYBACK_PRICE_1 = UNIT_END + 0x041D,                // Size: 12, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BUYBACK_TIMESTAMP_1 = UNIT_END + 0x0429,            // Size: 12, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_KILLS = UNIT_END + 0x0435,                          // Size: 1, Type: TWO_SHORT, Flags: PRIVATE
    PLAYER_FIELD_TODAY_CONTRIBUTION = UNIT_END + 0x0436,             // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_YESTERDAY_CONTRIBUTION = UNIT_END + 0x0437,         // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_LIFETIME_HONORABLE_KILLS = UNIT_END + 0x0438,       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_BYTES2 = UNIT_END + 0x0439,                         // Size: 1, Type: 6, Flags: PRIVATE
    PLAYER_FIELD_WATCHED_FACTION_INDEX = UNIT_END + 0x043A,          // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_COMBAT_RATING_1 = UNIT_END + 0x043B,                // Size: 25, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_ARENA_TEAM_INFO_1_1 = UNIT_END + 0x0454,            // Size: 21, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_HONOR_CURRENCY = UNIT_END + 0x0469,                 // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_ARENA_CURRENCY = UNIT_END + 0x046A,                 // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_MAX_LEVEL = UNIT_END + 0x046B,                      // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_DAILY_QUESTS_1 = UNIT_END + 0x046C,                 // Size: 25, Type: INT, Flags: PRIVATE
    PLAYER_RUNE_REGEN_1 = UNIT_END + 0x0485,                         // Size: 4, Type: FLOAT, Flags: PRIVATE
    PLAYER_NO_REAGENT_COST_1 = UNIT_END + 0x0489,                    // Size: 3, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_GLYPH_SLOTS_1 = UNIT_END + 0x048C,                  // Size: 6, Type: INT, Flags: PRIVATE
    PLAYER_FIELD_GLYPHS_1 = UNIT_END + 0x0492,                       // Size: 6, Type: INT, Flags: PRIVATE
    PLAYER_GLYPHS_ENABLED = UNIT_END + 0x0498,                       // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_PET_SPELL_POWER = UNIT_END + 0x0499,                      // Size: 1, Type: INT, Flags: PRIVATE
    PLAYER_END = UNIT_END + 0x049A
};
enum UpdatefieldFlags
{
    UPDATEFIELD_FLAG_NONE = 0x000,
    UPDATEFIELD_FLAG_PUBLIC = 0x001,
    UPDATEFIELD_FLAG_SELF = 0x002,
    UPDATEFIELD_FLAG_OWNER = 0x004,
    UPDATEFIELD_FLAG_ITEM_OWNER = 0x010,
    UPDATEFIELD_FLAG_SPECIAL_INFO = 0x020,
    UPDATEFIELD_FLAG_PARTY = 0x040,
    UPDATEFIELD_FLAG_ALL = 0x100
};
static u32 ItemUpdateFieldFlags[CONTAINER_END] =
    {
        UPDATEFIELD_FLAG_PUBLIC,                              // OBJECT_FIELD_GUID
        UPDATEFIELD_FLAG_PUBLIC,                              // OBJECT_FIELD_GUID+1
        UPDATEFIELD_FLAG_PUBLIC,                              // OBJECT_FIELD_TYPE
        UPDATEFIELD_FLAG_PUBLIC,                              // OBJECT_FIELD_ENTRY
        UPDATEFIELD_FLAG_PUBLIC,                              // OBJECT_FIELD_SCALE_X
        UPDATEFIELD_FLAG_NONE,                                // OBJECT_FIELD_PADDING
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_OWNER
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_OWNER+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_CONTAINED
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_CONTAINED+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_CREATOR
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_CREATOR+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_GIFTCREATOR
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_GIFTCREATOR+1
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_STACK_COUNT
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_DURATION
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_SPELL_CHARGES
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_SPELL_CHARGES+1
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_SPELL_CHARGES+2
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_SPELL_CHARGES+3
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_SPELL_CHARGES+4
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_FLAGS
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_1_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_1_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_1_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_2_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_2_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_2_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_3_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_3_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_3_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_4_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_4_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_4_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_5_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_5_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_5_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_6_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_6_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_6_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_7_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_7_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_7_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_8_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_8_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_8_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_9_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_9_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_9_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_10_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_10_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_10_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_11_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_11_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_11_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_12_1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_12_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_ENCHANTMENT_12_3
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_PROPERTY_SEED
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_RANDOM_PROPERTIES_ID
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_DURABILITY
        UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_ITEM_OWNER, // ITEM_FIELD_MAXDURABILITY
        UPDATEFIELD_FLAG_PUBLIC,                              // ITEM_FIELD_CREATE_PLAYED_TIME
        UPDATEFIELD_FLAG_NONE,                                // ITEM_FIELD_PAD
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_NUM_SLOTS
        UPDATEFIELD_FLAG_NONE,                                // CONTAINER_ALIGN_PAD
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+1
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+2
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+3
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+4
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+5
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+6
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+7
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+8
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+9
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+10
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+11
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+12
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+13
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+14
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+15
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+16
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+17
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+18
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+19
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+20
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+21
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+22
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+23
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+24
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+25
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+26
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+27
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+28
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+29
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+30
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+31
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+32
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+33
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+34
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+35
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+36
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+37
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+38
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+39
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+40
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+41
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+42
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+43
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+44
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+45
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+46
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+47
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+48
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+49
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+50
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+51
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+52
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+53
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+54
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+55
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+56
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+57
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+58
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+59
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+60
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+61
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+62
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+63
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+64
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+65
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+66
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+67
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+68
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+69
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+70
        UPDATEFIELD_FLAG_PUBLIC,                              // CONTAINER_FIELD_SLOT_1+71
};
static u32 UnitUpdateFieldFlags[PLAYER_END] =
    {
        UPDATEFIELD_FLAG_PUBLIC,                                                        // OBJECT_FIELD_GUID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // OBJECT_FIELD_GUID+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // OBJECT_FIELD_TYPE
        UPDATEFIELD_FLAG_PUBLIC,                                                        // OBJECT_FIELD_ENTRY
        UPDATEFIELD_FLAG_PUBLIC,                                                        // OBJECT_FIELD_SCALE_X
        UPDATEFIELD_FLAG_NONE,                                                          // OBJECT_FIELD_PADDING
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CHARM
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CHARM+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_SUMMON
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_SUMMON+1
        UPDATEFIELD_FLAG_SELF,                                                          // UNIT_FIELD_CRITTER
        UPDATEFIELD_FLAG_SELF,                                                          // UNIT_FIELD_CRITTER+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CHARMEDBY
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CHARMEDBY+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_SUMMONEDBY
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_SUMMONEDBY+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CREATEDBY
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CREATEDBY+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_TARGET
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_TARGET+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CHANNEL_OBJECT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_CHANNEL_OBJECT+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_CHANNEL_SPELL
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BYTES_0
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_HEALTH
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER2
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER3
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER4
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER5
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER6
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_POWER7
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXHEALTH
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER2
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER3
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER4
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER5
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER6
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MAXPOWER7
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER+6
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER+6
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_LEVEL
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_FACTIONTEMPLATE
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_VIRTUAL_ITEM_SLOT_ID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_VIRTUAL_ITEM_SLOT_ID+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_VIRTUAL_ITEM_SLOT_ID+2
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_FLAGS
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_FLAGS_2
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_AURASTATE
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BASEATTACKTIME
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BASEATTACKTIME+1
        UPDATEFIELD_FLAG_SELF,                                                          // UNIT_FIELD_RANGEDATTACKTIME
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BOUNDINGRADIUS
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_COMBATREACH
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_DISPLAYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_NATIVEDISPLAYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_MOUNTDISPLAYID
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_MINDAMAGE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_MAXDAMAGE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_MINOFFHANDDAMAGE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_MAXOFFHANDDAMAGE
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BYTES_1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_PETNUMBER
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_PET_NAME_TIMESTAMP
        UPDATEFIELD_FLAG_OWNER,                                                         // UNIT_FIELD_PETEXPERIENCE
        UPDATEFIELD_FLAG_OWNER,                                                         // UNIT_FIELD_PETNEXTLEVELEXP
        UPDATEFIELD_FLAG_ALL,                                                           // UNIT_DYNAMIC_FLAGS
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_MOD_CAST_SPEED
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_CREATED_BY_SPELL
        UPDATEFIELD_FLAG_ALL,                                                           // UNIT_NPC_FLAGS
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_NPC_EMOTESTATE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_STAT0
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_STAT1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_STAT2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_STAT3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_STAT4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POSSTAT0
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POSSTAT1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POSSTAT2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POSSTAT3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POSSTAT4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_NEGSTAT0
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_NEGSTAT1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_NEGSTAT2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_NEGSTAT3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_NEGSTAT4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER | UPDATEFIELD_FLAG_SPECIAL_INFO, // UNIT_FIELD_RESISTANCES+6
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE+6
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE+6
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BASE_MANA
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_BASE_HEALTH
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_BYTES_2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_ATTACK_POWER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_ATTACK_POWER_MODS
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_ATTACK_POWER_MULTIPLIER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RANGED_ATTACK_POWER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RANGED_ATTACK_POWER_MODS
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_MINRANGEDDAMAGE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_MAXRANGEDDAMAGE
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MODIFIER+6
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER+1
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER+2
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER+3
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER+4
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER+5
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_POWER_COST_MULTIPLIER+6
        UPDATEFIELD_FLAG_SELF | UPDATEFIELD_FLAG_OWNER,                                 // UNIT_FIELD_MAXHEALTHMODIFIER
        UPDATEFIELD_FLAG_PUBLIC,                                                        // UNIT_FIELD_HOVERHEIGHT
        UPDATEFIELD_FLAG_NONE,                                                          // UNIT_FIELD_PADDING
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_DUEL_ARBITER
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_DUEL_ARBITER+1
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_FLAGS
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_GUILDID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_GUILDRANK
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_BYTES
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_BYTES_2
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_BYTES_3
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_DUEL_TEAM
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_GUILD_TIMESTAMP
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_1_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_1_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_1_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_1_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_1_4
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_2_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_2_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_2_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_2_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_2_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_3_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_3_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_3_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_3_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_3_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_4_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_4_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_4_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_4_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_4_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_5_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_5_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_5_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_5_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_5_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_6_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_6_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_6_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_6_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_6_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_7_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_7_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_7_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_7_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_7_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_8_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_8_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_8_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_8_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_8_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_9_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_9_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_9_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_9_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_9_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_10_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_10_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_10_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_10_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_10_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_11_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_11_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_11_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_11_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_11_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_12_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_12_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_12_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_12_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_12_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_13_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_13_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_13_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_13_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_13_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_14_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_14_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_14_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_14_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_14_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_15_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_15_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_15_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_15_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_15_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_16_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_16_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_16_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_16_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_16_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_17_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_17_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_17_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_17_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_17_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_18_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_18_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_18_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_18_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_18_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_19_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_19_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_19_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_19_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_19_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_20_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_20_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_20_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_20_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_20_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_21_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_21_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_21_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_21_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_21_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_22_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_22_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_22_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_22_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_22_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_23_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_23_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_23_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_23_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_23_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_24_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_24_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_24_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_24_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_24_5
        UPDATEFIELD_FLAG_PARTY,                                                         // PLAYER_QUEST_LOG_25_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_25_2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_25_3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_25_3+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_QUEST_LOG_25_5
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_1_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_1_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_2_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_2_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_3_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_3_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_4_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_4_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_5_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_5_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_6_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_6_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_7_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_7_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_8_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_8_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_9_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_9_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_10_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_10_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_11_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_11_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_12_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_12_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_13_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_13_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_14_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_14_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_15_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_15_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_16_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_16_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_17_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_17_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_18_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_18_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_19_ENTRYID
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_VISIBLE_ITEM_19_ENCHANTMENT
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_CHOSEN_TITLE
        UPDATEFIELD_FLAG_PUBLIC,                                                        // PLAYER_FAKE_INEBRIATION
        UPDATEFIELD_FLAG_NONE,                                                          // PLAYER_FIELD_PAD_0
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+32
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+33
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+34
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+35
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+36
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+37
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+38
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+39
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+40
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+41
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+42
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+43
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+44
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_INV_SLOT_HEAD+45
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PACK_SLOT_1+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+32
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+33
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+34
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+35
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+36
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+37
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+38
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+39
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+40
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+41
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+42
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+43
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+44
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+45
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+46
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+47
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+48
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+49
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+50
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+51
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+52
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+53
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+54
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANK_SLOT_1+55
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BANKBAG_SLOT_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_VENDORBUYBACK_SLOT_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+32
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+33
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+34
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+35
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+36
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+37
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+38
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+39
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+40
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+41
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+42
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+43
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+44
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+45
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+46
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+47
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+48
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+49
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+50
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+51
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+52
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+53
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+54
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+55
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+56
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+57
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+58
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+59
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+60
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+61
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+62
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KEYRING_SLOT_1+63
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+32
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+33
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+34
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+35
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+36
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+37
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+38
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+39
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+40
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+41
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+42
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+43
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+44
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+45
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+46
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+47
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+48
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+49
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+50
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+51
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+52
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+53
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+54
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+55
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+56
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+57
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+58
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+59
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+60
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+61
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+62
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_CURRENCYTOKEN_SLOT_1+63
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FARSIGHT
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FARSIGHT+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER__FIELD_KNOWN_TITLES
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER__FIELD_KNOWN_TITLES+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER__FIELD_KNOWN_TITLES1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER__FIELD_KNOWN_TITLES1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER__FIELD_KNOWN_TITLES2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER__FIELD_KNOWN_TITLES2+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KNOWN_CURRENCIES
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KNOWN_CURRENCIES+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_XP
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_NEXT_LEVEL_XP
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+32
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+33
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+34
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+35
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+36
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+37
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+38
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+39
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+40
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+41
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+42
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+43
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+44
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+45
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+46
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+47
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+48
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+49
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+50
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+51
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+52
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+53
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+54
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+55
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+56
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+57
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+58
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+59
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+60
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+61
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+62
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+63
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+64
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+65
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+66
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+67
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+68
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+69
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+70
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+71
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+72
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+73
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+74
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+75
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+76
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+77
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+78
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+79
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+80
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+81
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+82
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+83
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+84
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+85
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+86
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+87
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+88
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+89
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+90
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+91
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+92
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+93
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+94
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+95
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+96
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+97
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+98
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+99
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+100
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+101
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+102
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+103
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+104
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+105
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+106
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+107
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+108
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+109
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+110
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+111
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+112
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+113
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+114
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+115
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+116
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+117
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+118
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+119
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+120
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+121
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+122
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+123
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+124
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+125
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+126
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+127
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+128
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+129
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+130
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+131
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+132
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+133
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+134
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+135
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+136
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+137
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+138
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+139
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+140
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+141
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+142
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+143
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+144
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+145
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+146
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+147
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+148
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+149
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+150
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+151
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+152
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+153
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+154
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+155
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+156
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+157
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+158
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+159
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+160
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+161
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+162
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+163
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+164
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+165
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+166
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+167
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+168
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+169
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+170
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+171
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+172
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+173
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+174
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+175
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+176
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+177
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+178
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+179
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+180
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+181
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+182
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+183
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+184
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+185
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+186
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+187
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+188
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+189
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+190
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+191
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+192
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+193
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+194
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+195
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+196
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+197
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+198
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+199
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+200
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+201
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+202
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+203
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+204
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+205
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+206
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+207
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+208
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+209
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+210
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+211
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+212
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+213
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+214
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+215
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+216
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+217
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+218
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+219
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+220
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+221
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+222
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+223
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+224
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+225
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+226
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+227
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+228
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+229
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+230
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+231
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+232
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+233
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+234
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+235
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+236
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+237
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+238
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+239
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+240
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+241
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+242
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+243
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+244
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+245
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+246
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+247
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+248
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+249
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+250
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+251
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+252
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+253
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+254
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+255
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+256
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+257
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+258
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+259
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+260
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+261
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+262
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+263
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+264
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+265
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+266
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+267
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+268
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+269
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+270
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+271
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+272
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+273
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+274
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+275
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+276
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+277
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+278
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+279
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+280
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+281
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+282
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+283
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+284
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+285
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+286
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+287
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+288
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+289
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+290
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+291
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+292
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+293
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+294
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+295
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+296
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+297
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+298
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+299
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+300
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+301
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+302
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+303
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+304
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+305
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+306
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+307
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+308
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+309
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+310
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+311
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+312
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+313
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+314
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+315
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+316
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+317
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+318
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+319
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+320
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+321
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+322
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+323
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+324
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+325
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+326
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+327
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+328
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+329
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+330
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+331
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+332
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+333
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+334
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+335
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+336
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+337
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+338
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+339
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+340
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+341
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+342
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+343
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+344
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+345
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+346
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+347
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+348
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+349
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+350
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+351
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+352
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+353
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+354
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+355
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+356
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+357
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+358
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+359
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+360
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+361
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+362
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+363
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+364
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+365
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+366
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+367
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+368
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+369
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+370
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+371
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+372
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+373
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+374
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+375
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+376
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+377
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+378
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+379
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+380
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+381
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+382
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SKILL_INFO_1_1+383
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_CHARACTER_POINTS1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_CHARACTER_POINTS2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_TRACK_CREATURES
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_TRACK_RESOURCES
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_BLOCK_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_DODGE_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_PARRY_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPERTISE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_OFFHAND_EXPERTISE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_CRIT_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_RANGED_CRIT_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_OFFHAND_CRIT_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SPELL_CRIT_PERCENTAGE1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SHIELD_BLOCK
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+25
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+26
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+27
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+28
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+29
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+30
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+31
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+32
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+33
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+34
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+35
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+36
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+37
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+38
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+39
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+40
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+41
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+42
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+43
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+44
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+45
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+46
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+47
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+48
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+49
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+50
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+51
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+52
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+53
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+54
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+55
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+56
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+57
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+58
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+59
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+60
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+61
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+62
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+63
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+64
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+65
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+66
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+67
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+68
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+69
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+70
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+71
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+72
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+73
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+74
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+75
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+76
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+77
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+78
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+79
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+80
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+81
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+82
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+83
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+84
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+85
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+86
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+87
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+88
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+89
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+90
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+91
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+92
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+93
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+94
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+95
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+96
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+97
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+98
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+99
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+100
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+101
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+102
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+103
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+104
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+105
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+106
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+107
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+108
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+109
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+110
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+111
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+112
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+113
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+114
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+115
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+116
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+117
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+118
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+119
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+120
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+121
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+122
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+123
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+124
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+125
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+126
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_EXPLORED_ZONES_1+127
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_REST_STATE_EXPERIENCE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COINAGE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_POS+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_HEALING_DONE_POS
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_HEALING_PCT
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_HEALING_DONE_PCT
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_TARGET_RESISTANCE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BYTES
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_AMMO_ID
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_SELF_RES_SPELL
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_PVP_MEDALS
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_PRICE_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BUYBACK_TIMESTAMP_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_KILLS
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_TODAY_CONTRIBUTION
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_YESTERDAY_CONTRIBUTION
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_LIFETIME_HONORBALE_KILLS
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_BYTES2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_WATCHED_FACTION_INDEX
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_COMBAT_RATING_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_TEAM_INFO_1_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_HONOR_CURRENCY
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_ARENA_CURRENCY
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_MAX_LEVEL
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+6
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+7
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+8
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+9
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+10
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+11
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+12
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+13
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+14
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+15
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+16
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+17
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+18
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+19
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+20
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+21
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+22
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+23
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_DAILY_QUESTS_1+24
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_RUNE_REGEN_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_RUNE_REGEN_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_RUNE_REGEN_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_RUNE_REGEN_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_NO_REAGENT_COST_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_NO_REAGENT_COST_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_NO_REAGENT_COST_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPH_SLOTS_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPH_SLOTS_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPH_SLOTS_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPH_SLOTS_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPH_SLOTS_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPH_SLOTS_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPHS_1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPHS_1+1
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPHS_1+2
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPHS_1+3
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPHS_1+4
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_FIELD_GLYPHS_1+5
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_GLYPHS_ENABLED
        UPDATEFIELD_FLAG_SELF,                                                          // PLAYER_PET_SPELL_POWER
};
enum ObjectUpdateType : u8
{
    UPDATETYPE_VALUES = 0,
    UPDATETYPE_MOVEMENT = 1,
    UPDATETYPE_CREATE_OBJECT = 2,
    UPDATETYPE_CREATE_OBJECT2 = 3,
    UPDATETYPE_OUT_OF_RANGE_OBJECTS = 4,
    UPDATETYPE_NEAR_OBJECTS = 5
};
enum ObjectUpdateFlag : u16
{
    UPDATEFLAG_NONE = 0x00,
    UPDATEFLAG_SELF = 0x01,
    UPDATEFLAG_TRANSPORT = 0x02,
    UPDATEFLAG_HAS_TARGET = 0x04,
    UPDATEFLAG_UNK4 = 0x08,
    UPDATEFLAG_LOWGUID = 0x10,
    UPDATEFLAG_LIVING = 0x20,
    UPDATEFLAG_STATIONARY_POSITION = 0x40,
    UPDATEFLAG_VEHICLE = 0x80,
    UPDATEFLAG_POSITION = 0x100,
    UPDATEFLAG_ROTATION = 0x200
};
enum EntityDynamicFlag : u8
{
    ENTIY_DYNAMICFLAG_NONE = 0x00,
    ENTIY_DYNAMICFLAG_IS_LOOTABLE = 0x01,
    ENTIY_DYNAMICFLAG_IS_TRACKED = 0x02,
    ENTIY_DYNAMICFLAG_IS_TAGGED = 0x04,
    ENTIY_DYNAMICFLAG_IS_TAGGED_BY_PLAYER = 0x08,
    ENTIY_DYNAMICFLAG_SPECIAL_INFO = 0x10,
    ENTIY_DYNAMICFLAG_IS_DEAD = 0x20,
    ENTIY_DYNAMICFLAG_IS_RAF = 0x40,
    ENTIY_DYNAMICFLAG_IS_TAGGED_BY_ALL = 0x80
};

enum Language
{
    LANG_UNIVERSAL = 0,
    LANG_ORCISH = 1,
    LANG_DARNASSIAN = 2,
    LANG_TAURAHE = 3,
    LANG_DWARVISH = 6,
    LANG_COMMON = 7,
    LANG_DEMONIC = 8,
    LANG_TITAN = 9,
    LANG_THALASSIAN = 10,
    LANG_DRACONIC = 11,
    LANG_KALIMAG = 12,
    LANG_GNOMISH = 13,
    LANG_TROLL = 14,
    LANG_GUTTERSPEAK = 33,
    LANG_DRAENEI = 35,
    LANG_ZOMBIE = 36,
    LANG_GNOMISH_BINARY = 37,
    LANG_GOBLIN_BINARY = 38,
    LANG_ADDON = 0xFFFFFFFF // used by addons, in 2.4.0 not exist, replaced by messagetype?
};
#define MAX_LANGUAGE_TYPE 19

enum ChatMsgType
{
    CHAT_MSG_ADDON = 0xFFFFFFFF, // -1
    CHAT_MSG_SYSTEM = 0x00,
    CHAT_MSG_SAY = 0x01,
    CHAT_MSG_PARTY = 0x02,
    CHAT_MSG_RAID = 0x03,
    CHAT_MSG_GUILD = 0x04,
    CHAT_MSG_OFFICER = 0x05,
    CHAT_MSG_YELL = 0x06,
    CHAT_MSG_WHISPER = 0x07,
    CHAT_MSG_WHISPER_FOREIGN = 0x08,
    CHAT_MSG_WHISPER_INFORM = 0x09,
    CHAT_MSG_EMOTE = 0x0A,
    CHAT_MSG_TEXT_EMOTE = 0x0B,
    CHAT_MSG_MONSTER_SAY = 0x0C,
    CHAT_MSG_MONSTER_PARTY = 0x0D,
    CHAT_MSG_MONSTER_YELL = 0x0E,
    CHAT_MSG_MONSTER_WHISPER = 0x0F,
    CHAT_MSG_MONSTER_EMOTE = 0x10,
    CHAT_MSG_CHANNEL = 0x11,
    CHAT_MSG_CHANNEL_JOIN = 0x12,
    CHAT_MSG_CHANNEL_LEAVE = 0x13,
    CHAT_MSG_CHANNEL_LIST = 0x14,
    CHAT_MSG_CHANNEL_NOTICE = 0x15,
    CHAT_MSG_CHANNEL_NOTICE_USER = 0x16,
    CHAT_MSG_AFK = 0x17,
    CHAT_MSG_DND = 0x18,
    CHAT_MSG_IGNORED = 0x19,
    CHAT_MSG_SKILL = 0x1A,
    CHAT_MSG_LOOT = 0x1B,
    CHAT_MSG_MONEY = 0x1C,
    CHAT_MSG_OPENING = 0x1D,
    CHAT_MSG_TRADESKILLS = 0x1E,
    CHAT_MSG_PET_INFO = 0x1F,
    CHAT_MSG_COMBAT_MISC_INFO = 0x20,
    CHAT_MSG_COMBAT_XP_GAIN = 0x21,
    CHAT_MSG_COMBAT_HONOR_GAIN = 0x22,
    CHAT_MSG_COMBAT_FACTION_CHANGE = 0x23,
    CHAT_MSG_BG_SYSTEM_NEUTRAL = 0x24,
    CHAT_MSG_BG_SYSTEM_ALLIANCE = 0x25,
    CHAT_MSG_BG_SYSTEM_HORDE = 0x26,
    CHAT_MSG_RAID_LEADER = 0x27,
    CHAT_MSG_RAID_WARNING = 0x28,
    CHAT_MSG_RAID_BOSS_EMOTE = 0x29,
    CHAT_MSG_RAID_BOSS_WHISPER = 0x2A,
    CHAT_MSG_FILTERED = 0x2B,
    CHAT_MSG_BATTLEGROUND = 0x2C,
    CHAT_MSG_BATTLEGROUND_LEADER = 0x2D,
    CHAT_MSG_RESTRICTED = 0x2E,
    CHAT_MSG_BATTLENET = 0x2F,
    CHAT_MSG_ACHIEVEMENT = 0x30,
    CHAT_MSG_GUILD_ACHIEVEMENT = 0x31,
    CHAT_MSG_ARENA_POINTS = 0x32,
    CHAT_MSG_PARTY_LEADER = 0x33,
    CHAT_MSG_TYPE_MAX
};

enum DataCacheType
{
    ACCOUNT_CONFIG_CACHE = 0,
    CHARACTER_CONFIG_CACHE = 1,
    ACCOUNT_BINDINGS_CACHE = 2,
    CHARACTER_BINDINGS_CACHE = 3,
    ACCOUNT_MACROS_CACHE = 4,
    CHARACTER_MACROS_CACHE = 5,
    CHARACTER_LAYOUT_CACHE = 6,
    CHARACTER_CHAT_CACHE = 7,
    DATA_CACHE_TYPE_MAX
};
#define ACCOUNT_DATA_CACHE_MASK 0x15
#define CHARACTER_DATA_CACHE_MASK 0xEA

enum MoveFlags
{
    MOVEFLAG_NONE = 0x00,
    MOVEFLAG_FORWARD = 0x01,
    MOVEFLAG_BACKWARD = 0x02,
    MOVEFLAG_STRAFE_LEFT = 0x04,
    MOVEFLAG_STRAFE_RIGHT = 0x08,
    MOVEFLAG_TURN_LEFT = 0x10,
    MOVEFLAG_TURN_RIGHT = 0x20,
    MOVEFLAG_PITCH_DOWN = 0x40,
    MOVEFLAG_PITCH_UP = 0x80,
    MOVEFLAG_WALK = 0x100,
    MOVEFLAG_TRANSPORT = 0x200,
    MOVEFLAG_DISABLE_GRAVITY = 0x400,
    MOVEFLAG_ROOT = 0x800,
    MOVEFLAG_FALLING = 0x1000,
    MOVEFLAG_FALLING_FAR = 0x2000,
    MOVEFLAG_PENDING_STOP = 0x4000,
    MOVEFLAG_PENDING_STOP_STRAFE = 0x8000,
    MOVEFLAG_PENDING_FORWARD = 0x10000,
    MOVEFLAG_PENDING_BACKWARD = 0x20000,
    MOVEFLAG_PENDING_STRAFE_LEFT = 0x40000,
    MOVEFLAG_PENDING_STRAFE_RIGHT = 0x80000,
    MOVEFLAG_PENDING_ROOT = 0x100000,
    MOVEFLAG_SWIM = 0x200000,
    MOVEFLAG_ASCEND = 0x400000,
    MOVEFLAG_DESCEND = 0x800000,
    MOVEFLAG_CAN_FLY = 0x1000000,
    MOVEFLAG_FLY = 0x2000000,
    MOVEFLAG_SPLINE_ELEVATION = 0x4000000,
    MOVEFLAG_SPLINE_ENABLED = 0x8000000,
    MOVEFLAG_WATER_WALK = 0x10000000,
    MOVEFLAG_FEATHER_FALL = 0x20000000,
    MOVEFLAG_HOVER = 0x40000000,

    MOVEFLAG_MASK_MOVING = MOVEFLAG_FORWARD | MOVEFLAG_BACKWARD | MOVEFLAG_STRAFE_LEFT | MOVEFLAG_STRAFE_RIGHT |
                           MOVEFLAG_FALLING | MOVEFLAG_FALLING_FAR | MOVEFLAG_ASCEND | MOVEFLAG_DESCEND |
                           MOVEFLAG_SPLINE_ELEVATION,
    MOVEFLAG_MASK_TURNING = MOVEFLAG_TURN_LEFT | MOVEFLAG_TURN_RIGHT | MOVEFLAG_PITCH_UP | MOVEFLAG_PITCH_DOWN,
    MOVEFLAG_MASK_FLYING = MOVEFLAG_FLY | MOVEFLAG_ASCEND | MOVEFLAG_DESCEND,
    MOVEFLAG_MASK_PITCH = MOVEFLAG_SWIM | MOVEFLAG_FLY
};

enum MoveFlagsExtra
{
    MOVEFLAG_EXTRA_NONE = 0x00,
    MOVEFLAG_EXTRA_NO_STRAFE = 0x01,
    MOVEFLAG_EXTRA_NO_JUMPING = 0x02,
    MOVEFLAG_EXTRA_UNK3 = 0x04,
    MOVEFLAG_EXTRA_FULL_SPEED_TURNING = 0x08,
    MOVEFLAG_EXTRA_FULL_SPEED_PITCHING = 0x10,
    MOVEFLAG_EXTRA_ALWAYS_ALLOW_PITCHING = 0x20,
    MOVEFLAG_EXTRA_UNK7 = 0x40,  // Possibly has to do with Transports (Packet_Group_24)
    MOVEFLAG_EXTRA_UNK8 = 0x80,  // Possibly has to do with Knock backs CMovement::OnMonsterMoveFlag_0x800
    MOVEFLAG_EXTRA_UNK9 = 0x100, // Possibly has to do with Transports
    MOVEFLAG_EXTRA_UNK10 = 0x200,
    MOVEFLAG_EXTRA_INTERPOLATED_MOVEMENT = 0x400,
    MOVEFLAG_EXTRA_INTERPOLATED_TURNING = 0x800,
    MOVEFLAG_EXTRA_INTERPOLATED_PITCHING = 0x1000,
    MOVEFLAG_EXTRA_UNK14 = 0x2000,
    MOVEFLAG_EXTRA_UNK15 = 0x4000,
    MOVEFLAG_EXTRA_UNK16 = 0x8000,
};

enum SpellEffect
{
    SPELL_EFFECT_NONE = 0x00,
    SPELL_EFFECT_INSTANT_KILL = 0x01,
    SPELL_EFFECT_SCHOOL_DAMAGE = 0x02,
    SPELL_EFFECT_DUMMY = 0x03,
    SPELL_EFFECT_PORTAL_TELEPORT = 0x04,
    SPELL_EFFECT_TELEPORT_UNITS = 0x05,
    SPELL_EFFECT_APPLY_AURA = 0x06,
    SPELL_EFFECT_ENVIRONMENTAL_DAMAGE = 0x07,
    SPELL_EFFECT_POWER_DRAIN = 0x08,
    SPELL_EFFECT_HEALTH_LEECH = 0x09,
    SPELL_EFFECT_HEAL = 0x0A,
    SPELL_EFFECT_BIND = 0x0B,
    SPELL_EFFECT_PORTAL = 0x0C,
    SPELL_EFFECT_RITUAL_BASE = 0x0D,
    SPELL_EFFECT_RITUAL_SPECIALIZE = 0x0E,
    SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL = 0x0F,
    SPELL_EFFECT_QUEST_COMPLETE = 0x10,
    SPELL_EFFECT_WEAPON_DAMAGE = 0x11,
    SPELL_EFFECT_RESSURECT = 0x12,
    SPELL_EFFECT_ADD_EXTRA_ATTACKS = 0x13,
    SPELL_EFFECT_DODGE = 0x14,
    SPELL_EFFECT_EVADE = 0x15,
    SPELL_EFFECT_PARRY = 0x16,
    SPELL_EFFECT_BLOCK = 0x17,
    SPELL_EFFECT_CREATE_ITEM = 0x18,
    SPELL_EFFECT_WEAPON = 0x19,
    SPELL_EFFECT_DEFENSE = 0x1A,
    SPELL_EFFECT_PERSISTENT_AREA_AURA = 0x1B,
    SPELL_EFFECT_SUMMON = 0x1C,
    SPELL_EFFECT_LEAP = 0x1D
};

enum AuraApplyName
{
    SPELL_AURA_FLY = 201
};

enum AuraClientFlags
{
    AURA_CLIENT_FLAG_NONE = 0x00,
    AURA_CLIENT_FLAG_EFFECT_INDEX_1 = 0x01,
    AURA_CLIENT_FLAG_EFFECT_INDEX_2 = 0x02,
    AURA_CLIENT_FLAG_EFFECT_INDEX_3 = 0x04,
    AURA_CLIENT_FLAG_IS_CASTER = 0x08,
    AURA_CLIENT_FLAG_IS_POSITIVE = 0x10,
    AURA_CLIENT_FLAG_HAS_DURATION = 0x20,
    AURA_CLIENT_FLAG_IS_NEGATIVE = 0x80,
};