#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI_Players\y_groups>
#include <YSI_Visual\y_commands>
#include <YSI_Visual\y_dialog>

enum {
	E_FACTION_INVALID_PLAYER = 1,
	E_FACTION_PLAYER_IN_ANY_FACTION,
	E_FACTION_PLAYER_ALREADY_ADDED,
	E_FACTION_PLAYER_OFFLINE_INVITE
}

enum E_FACTION_MEMBER {
	DBID:E_FACTION_Member_id,
	E_FACTION_Member_player,
	E_FACTION_Member_name[MAX_PLAYER_NAME + 1],
	E_FACTION_Member_rank,
	E_FACTION_Member_spawn
}

new FactionMember[Y_RP_FACTION_LIMIT][Y_RP_FACTION_MEMBER_LIMIT][E_FACTION_MEMBER];
new Iterator:IFactionMember[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_MEMBER_LIMIT>;
new Iterator:IFactionOnline[Y_RP_FACTION_LIMIT]<Y_RP_FACTION_MEMBER_LIMIT>;
new Iterator:IFactionLeader<Y_RP_FACTION_LIMIT>;

new Group:AllFactionGroup;
new Group:FactionLeaderGroup;
new Group:FactionGroup[Y_RP_FACTION_LIMIT];
new Group:FactionRankGroup[Y_RP_FACTION_LIMIT][Y_RP_FACTION_RANK_LIMIT];

#define FACTION_MEMBER_TABLE "rp_faction_members"

#define IsFactionLeader(%0) Group_GetPlayer(FactionLeaderGroup, %0)