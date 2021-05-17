#if defined _inc_header
	#undef _inc_header
#endif

enum {
	E_FACTION_INSPECT_RANK_NAME,
    E_FACTION_INSPECT_RANK_FLAGS
}

enum {
	ERR_RANK_NAME_EMPTY = 1,
	ERR_RANK_NAME_TOO_LONG
}

enum E_FACTION_RANK {
    E_FACTION_Rank_name[Y_RP_FACTION_MAX_RANK_NAME],
    E_FACTION_Rank_flags
}

new FactionRank[Y_RP_FACTION_LIMIT][Y_RP_FACTION_RANK_LIMIT][E_FACTION_RANK];

#define FACTION_RANK_TABLE "rp_faction_ranks"