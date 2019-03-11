/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dumping database structure for authserver
CREATE DATABASE IF NOT EXISTS `authserver` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `authserver`;

-- Dumping structure for table authserver.accounts
CREATE TABLE IF NOT EXISTS `accounts` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(32) NOT NULL,
  `salt` varchar(64) NOT NULL,
  `verifier` varchar(64) NOT NULL,
  `sessionkey` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- Dumping structure for table authserver.realms
CREATE TABLE IF NOT EXISTS `realms` (
  `id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(64) NOT NULL DEFAULT 'NovusCore Realm',
  `address` varchar(64) NOT NULL DEFAULT '127.0.0.1:8000' COMMENT 'Specify IP:Port',
  `type` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '0 = PvE, 1 = PvP, 4 = Normal, 6 = RP, 8 = RPPvP, 14 = Max Client',
  `flags` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '0x01 = Version Mismatch, 0x02 = Offline, 0x04 = Specify Build, 0x08 = unk1, 0x10 = unk2, 0x20 = Recommended, 0x40 = New, 0x80 = Full',
  `timezone` tinyint(3) unsigned NOT NULL DEFAULT '1' COMMENT '0 = hide, > 0 = Timezone',
  `population` float NOT NULL DEFAULT '0' COMMENT '0 = Low, 1 = Medium, 2 = High',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

-- Dumping data for table authserver.realms: ~0 rows (approximately)
/*!40000 ALTER TABLE `realms` DISABLE KEYS */;
INSERT INTO `realms` (`id`, `name`, `address`, `type`, `flags`, `timezone`, `population`) VALUES
	(1, 'NovusCore Realm', '127.0.0.1:8000', 1, 0, 1, 0),
/*!40000 ALTER TABLE `realms` ENABLE KEYS */;

-- Dumping structure for table authserver.realm_characters
CREATE TABLE IF NOT EXISTS `realm_characters` (
  `account` int(10) unsigned NOT NULL,
  `realmid` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `characters` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`account`,`realmid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;