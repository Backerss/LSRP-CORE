CREATE TABLE IF NOT EXISTS `characters`
(
    `c_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
    `u_id` bigint(20) UNSIGNED NOT NULL,
    `player_name` varchar(24) NOT NULL,
    `create_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `last_login` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY
        (c_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        accounts(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);