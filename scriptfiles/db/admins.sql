CREATE TABLE IF NOT EXISTS `admins` (
    `u_id` bigint(20) UNSIGNED NOT NULL,
    `admin_level` TINYINT(2) NOT NULL DEFAULT 0,
    PRIMARY KEY
        (u_id),
    FOREIGN KEY
        (u_id)
    REFERENCES
        accounts(u_id)
    ON DELETE
        CASCADE
    ON UPDATE
        NO ACTION
);