#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    const Float:LS_SpawnPoints[][] = {
    {810.63520, -1340.0682, 13.5386, 37.33070},
    {1124.6071, -1427.5155, 15.7969, 350.9336},
    {585.81520, -1247.9160, 17.9521, 335.6035},
    {2025.2626, -1423.2682, 16.9922, 135.4516},
    {2509.2468, -1679.2029, 13.5469, 50.24740},
    {1457.1467, -1011.7307, 26.8438, 51.79910},
    {2017.8206, -1279.4851, 23.9820, 47.38920},
    {1935.7644, -1794.6068, 13.5469, 295.5515},
    {1371.4569, -1090.6387, 24.5459, 92.84640},
    {2298.4055, -1500.3264, 25.3047, 199.6940},
    {1178.0417, -1323.6000, 14.1005, 285.5701},
    {1757.44350, -1456.7, 13.5469, 282.4133}
};

static
    PlayerLastSpawn[MAX_PLAYERS],
    bool:PlayerFirstSpawn[MAX_PLAYERS] = true
;
hook OnPlayerConnect(playerid) {
    PlayerFirstSpawn[playerid] = true;
    PlayerLastSpawn[playerid] = -1;
    return 1;
}

hook OnPlayerSpawn(playerid) {
    // Execute this when player spawns for the first time.
    if(PlayerFirstSpawn[playerid]) {

        // Stop bg sound
        PlayerPlaySound(playerid, 1098, 0.0, 0.0, 0.0);
        SendClientMessage(playerid, -1, "ยินดีต้อนรับเข้าสู่ "INDIANRED""SERVER_NAME"");
        
        Logger_Dbg(SERVER_DEBUG_HANDLER, "[OnPlayerSpawn / character/spawn] calling OnPlayerFirstSpawn");
        CallLocalFunction("OnPlayerFirstSpawn", "i", playerid);
        PlayerFirstSpawn[playerid] = false;
    }

    new
        id = 0,
        Float: x,
        Float: y,
        Float: z,
        Float: ang;

    // random spawn ?
    if (id == 1)
    {
        id = random(sizeof(LS_SpawnPoints));

        if(id == PlayerLastSpawn[playerid]) {
            // regen new id
            id = random(sizeof(LS_SpawnPoints));
            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:character:spawn / OnPlayerSpawn] calling OnPlayerFirstSpawn",
                Logger_P(playerid),
                Logger_I("id", playerid)
            );
        }

        x = LS_SpawnPoints[id][0],
        y = LS_SpawnPoints[id][1],
        z = LS_SpawnPoints[id][2],
        ang = LS_SpawnPoints[id][3];

        PlayerLastSpawn[playerid] = id;
    
        SetPlayerPos(playerid, x, y, z);
        SetPlayerFacingAngle(playerid, ang);
        SetPlayerInterior(playerid, 0);
        SetPlayerVirtualWorld(playerid, 0);
        SetCameraBehindPlayer(playerid);

        Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:character:spawn / OnPlayerSpawn] player random spawned",
            Logger_P(playerid),
            Logger_I("id", playerid),
            Logger_F("x", x),
            Logger_F("y", y),
            Logger_F("z", z)
        );
        
    }
    else
    {
        x = 1642.2770,
        y = -2332.8931,
        z = -2.6797,
        ang = 0.0;

        SetPlayerPos(playerid, x, y, z);
        SetPlayerFacingAngle(playerid, ang);
        SetPlayerInterior(playerid, 0);
        SetPlayerVirtualWorld(playerid, 0);
        SetCameraBehindPlayer(playerid);

        Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:character:spawn / OnPlayerSpawn] player spawned",
            Logger_P(playerid),
            Logger_I("id", playerid),
            Logger_F("x", x),
            Logger_F("y", y),
            Logger_F("z", z)
        );
    }

    return 1;
}