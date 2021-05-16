
#if defined _inc_impl
	#undef _inc_impl
#endif

stock RPizeName(name[], len = sizeof name)
{
	for (new i = 0; i != len; ++i) {
		if (name[i] == '_') {
			name[i] = ' ';
		}
	}
}

stock UnRPizeName(name[], len = sizeof name)
{
	for (new i = 0; i != len; ++i) {
		if (name[i] == ' ') {
			name[i] = '_';
		}
	}	
}

stock Player_FacePlayer(playerid, giveplayerid) {
    new
        Float: pX,
        Float: pY,
        Float: pZ,
        Float: gX,
        Float: gY,
        Float: gZ
    ;
    if(GetPlayerPos(playerid, pX, pY, pZ) && GetPlayerPos(giveplayerid, gX, gY, gZ)) {
        SetPlayerFacingAngle(playerid, (pX = -atan2((gX - pX), (gY - pY))));
        return SetPlayerFacingAngle(giveplayerid, (pX + 180.0));
    }
    return false;
}