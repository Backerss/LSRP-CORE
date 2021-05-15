#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
	Player_UID[MAX_PLAYERS],
	Player_AccountName[MAX_PLAYERS][MAX_PLAYER_NAME + 1];

hook OnPlayerConnect(playerid)
{
	new
        playerName[MAX_PLAYER_NAME + 1];

    GetPlayerName(playerid, playerName, sizeof(playerName));
	Player_SetAccountName(playerid, playerName);

	Admin_CheckBanUser(playerid);
	return 1;
}

stock Player_SetAccountName(playerid, const name[], len = sizeof(name)) {
	Player_AccountName[playerid][0] = '\0';
    strcat(Player_AccountName[playerid], name, len);
}

stock Player_GetAccountName(playerid) {
    return Player_AccountName[playerid];
}

stock Player_SetAccountID(playerid, value) {
    return Player_UID[playerid] = value;
}

stock Player_GetAccountID(playerid) {
    return Player_UID[playerid];
}