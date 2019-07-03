-- Dumping database structure for dbc
CREATE DATABASE IF NOT EXISTS `dbc` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `dbc`;

-- Dumping structure for table dbc.map
DROP TABLE IF EXISTS `map`;
CREATE TABLE IF NOT EXISTS `map` (
  `id` smallint(5) unsigned NOT NULL,
  `internalName` varchar(255) NOT NULL,
  `instanceType` int(10) unsigned NOT NULL,
  `flags` int(10) unsigned NOT NULL,
  `name` varchar(255) NOT NULL,
  `expansion` int(10) unsigned NOT NULL,
  `maxPlayers` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Dumping structure for table dbc.emotes_text
DROP TABLE IF EXISTS `emotes_text`;
CREATE TABLE IF NOT EXISTS `emotes_text`(
	`id` INT unsigned NOT NULL,
	`internalName` varchar(255) NOT NULL,
	`animationId` INT unsigned NOT NULL,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;