#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI_Players\y_groups>
#include <YSI_Server\y_colours>
#include <YSI_Visual\y_commands>
#include <YSI_Visual\y_dialog>
#include <YSI_Coding\y_inline>
#include <YSI_Extra\y_inline_mysql>

#define Y_PLAYER_ADMIN_MAX_LEVEL		5
#define Y_PLAYER_ADMIN_MANAGE_LEVEL     4
#define Y_PLAYER_ADMIN_LEAD_LEVEL       3
#define Y_PLAYER_ADMIN_STAFF_LEVEL      2

#define MAX_BAN_REASON                  32
#define PLAYER_ADMIN_TABLE				"admins"

new
	Group:DeveloperGroup,
	Group:AdminGroup[Y_PLAYER_ADMIN_MAX_LEVEL]
;

#define Admin_IsPlayerDev(%0) Group_GetPlayer(DeveloperGroup, %0)

stock Admin_GetRankName(playerid) {
    static const admin_RankNames[Y_PLAYER_ADMIN_MAX_LEVEL + 1][24] = {
        "ไม่มี",          
        "ผู้ช่วยแอดมิน",  	 
        "แอดมิน", 	  	
        "หัวหน้าแอดมิน",    
        "เจ้าของเซิร์ฟเวอร์",
		"ทีมพัฒนาระบบ"
    };

    return (Admin_IsPlayerDev(playerid) ? (admin_RankNames[Y_PLAYER_ADMIN_MAX_LEVEL]) : (admin_RankNames[Admin_GetPlayerLevel(playerid)]));
}