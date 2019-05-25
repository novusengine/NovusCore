CREATE DATABASE IF NOT EXISTS `auth`;
USE `auth`;

CREATE TABLE IF NOT EXISTS `accounts` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(32) NOT NULL,
  `salt` varchar(64) NOT NULL,
  `verifier` varchar(64) NOT NULL,
  `sessionKey` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

DELETE FROM `accounts`;
INSERT INTO `accounts` (`guid`, `username`, `salt`, `verifier`, `sessionKey`) VALUES
	(1, 'ADMIN', 'AD31174982EF8F0B8686FCAC4857D89D093D5C000F68DFB102178FBC8D3ADFA9', '10DB5B75BCF6946B5F4DECCA7C5635D04374FFAE4105620C824A644FE876AE92', 'E7379390AA3C1624BEEDA7C6675C245AA84BB4A6962700278E96F60229057A5494FC99568B68B4B7');

CREATE TABLE IF NOT EXISTS `account_data` (
  `accountGuid` int(11) unsigned NOT NULL,
  `type` int(11) unsigned NOT NULL,
  `timestamp` int(11) unsigned NOT NULL,
  `data` blob NOT NULL,
  PRIMARY KEY (`accountGuid`,`type`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `account_data`;

CREATE TABLE IF NOT EXISTS `realms` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(64) NOT NULL DEFAULT 'NovusCore Realm',
  `address` varchar(64) NOT NULL DEFAULT '127.0.0.1:8000' COMMENT 'Specify IP:Port',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '0 = PvE, 1 = PvP, 4 = Normal, 6 = RP, 8 = RPPvP, 14 = Max Client',
  `flags` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '0x01 = Version Mismatch, 0x02 = Offline, 0x04 = Specify Build, 0x08 = unk1, 0x10 = unk2, 0x20 = Recommended, 0x40 = New, 0x80 = Full',
  `timezone` tinyint(3) unsigned NOT NULL DEFAULT '1' COMMENT '0 = hide, > 0 = Timezone',
  `population` float NOT NULL DEFAULT '0' COMMENT '0 = Low, 1 = Medium, 2 = High',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
DELETE FROM `realms`;
INSERT INTO `realms` (`id`, `name`, `address`, `type`, `flags`, `timezone`, `population`) VALUES
	(1, 'NovusCore Realm', '127.0.0.1:8000', 1, 32, 1, 0);

CREATE TABLE IF NOT EXISTS `realm_characters` (
  `account` int(10) unsigned NOT NULL,
  `realmId` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `characters` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`account`,`realmId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
DELETE FROM `realm_characters`;
