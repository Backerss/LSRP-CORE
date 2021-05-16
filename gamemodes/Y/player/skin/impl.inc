#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    PlayerSkin[MAX_PLAYERS] = 0;

hook OnPlayerConnect(playerid) {
    PlayerSkin[playerid] = 0;
    return 1;
}

stock Player_SetSkin(playerid, skinid, bool:save = false) {

	PlayerSkin[playerid] = skinid;
    SetPlayerSkin(playerid, PlayerSkin[playerid]);
    CallLocalFunction("OnPlayerChangeSkin", "iib", playerid, skinid, save);
}

stock Player_UpdateSkin(playerid, skinid) {

	PlayerSkin[playerid] = skinid;
}

stock Player_GetSkin(playerid) {
	return PlayerSkin[playerid];
}