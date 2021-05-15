CREATE TABLE IF NOT EXISTS `accounts`
(
    `u_id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT,
    `username` varchar(24) NOT NULL,
    `password` char(60) NOT NULL,
    `register_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
    `last_login` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY
        (u_id),
    UNIQUE KEY
        username (username)
);