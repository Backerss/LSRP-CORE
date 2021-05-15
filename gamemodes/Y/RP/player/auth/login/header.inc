#if defined _inc_header
	#undef _inc_header
#endif

#include <samp_bcrypt>
#include <YSI_Extra\y_inline_bcrypt>
#include <YSI_Players\y_groups>

#define MAX_PASSWORD_ATTEMPT (5)

new
	Group:LoggedGroup
;

#define Player_IsLoggedIn(%0) Group_GetPlayer(LoggedGroup, %0)