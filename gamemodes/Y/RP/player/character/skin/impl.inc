#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    Statement: stmt_updatePlayerSkin,
    Statement: stmt_loadPlayerSkin;

hook OnMySQLConnected() {
    stmt_updatePlayerSkin = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE character_stats SET skin = ? WHERE c_id = ?");
    stmt_loadPlayerSkin = MySQL_PrepareStatement(MySQL_GetHandle(), "SELECT skin FROM character_stats WHERE c_id = ?");
    return 1;
}

hook OnPlayerLogin(playerid) {
    inline const OnSkinLoad() {
        new
            playerSkin;

        MySQL_BindResultInt(stmt_loadPlayerSkin, 0, playerSkin);

        if(MySQL_Statement_FetchRow(stmt_loadPlayerSkin)) {
            Player_UpdateSkin(playerid, playerSkin);

            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:character:skin / OnPlayerLogin] skin loaded",
                Logger_I("skinid", playerSkin),
                Logger_P(playerid)
            );
        }
    }
    MySQL_BindInt(stmt_loadPlayerSkin, 0, Player_GetCharacterID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadPlayerSkin, using inline OnSkinLoad);
    return 1;
}

/** hooked */
hook OnPlayerFirstSpawn(playerid) {

    new
        skinid = Player_GetSkin(playerid);
    SetPlayerSkin(playerid, skinid);
    return 1;
}

hook OnPlayerChangeSkin(playerid, skinid, bool:save) {
    if(save) {
        MySQL_BindInt(stmt_updatePlayerSkin, 0, Player_GetSkin(playerid));
        MySQL_BindInt(stmt_updatePlayerSkin, 1, Player_GetCharacterID(playerid));
        MySQL_ExecuteParallel(stmt_updatePlayerSkin);
    }
}