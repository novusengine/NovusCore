/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


CREATE DATABASE IF NOT EXISTS `charserver` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `charserver`;

CREATE TABLE IF NOT EXISTS `characters` (
  `guid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `account` int(10) unsigned NOT NULL,
  `name` varchar(32) NOT NULL,
  `race` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `gender` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `level` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `map_id` int(10) unsigned NOT NULL DEFAULT '0',
  `zone_id` int(10) unsigned NOT NULL DEFAULT '0',
  `coordinate_x` float NOT NULL DEFAULT '0',
  `coordinate_y` float NOT NULL DEFAULT '0',
  `coordinate_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`),
  UNIQUE KEY `name` (`name`),
  KEY `account` (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `character_skill_storage` (
  `guid` bigint(20) NOT NULL,
  `skill` smallint(5) unsigned NOT NULL,
  `value` smallint(5) unsigned NOT NULL DEFAULT '0',
  `maxValue` smallint(5) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `guid_spell` (`guid`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `character_spell_storage` (
  `guid` bigint(20) NOT NULL,
  `spell` int(10) unsigned NOT NULL,
  UNIQUE KEY `guid_spell` (`guid`,`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `character_visual_data` (
  `guid` bigint(20) unsigned NOT NULL,
  `skin` tinyint(3) unsigned NOT NULL,
  `face` tinyint(3) unsigned NOT NULL,
  `facial_style` tinyint(3) unsigned NOT NULL,
  `hair_style` tinyint(3) unsigned NOT NULL,
  `hair_color` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `default_skills` (
  `raceMask` smallint(6) NOT NULL DEFAULT '-1',
  `classMask` smallint(6) NOT NULL DEFAULT '-1',
  `skill` smallint(6) unsigned NOT NULL,
  `value` smallint(6) unsigned NOT NULL DEFAULT '0',
  `maxValue` smallint(6) unsigned NOT NULL DEFAULT '0',
  `comment` varchar(50) NOT NULL DEFAULT '',
  UNIQUE KEY `raceMask_classMask_skill` (`raceMask`,`classMask`,`skill`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40000 ALTER TABLE `default_skills` DISABLE KEYS */;
INSERT INTO `default_skills` (`raceMask`, `classMask`, `skill`, `value`, `maxValue`, `comment`) VALUES
	(-1, -1, 162, 1, 5, 'Weapon Skill: Unarmed'),
	(-1, -1, 415, 1, 1, 'Armor Skill: Cloth'),
	(690, -1, 109, 300, 300, 'Language: Orcish'),
	(1101, -1, 98, 300, 300, 'Language: Common');
/*!40000 ALTER TABLE `default_skills` ENABLE KEYS */;

CREATE TABLE IF NOT EXISTS `default_spells` (
  `raceMask` smallint(6) NOT NULL DEFAULT '-1',
  `classMask` smallint(6) NOT NULL DEFAULT '-1',
  `spell` int(11) NOT NULL,
  `comment` varchar(50) NOT NULL DEFAULT ''
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40000 ALTER TABLE `default_spells` DISABLE KEYS */;
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
	(-1, -1, 6603, 'Auto Attack');
/*!40000 ALTER TABLE `default_spells` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
