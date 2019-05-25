CREATE DATABASE IF NOT EXISTS `dbc`;
USE `dbc`;

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

DROP TABLE IF EXISTS `emotesText`;
CREATE TABLE IF NOT EXISTS `emotesText`(
	`id` INT unsigned NOT NULL,
	`internalName` varchar(255) NOT NULL,
	`animationId` INT unsigned NOT NULL,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
