#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    PlayerAdminLevel[MAX_PLAYERS],
    BitArray: adminOnDuty<MAX_PLAYERS>,
    Statement: stmt_retrieveAdmin;

hook OnGameModeInit() {
    static const query_retrieveAdmin[] = "\
        SELECT \
            admin_level \
        FROM  \
            admins \
        WHERE \
            u_id = ? \
        "
    ;
    stmt_retrieveAdmin = MySQL_PrepareStatement(MySQL_GetHandle(), query_retrieveAdmin);
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerAdminLevel[playerid] = 0 ;
    return 1;
}

hook OnRconLoginAttempt(ip[], password[], success)
{
	new ip2 = IPToInt(ip);

	if (success) {
		defer RetrieveRconPlayer(ip2);
	}

	return 1;
}

hook OnPlayerRconLogin(playerid)
{
	Admin_SetPlayerLevel(playerid, Y_PLAYER_ADMIN_MAX_LEVEL, true);
	return 1;
}

timer RetrieveRconPlayer[1000](ip)
{
    new
        playerIP[16 + 1];

	foreach(new i : Player) {
        GetPlayerIp(i, playerIP, sizeof(playerIP));
		if (IPToInt(playerIP) == ip && IsPlayerAdmin(i)) {
			CallRemoteFunction("OnPlayerRconLogin", "i", i);
			return 1;
		}
	}

	return 1;
}

hook OnPlayerAccountLogin(playerid) {
    inline OnPlayerAdminLoad() {
        new
            adminLevel;

        MySQL_BindResultInt(stmt_retrieveAdmin, 0, adminLevel);

        if(MySQL_Statement_FetchRow(stmt_retrieveAdmin)) {
            Admin_SetPlayerLevel(playerid, adminLevel);
        }

    }
    MySQL_BindInt(stmt_retrieveAdmin, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded_Inline(stmt_retrieveAdmin, using inline OnPlayerAdminLoad);
}

stock Admin_SetPlayerLevel(playerid, level, bool:force = false) {

	if (!force && IsPlayerAdmin(playerid)) {
		return 2;
	}

    Admin_ClearPlayerLevel(playerid);

	if (!(1 <= level <= Y_PLAYER_ADMIN_MAX_LEVEL)) {
		Logger_Err("invalid player admin level", 
            Logger_P(playerid), 
            Logger_I("level", level)
        );
		return 1;
	}

	Group_SetPlayer(AdminGroup[level - 1], playerid, true);
	PlayerAdminLevel[playerid] = level;

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:admin:account / Admin_SetPlayerLevel] Administrative privileges", 
        Logger_P(playerid), 
        Logger_I("level", level)
    );

    new string[256];

    static const query[] = "\
    INSERT INTO \
        admins(u_id, admin_level) \
    VALUES \
        (%i, %i) \
    ON DUPLICATE KEY UPDATE \
        admin_level = %i \
    ";

    mysql_format(MySQL_GetHandle(), string, sizeof(string), query, Player_GetAccountID(playerid), level, level);
    mysql_tquery(MySQL_GetHandle(), string);

	return 0;
}

stock Admin_GetPlayerLevel(playerid)
    return PlayerAdminLevel[playerid];

stock Admin_IsPlayerAdminLevel(playerid, level)
{
	if (!Admin_GetPlayerLevel(playerid)) {
		return false;
	}

	if (1 <= level <= Y_PLAYER_ADMIN_MAX_LEVEL) {
		return Admin_GetPlayerLevel(playerid) >= level;
	}

	return false;
}

Admin_ClearPlayerLevel(playerid)
{
	if (PlayerAdminLevel[playerid] > 0) {
		Group_SetPlayer(AdminGroup[PlayerAdminLevel[playerid] - 1], playerid, false);	
	}
}

stock SendAdminCmdUsage(playerid, const cmd[], const params[]) {

    if(Admin_GetPlayerLevel(playerid) == 5) {
        return 0;
    }

   // loop through all players
    foreach(new i : Player) {
        // only send message if admin level is more than 1
        if (Admin_GetPlayerLevel(i) >= 1) {
            SendMsgF(i, COLOR_MODERATION, "(AdmCmd) "C_GREY"%p(%d) "C_WHITE"used "C_GREY"/%s %s", playerid, playerid, cmd, params);
        }
    }
    return 1;
}

stock SendAdminMsg(E_ADMIN_MSG:type, level, const msg[]) {

	if (level < 1 || level > Y_PLAYER_ADMIN_MAX_LEVEL) {
		Logger_Err("invalid admin level", 
            Logger_I("level", level)
        );
		return;
	}

	new
		out[144],
        color = X11_YELLOW2
    ;

    switch (type) {
		case ADMIN_MSG_INFO: {
			color = X11_YELLOW3;
			format(out, sizeof out, "AdmLog: %s", msg);
		}
		case ADMIN_MSG_WARNING: {
			color = X11_YELLOW1;
			format(out, sizeof out, "AdmWarning: %s", msg);
		}
        default: {
            strcpy(out, msg);
        }
	}

    new szMsgWithoutColor[2048];
    strcpy(szMsgWithoutColor, out);
    StripColorEmbedding(szMsgWithoutColor);

    if (type == ADMIN_MSG_WARNING)
    {
        SendDiscordMessage(1, szMsgWithoutColor);
    }
    else if (type == ADMIN_MSG_INFO)
    {
        SendDiscordMessage(0, szMsgWithoutColor);
    }
    else
    {
        SendDiscordMessage(2, szMsgWithoutColor);
    }

    foreach(new i:GroupMember(AdminGroup[level - 1]))
	{
        SendClientMessage(i, color, out);
	}
    return;
}

stock SendAdminMsgF(E_ADMIN_MSG:type, level, const fmat[],  va_args<>)
{
	if (level < 1 || level > Y_PLAYER_ADMIN_MAX_LEVEL) {
		Logger_Err("invalid admin level", 
            Logger_I("level", level)
        );
		return;
	}

	new
		out[144];

	va_format(out, sizeof (out), fmat, va_start<3>);

    SendAdminMsg(type, level, out);
	return;
}

stock Admin_SetPlayerDuty(playerid, bool:toggle) {
    // TODO: Change colour when player is on duty.

    if (toggle) SetPlayerColor(playerid, Y_PLAYER_ADMIN_DUTY_COLOR);
    else SetPlayerColor(playerid, Y_RP_DEFAULT_PLAYER_COLOR);

    Bit_Set(adminOnDuty, playerid, toggle);
    return 1;
}

stock Admin_IsOnDuty(playerid)
    return Bit_Get(adminOnDuty, playerid);


hook OnPlayerSpawn(playerid)
{
    if (Admin_IsOnDuty(playerid)) SetPlayerColor(playerid, Y_PLAYER_ADMIN_DUTY_COLOR);
    else SetPlayerColor(playerid, Y_RP_DEFAULT_PLAYER_COLOR);
}