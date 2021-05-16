#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid) {
    Bit_Set(PlayerSpawnState, playerid, false);
}

hook OnPlayerSpawn(playerid) {
    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[player:spawn / OnPlayerSpawn]");
    Bit_Set(PlayerSpawnState, playerid, true);
    Player_UpdateColour(playerid);
    return 1;
}

hook OnPlayerDeath(playerid, killerid, reason) {
    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[player:spawn / OnPlayerDeath]");
    Bit_Set(PlayerSpawnState, playerid, false);
    return 1;
}

stock Player_IsSpawned(playerid) {
    return Bit_Get(PlayerSpawnState, playerid);
}

stock Player_UpdateColour(playerid) {
	SetPlayerColor(playerid, Y_RP_DEFAULT_PLAYER_COLOR);
    return 1;
}