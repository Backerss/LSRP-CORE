CREATE TABLE IF NOT EXISTS `rp_factions`
(
    `id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
    `creator_id` bigint(20) UNSIGNED NOT NULL DEFAULT '0',
    `owner_id` bigint(20) UNSIGNED NOT NULL DEFAULT '0',
    `name` varchar(32) NOT NULL DEFAULT '0',
    `subtype` smallint(5) UNSIGNED NOT NULL,
    `color` smallint(5) UNSIGNED NOT NULL DEFAULT '0',
    `initial` varchar(16) NOT NULL DEFAULT '',
    PRIMARY KEY 
        (`id`),
    KEY `player_id` (`owner_id`),
    KEY `owner_id` (`creator_id`)
);