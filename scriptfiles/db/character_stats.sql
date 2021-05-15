CREATE TABLE IF NOT EXISTS `character_stats`
(
    `c_id` bigint(20) UNSIGNED NOT NULL,
    `developer` tinyint(1) NOT NULL DEFAULT 0,
    `age` tinyint(3) UNSIGNED NOT NULL,
    `armour` float NOT NULL DEFAULT '100.0',
    `health` float NOT NULL DEFAULT '100.0',
    `kills` mediumint(8) NOT NULL DEFAULT '0',
    `deaths` mediumint(8) NOT NULL DEFAULT '0',
    `job_id` tinyint(4) NOT NULL DEFAULT '0',
    `class_id` tinyint(4) NOT NULL DEFAULT '0',
    `wanted_level` mediumint(6) NOT NULL DEFAULT '0',
    `player_bounty` mediumint(8) NOT NULL DEFAULT '0',
    `money` bigint(20) NOT NULL DEFAULT '0',
    `score` int(11) NOT NULL DEFAULT '0',
    `xp` int(11) UNSIGNED NOT NULL DEFAULT '0',
    `skin` smallint(4) NOT NULL DEFAULT '0',
    PRIMARY KEY
        (c_id),
    FOREIGN KEY
        (c_id)
    REFERENCES
        characters(c_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);