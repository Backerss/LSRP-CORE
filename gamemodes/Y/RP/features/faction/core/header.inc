#if defined _inc_header
	#undef _inc_header
#endif

#define Faction. OO_TYPE(FACTION,Faction)

#include <YSI_Players\y_groups>
#include <YSI_Visual\y_commands>
#include <YSI_Visual\y_dialog>
#include <YSI_Core\y_utils>

#define INVALID_FACTION_ID 	-1

#define	Y_RP_FACTION_LIMIT				        20
#define Y_RP_FACTION_RANK_LIMIT			        20
#define Y_RP_FACTION_SPAWN_LIMIT 			    3
#define Y_RP_FACTION_MAX_NAME			        32
#define Y_RP_FACTION_MAX_INITIAL_NAME	        16
#define Y_RP_FACTION_MAX_SPAWN_NAME 		    64
#define Y_RP_FACTION_MAX_RANK_NAME 				24
#define Y_RP_FACTION_MEMBER_LIMIT               50
#define Y_RP_FACTION_ADMIN_MANAGE_LEVEL 	    4
#define Y_RP_FACTION_VEHICLE_LIMIT              20

#define FACTION_TABLE "rp_factions"

enum {
    POLICE = 1,
    MEDIC,
    GANG,
	SASD
}

new const PoliceTypes[] = { POLICE, SASD };

enum {
	E_FACTION_INSPECT_NAME,
	E_FACTION_INSPECT_MEMBERS,
	E_FACTION_INSPECT_RANKS,
    E_FACTION_INSPECT_SPAWN,
    E_FACTION_INSPECT_REMOVE
}

enum {
    MIN = 0,            // ไม่มีความสามารถใด ๆ
    RECRUIT = 1,        // รับเข้าแฟคชั่น
    KICK = 2,           // เตะออกจากแฟคชั่น  
    PROMOTE = 4,        // เลื่อนขั้นในแฟคชั่น    ; manage member rank
    EDITORS = 8,        // แก้ไขตั้งค่าแฟคชั่น    ; manage faction rank / add vehicle to faction
    RESPAWN = 16,       // รียานพาหนะ        ; respawn faction vehicle
    PERMISSIONS = 32,   // แก้ไขสิทธิ์          ; edit faction rank flags
    MAX = 63            // ตำแหน่งสูงสุดทำได้ทุกอย่าง
}

enum E_FACTION_DATA {
    DBID:E_FACTION_Meta_id,
    E_FACTION_Meta_name[Y_RP_FACTION_MAX_NAME + 1],
    E_FACTION_Meta_abbrv[Y_RP_FACTION_MAX_INITIAL_NAME + 1],
    E_FACTION_Meta_type,
    E_FACTION_Meta_color,
	DBID:E_FACTION_Meta_creator,
	E_FACTION_Meta_creatorName[MAX_PLAYER_NAME + 1],
	E_FACTION_Meta_ownerName[MAX_PLAYER_NAME + 1],
    DBID:E_FACTION_Meta_owner
}

new FactionMeta[Y_RP_FACTION_LIMIT][E_FACTION_DATA];
new Iterator:IFaction<Y_RP_FACTION_LIMIT>;

new Group:FactionPolice;