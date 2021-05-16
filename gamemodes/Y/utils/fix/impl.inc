#if defined _inc_impl
	#undef _inc_impl
#endif

native KickxTimer(playerid) = Kick;
native BanxTimer(playerid) = Ban;

forward kickfix(playerid); public kickfix(playerid)
{
    KickxTimer(playerid);
}

forward banfix(playerid); public banfix(playerid){
    BanxTimer(playerid);
}