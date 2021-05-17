CREATE TABLE IF NOT EXISTS `rp_faction_members` (
  `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
  `faction_id` bigint(20) UNSIGNED NOT NULL DEFAULT '0',
  `player_id` bigint(20) UNSIGNED NOT NULL DEFAULT '0',
  `rankId` tinyint(3) UNSIGNED NOT NULL DEFAULT '0',
  `spawn` bigint(20) UNSIGNED DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `faction_id_player_id` (`faction_id`,`player_id`),
  UNIQUE KEY `faction_id_2` (`faction_id`,`player_id`,`spawn`),
  FOREIGN KEY
      (player_id)
  REFERENCES
      characters(c_id)
  ON DELETE
      CASCADE
  ON UPDATE
      NO ACTION
);