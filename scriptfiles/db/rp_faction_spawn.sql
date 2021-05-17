CREATE TABLE IF NOT EXISTS `rp_faction_spawn` (
  `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `faction_id` bigint(20) UNSIGNED NOT NULL,
  `spawn_name` varchar(64) NOT NULL,
  `spawn_x` float NOT NULL,
  `spawn_y` float NOT NULL,
  `spawn_z` float NOT NULL,
  `spawn_a` float NOT NULL,
  `spawn_int` int(11) NOT NULL,
  `spawn_world` int(11) NOT NULL,
  `spawn_door` int(10) UNSIGNED NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `FK_faction_spawn` (`faction_id`)
);