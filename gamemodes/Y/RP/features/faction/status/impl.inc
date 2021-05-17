#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnPlayerDisconnect(playerid, reason)
{
	PlayerDuty{playerid} = false;
	PlayerSWAT{playerid} = false;
}

stock Player_IsFactionDuty(playerid)
{
	return PlayerDuty{playerid};
}