#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    Statement: stmt_checkBan,
    Statement: stmt_getUID
;


hook OnMySQLConnected() {

    static const checkQuery[] = "\
        SELECT \
            *,  DATE_FORMAT(ban_date,'%d %M %Y เวลา %r') AS date_ban, \
            DATE_FORMAT(unban_date,'%d %M %Y เวลา %r') AS date_unban, \
            TIMESTAMPDIFF(SECOND, CURRENT_TIMESTAMP(), unban_date) AS expired \
        FROM \
            bans \
        WHERE \
            username = ? OR ip = ? LIMIT 1\
    ";
    stmt_checkBan = MySQL_PrepareStatement (MySQL_GetHandle(), checkQuery);

    static const checkuidQuery[] = "\
    SELECT \
        username \
    FROM \
        accounts \
    WHERE \
        username = ? \
    ";
    stmt_getUID = MySQL_PrepareStatement(MySQL_GetHandle(), checkuidQuery);
}


hook OnGameModeInit()
{
	for (new i = 0; i != Y_PLAYER_ADMIN_MAX_LEVEL; ++i) {
		new name[32];
		format(name, sizeof name, "Admin level %d", i + 1);
		AdminGroup[i] = Group_Create(name);

		if (i > 0) {
			Group_AddChild(AdminGroup[i], AdminGroup[i - 1]);
		}
	}

	DeveloperGroup = Group_Create("Developer");
}

hook OnGroupInit()
{
	Group_AddChild(AdminGroup[0], DeveloperGroup);
	return 1;
}

void:Admin_AddBanUser(playerid, byid = INVALID_PLAYER_ID, reason[], interval_type, banduration) {
    new
        playerIP[16 + 1],
        bannerName[MAX_PLAYER_NAME + 1],
        string[512],
        playerGCPI[60],
        playerName[MAX_PLAYER_NAME + 1]
    ;

    GetPlayerName(playerid, playerName, sizeof(playerName));
    GetPlayerIp(playerid, playerIP, sizeof(playerIP));
    gpci(playerid, playerGCPI, sizeof(playerGCPI));

    if (byid != INVALID_PLAYER_ID)
    {
        GetPlayerName(byid, bannerName, sizeof(bannerName));
    }
    else
    {
        strcpy(bannerName, "เซิร์ฟเวอร์", sizeof(bannerName));
    }

    new
        interval[6]
    ;
    switch(interval_type) {
        case 0: strcpy(interval, "DAY", sizeof(interval));
        case 1: strcpy(interval, "MONTH", sizeof(interval));
        case 2: strcpy(interval, "YEAR", sizeof(interval));
    }

    static const query[] = "\
        INSERT INTO bans \
            (username, ip, gcpi, reason, admin, ban_date, unban_date) \
        VALUES \
            ('%e', '%e', '%e', '%e', '%e',  CURRENT_TIMESTAMP(), DATE_ADD(CURRENT_TIMESTAMP(), INTERVAL %i %s))"
    ;
    mysql_format(MySQL_GetHandle(), string, sizeof(string), query, playerName, playerIP, playerGCPI, reason, bannerName, banduration, interval);
    return mysql_tquery(MySQL_GetHandle(), string);
}

void:Admin_AddOffBanUser(const username[], byid = INVALID_PLAYER_ID, reason[], interval_type, banduration)
{
    new
        banusername[MAX_PLAYER_NAME + 1],
        string[512],
        bannerName[MAX_PLAYER_NAME + 1],
        banreason[MAX_BAN_REASON]
	;

    strcpy(banusername, username, sizeof(banusername));

    if(byid != INVALID_PLAYER_ID) {
        GetPlayerName(byid, bannerName, sizeof(bannerName));
    }
    else {
        strcpy(bannerName, "เซิร์ฟเวอร์", sizeof(banusername));
    }

    inline OnPlayerOffBan() {
        new
            playerNAME[MAX_PLAYER_NAME];

        MySQL_BindResult(stmt_getUID, 0, playerNAME);

        if(MySQL_Statement_FetchRow(stmt_getUID)) {

            new
                interval[6]
            ;
            switch(interval_type) {
                case 0: strcpy(interval, "DAY", sizeof(interval));
                case 1: strcpy(interval, "MONTH", sizeof(interval));
                case 2: strcpy(interval, "YEAR", sizeof(interval));
            }

            static const query[] = "\
            INSERT INTO bans \
                (username, ip, gcpi, reason, admin, ban_date, unban_date) \
            VALUES \
                ('%e', '%e', '%e', '%e', '%e',  CURRENT_TIMESTAMP(), DATE_ADD(CURRENT_TIMESTAMP(), INTERVAL %i %s))"
            ;
            mysql_format(MySQL_GetHandle(), string, sizeof(string), query, playerNAME, 0, 0, reason, bannerName, banduration, interval);
            mysql_tquery(MySQL_GetHandle(), string);
            SendMsgAllF(X11_FIREBRICK, "[ระงับบัญชีแบบ Offline]: %s ได้ถูกแบนเนื่องจาก %s", playerNAME, banreason);
        }
        else {
            if(byid != INVALID_PLAYER_ID) {
                return SendErrorMsgF(byid, "บัญชี "GREY"(%s) "WHITE"นี้ไม่มีอยู่ในฐานข้อมูล", banusername);
            }
        }
    }
    MySQL_Bind(stmt_getUID, 0, banusername);
    MySQL_ExecuteParallel_Inline(stmt_getUID, using inline OnPlayerOffBan);
    return 1;
}

void:Admin_RemoveBanUser(username[], playerid = INVALID_PLAYER_ID) {
    new
        string[45 + MAX_PLAYER_NAME + 1],
        name[MAX_PLAYER_NAME + 1];

    strcpy(name, username, sizeof(name));

    inline OnPlayerRemoveBan() {
        if (!cache_num_rows()) {
            if (playerid != INVALID_PLAYER_ID) {
                SendServerMsgF(playerid, "บัญชี "GREY"(%s) "WHITE"นี้ที่คุณพิมพ์เพื่อปลดแบนไม่มีอยู่ในฐานข้อมูล", name);
                return;
            }
        }

        mysql_format(MySQL_GetHandle(), string, sizeof(string), "DELETE FROM bans WHERE username = '%e'", name);
        mysql_tquery(MySQL_GetHandle(), string, "", "");
        if (playerid != INVALID_PLAYER_ID) {
            SendAdminMsgF(ADMIN_MSG_INFO, 1, "บัญชี %s ได้รับการปลดแบนโดย %s", name, Player_GetCharacterName(playerid));
            SendServerMsgF(playerid, "คุณประสบความสำเร็จในการปลดแบนบัญชี "GREY"%s", name);
        }
    }

    static const query[] = "\
        SELECT \
            username \
        FROM \
            bans \
        WHERE \
            username = '%e'"
    ;
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnPlayerRemoveBan, query, username, playerid);
}

void:Admin_CheckBanUser(playerid) {

    new
        playerIP[20];

    GetPlayerIp(playerid, playerIP, sizeof(playerIP));

    inline OnPlayerBanLoad() {
        new
            banIP[16 + 1],
            banName[MAX_PLAYER_NAME],
            banAdmin[MAX_PLAYER_NAME],
            banReason[MAX_BAN_REASON],
            banDate[40],
            unbanDate[40],
            expired;

        if (cache_num_rows()) {
            cache_get_value_name(0, "username", banName, MAX_PLAYER_NAME);
            cache_get_value_name(0, "ip",  banIP, 20);
            cache_get_value_name(0, "admin", banAdmin, MAX_PLAYER_NAME);
            cache_get_value_name(0, "reason", banReason, MAX_BAN_REASON);

            cache_get_value_name(0, "date_ban", banDate, 40);
            cache_get_value_name(0, "date_unban", unbanDate, 40);

            cache_get_value_name_int(0, "expired", expired);

            if (expired <= 0) {
                SendMsgF(playerid, X11_WHITE, "คุณได้รับการปลดแบนแล้วเมื่อ "GREY"%s "WHITE"\nยินดีต้อนรับการกลับมา %s(%i) อย่ากระทำผิดกฎใด ๆ อีก", unbanDate, Player_GetAccountName(playerid), playerid);
                Admin_RemoveBanUser(banName, INVALID_PLAYER_ID);
                CallLocalFunction("OnPlayerPassedBanCheck", "i", playerid);
            }
            else {
                new string[512];
                format(string, sizeof(string), "\
                    "WHITE"ชื่อผู้ใช้: "GREY"%s\n\
                    "WHITE"ไอพีที่ถูกแบน: "GREY"%s\n\
                    "WHITE"ไอพีปัจจุบัน: "GREY"%s\n\
                    "WHITE"แบนโดย: "GREY"%s\n\
                    "WHITE"สาเหตุของการแบน: "GREY"%s\n\
                    "WHITE"วันที่ถูกแบน: "GREY"%s\n\
                    "WHITE"วันที่ได้รับการปลดแบน: "GREY"%s", banName, banIP, playerIP, banAdmin, banReason, banDate, unbanDate);
                Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "ข้อมูลการแบน", string, "ปิด", "");
                Kick(playerid);
            }
        } else {
            CallLocalFunction("OnPlayerPassedBanCheck", "i", playerid);
        }
    }

    MySQL_Bind(stmt_checkBan, 0, Player_GetAccountName(playerid));
    MySQL_Bind(stmt_checkBan, 1, playerIP);

    MySQL_ExecuteParallel_Inline(stmt_checkBan, using inline OnPlayerBanLoad);
}

Admin_SetPlayerDeveloper(playerid, bool:status)
{
    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:admin:core / Admin_SetPlayerDeveloper] Developer privileges", 
        Logger_P(playerid), 
        Logger_I("status", status)
    );

    Group_SetPlayer(DeveloperGroup, playerid, status);
}