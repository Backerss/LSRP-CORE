#if defined _inc_header
	#undef _inc_header
#endif

enum E_FACTION_SPAWN {
    DBID:E_FACTION_Spawn_id,
    E_FACTION_Spawn_name[Y_RP_FACTION_MAX_SPAWN_NAME + 1],
    Float:E_FACTION_Spawn_x,
    Float:E_FACTION_Spawn_y,
    Float:E_FACTION_Spawn_z,
    Float:E_FACTION_Spawn_a,
    E_FACTION_Spawn_int,
    E_FACTION_Spawn_world,
    E_FACTION_Spawn_door
}

new FactionSpawn[Y_RP_FACTION_LIMIT][Y_RP_FACTION_SPAWN_LIMIT][E_FACTION_SPAWN];
new Iterator:IFactionSpawn[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_SPAWN_LIMIT>;
#define FACTION_SPAWN_TABLE "rp_faction_spawn"