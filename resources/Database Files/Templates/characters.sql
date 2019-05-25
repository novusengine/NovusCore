CREATE DATABASE IF NOT EXISTS `characters`;
USE `characters`;

CREATE TABLE IF NOT EXISTS `characters` (
  `guid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `account` int(10) unsigned NOT NULL,
  `name` varchar(32) NOT NULL,
  `race` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `gender` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `level` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `mapId` smallint(5) unsigned NOT NULL DEFAULT '0',
  `zoneId` smallint(5) unsigned NOT NULL DEFAULT '0',
  `coordinate_x` float NOT NULL DEFAULT '0',
  `coordinate_y` float NOT NULL DEFAULT '0',
  `coordinate_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `lastLogin` int(10) unsigned NOT NULL DEFAULT '0',
  `online` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  UNIQUE KEY `name` (`name`),
  KEY `account` (`account`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;
DELETE FROM `characters`;

CREATE TABLE IF NOT EXISTS `character_data` (
  `guid` bigint(20) unsigned NOT NULL,
  `type` int(11) unsigned NOT NULL,
  `timestamp` int(11) unsigned NOT NULL,
  `data` blob NOT NULL,
  PRIMARY KEY (`guid`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `character_data`;

CREATE TABLE IF NOT EXISTS `character_items` (
  `lowGuid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `itemEntry` int(10) unsigned NOT NULL,
  `bagSlot` tinyint(3) unsigned NOT NULL,
  `bagPosition` int(10) unsigned NOT NULL,
  `characterGuid` bigint(20) unsigned NOT NULL,
  PRIMARY KEY (`lowGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `character_items`;

CREATE TABLE IF NOT EXISTS `character_skill_storage` (
  `guid` bigint(20) unsigned NOT NULL,
  `skill` smallint(5) unsigned NOT NULL,
  `value` smallint(5) unsigned NOT NULL DEFAULT '0',
  `maxValue` smallint(5) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `guid_spell` (`guid`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `character_skill_storage`;

CREATE TABLE IF NOT EXISTS `character_spell_storage` (
  `guid` bigint(20) unsigned NOT NULL,
  `spell` int(10) unsigned NOT NULL,
  UNIQUE KEY `guid_spell` (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `character_spell_storage`;

CREATE TABLE IF NOT EXISTS `character_visual_data` (
  `guid` bigint(20) unsigned NOT NULL,
  `skin` tinyint(3) unsigned NOT NULL,
  `face` tinyint(3) unsigned NOT NULL,
  `facial_style` tinyint(3) unsigned NOT NULL,
  `hair_style` tinyint(3) unsigned NOT NULL,
  `hair_color` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `character_visual_data`;

CREATE TABLE IF NOT EXISTS `default_skills` (
  `raceMask` smallint(6) NOT NULL DEFAULT '-1',
  `classMask` smallint(6) NOT NULL DEFAULT '-1',
  `skill` smallint(6) unsigned NOT NULL,
  `value` smallint(6) unsigned NOT NULL DEFAULT '0',
  `maxValue` smallint(6) unsigned NOT NULL DEFAULT '0',
  `comment` varchar(50) NOT NULL DEFAULT '',
  UNIQUE KEY `raceMask_classMask_skill` (`raceMask`,`classMask`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `default_skills`;
INSERT INTO `default_skills` (`raceMask`, `classMask`, `skill`, `value`, `maxValue`, `comment`) VALUES
	(-1, -1, 162, 1, 5, 'Weapon Skill: Unarmed'),
	(-1, -1, 415, 1, 1, 'Armor Skill: Cloth'),
	(690, -1, 109, 300, 300, 'Language: Orcish'),
	(1101, -1, 98, 300, 300, 'Language: Common');

CREATE TABLE IF NOT EXISTS `default_spawns` (
  `raceMask` smallint(6) NOT NULL DEFAULT '-1',
  `classMask` smallint(6) NOT NULL DEFAULT '-1',
  `mapId` smallint(6) unsigned NOT NULL DEFAULT '0',
  `zoneId` smallint(6) unsigned NOT NULL DEFAULT '0',
  `coordinate_x` float NOT NULL DEFAULT '0',
  `coordinate_y` float NOT NULL DEFAULT '0',
  `coordinate_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `comment` varchar(50) NOT NULL DEFAULT '',
  UNIQUE KEY `raceMask_classMask` (`raceMask`,`classMask`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `default_spawns`;
INSERT INTO `default_spawns` (`raceMask`, `classMask`, `mapId`, `zoneId`, `coordinate_x`, `coordinate_y`, `coordinate_z`, `orientation`, `comment`) VALUES
	(-1, 32, 609, 4298, 2355.84, -5664.77, 426.028, 3.65997, 'Death Knight'),
	(1, 411, 0, 12, -8949.95, -132.493, 83.5312, 0, 'Human'),
	(2, 333, 1, 14, -618.518, -4251.67, 38.718, 0, 'Orc'),
	(4, 31, 0, 1, -6240.32, 331.033, 382.758, 6.17716, 'Dwarf'),
	(8, 1053, 1, 141, 10311.3, 832.463, 1326.41, 5.69632, 'Night Elf'),
	(16, 409, 0, 85, 1676.71, 1678.31, 121.67, 2.70526, 'Undead'),
	(32, 1093, 1, 215, -2917.58, -257.98, 52.9968, 0, 'Tauren'),
	(64, 393, 0, 1, -6240, 331, 383, 0, 'Gnome'),
	(128, 221, 1, 14, -618.518, -4251.67, 38.718, 0, 'Troll'),
	(512, 414, 530, 3431, 10349.6, -6357.29, 33.4026, 5.31605, 'Blood Elf'),
	(1024, 215, 530, 3526, -3961.64, -13931.2, 100.615, 2.08364, 'Draenie');
  
CREATE TABLE IF NOT EXISTS `default_spells` (
  `raceMask` smallint(6) NOT NULL DEFAULT '-1',
  `classMask` smallint(6) NOT NULL DEFAULT '-1',
  `spell` int(11) NOT NULL,
  `comment` varchar(50) NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `default_spells`;
INSERT INTO `default_spells` (`raceMask`, `classMask`, `spell`, `comment`) VALUES
	(1101, -1, 668, 'Language: Common'),
	(690, -1, 669, 'Language: Orcish'),
	(-1, -1, 6233, 'Closing'),
	(-1, -1, 6246, 'Closing'),
	(-1, -1, 21652, 'Closing'),
	(-1, -1, 3365, 'Opening'),
	(-1, -1, 6247, 'Opening'),
	(-1, -1, 6477, 'Opening'),
	(-1, -1, 6478, 'Opening'),
	(-1, -1, 21651, 'Opening'),
	(-1, -1, 22810, 'Opening - No Text'),
	(-1, -1, 61437, 'Opening'),
	(-1, -1, 68398, 'Opening'),
	(-1, -1, 22027, 'Remove Insignia'),
	(-1, -1, 7355, 'Stuck'),
	(-1, -1, 203, 'Unarmed'),
	(-1, -1, 2382, 'Generic'),
	(-1, -1, 9125, 'Generic'),
	(-1, -1, 204, 'Defense'),
	(-1, -1, 16092, 'Defensive State (DND)'),
	(-1, -1, 9078, 'Cloth'),
	(-1, -1, 6603, 'Auto Attack'),
	(-1, -1, 1953, 'Blink - DEVELOPMENT');