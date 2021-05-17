CREATE TABLE IF NOT EXISTS `rp_faction_ranks` (
  `faction_id` bigint(20) UNSIGNED NOT NULL,
  `id` tinyint(3) UNSIGNED NOT NULL,
  `name` varchar(24) NOT NULL,
  `flags` int(10) UNSIGNED NOT NULL,
  UNIQUE KEY `faction_id` (`faction_id`,`id`)
);