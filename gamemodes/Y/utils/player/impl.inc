#if defined _inc_impl
	#undef _inc_impl
#endif

stock GetXYInFrontOfPlayer(playerid, &Float:x, &Float:y, Float:distance)
{
	// Created by Y_Less

	new Float:a;

	GetPlayerPos(playerid, x, y, a);

	if (GetPlayerVehicleID(playerid)) {
	    GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
	}
	else GetPlayerFacingAngle(playerid, a);

	x += (distance * floatsin(-a, degrees));
	y += (distance * floatcos(-a, degrees));
}

stock Player_IsNearPlayer(playerid, targetid, Float:radius)
{
	new
        Float:x,
        Float:y,
        Float:z;

	GetPlayerPos(playerid, x, y, z);

    new
        matchingVW = GetPlayerVirtualWorld(playerid) == GetPlayerVirtualWorld(targetid),
        matchingInt = GetPlayerInterior(playerid) == GetPlayerInterior(targetid),
        inRange = IsPlayerInRangeOfPoint(targetid, radius, x, y, z);

	return matchingVW && matchingInt && inRange;
}


stock Player_RemoveWeaponID(playerid, weaponid)
{
	if (weaponid < 0 || weaponid > 50)
	    return;

	new 
		tempWeapon[13], 
		tempAmmo[13],
		currentWeapon = GetPlayerWeapon(playerid)
	;

	for(new slot = 0; slot < 13; slot++)
	    GetPlayerWeaponData(playerid, slot, tempWeapon[slot], tempAmmo[slot]);

	ResetPlayerWeapons(playerid);

	for(new slot; slot < 13; slot++)
	{
		if (tempWeapon[slot] == weaponid || tempAmmo[slot] <= 0)
			continue;

		GivePlayerWeapon(playerid, tempWeapon[slot], tempAmmo[slot]);
	}

	SetPlayerArmedWeapon(playerid, currentWeapon);
}

stock PlayerPlaySoundEx(playerid, sound, Float:range = 15.0)
{
	new
	    Float:x,
	    Float:y,
	    Float:z;

	GetPlayerPos(playerid, x, y, z);

    foreach(new i: StreamedPlayer[playerid])
    {
    	if (IsPlayerInRangeOfPoint(i, range, x, y, z)) {
		    PlayerPlaySound(i, sound, x, y, z);
        }
	}
	return 1;
}


/*Player_GetClosestPlayer(playerid, Float:range = 10000.0)
{
    new
        Float:x,
        Float:y,
        Float:z;

    GetPlayerPos(playerid, x, y, z);

    return GetClosestPlayerFromPoint(x, y, z, range, playerid);
}

GetClosestPlayerFromPoint(Float:x, Float:y, Float:z, &Float:lowestdistance = 10000.0, exceptionid = INVALID_PLAYER_ID)
{
    new
        Float:px,
        Float:py,
        Float:pz,
        Float:distance,
        closestplayer = -1;

    foreach(new i : Player)
    {
        if (i == exceptionid)
            continue;

        GetPlayerPos(i, px, py, pz);

        distance = Distance(px, py, pz, x, y, z);

        if (distance < lowestdistance)
        {
            lowestdistance = distance;
            closestplayer = i;
        }
    }
    return closestplayer;
}*/