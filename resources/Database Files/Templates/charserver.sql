/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dumping database structure for charserver
CREATE DATABASE IF NOT EXISTS `charserver` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `charserver`;

-- Dumping structure for table charserver.characters
CREATE TABLE IF NOT EXISTS `characters` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT,
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
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=latin1;

-- Dumping structure for table charserver.character_visual_data
CREATE TABLE IF NOT EXISTS `character_visual_data` (
  `guid` int(10) unsigned NOT NULL,
  `skin` tinyint(3) unsigned NOT NULL,
  `face` tinyint(3) unsigned NOT NULL,
  `facial_style` tinyint(3) unsigned NOT NULL,
  `hair_style` tinyint(3) unsigned NOT NULL,
  `hair_color` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;