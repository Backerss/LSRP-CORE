#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

stock Boombox_Place(playerid)
{
	new
	    Float:angle;

	GetPlayerFacingAngle(playerid, angle);

	strpack(BoomboxData[playerid][boomboxURL], "", 128 char);
	GetPlayerPos(playerid, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2]);

	BoomboxData[playerid][boomboxPlaced] = true;
	BoomboxData[playerid][boomboxInterior] = GetPlayerInterior(playerid);
	BoomboxData[playerid][boomboxWorld] = GetPlayerVirtualWorld(playerid);

    BoomboxData[playerid][boomboxObject] = CreateDynamicObject(2226, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2] - 0.9, 0.0, 0.0, angle, BoomboxData[playerid][boomboxWorld], BoomboxData[playerid][boomboxInterior]);
    BoomboxData[playerid][boomboxText3D] = CreateDynamic3DTextLabel("[Boombox]\n{FFFFFF}/boombox เพื่อใช้งานเครื่องเล่น", 0xFFFFFF, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2] - 0.7, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, BoomboxData[playerid][boomboxWorld], BoomboxData[playerid][boomboxInterior]);

	return 1;
}

stock Boombox_Nearest(playerid)
{
	foreach (new i : Player) if (BoomboxData[i][boomboxPlaced] && GetPlayerInterior(playerid) == BoomboxData[i][boomboxInterior] && GetPlayerVirtualWorld(playerid) == BoomboxData[i][boomboxWorld] && IsPlayerInRangeOfPoint(playerid, 30.0, BoomboxData[i][boomboxPos][0], BoomboxData[i][boomboxPos][1], BoomboxData[i][boomboxPos][2])) {
     	return i;
	}
	return INVALID_PLAYER_ID;
}
forward Boombox_SetURL(playerid, url[]);
public Boombox_SetURL(playerid, url[])
{
	if (BoomboxData[playerid][boomboxPlaced])
	{
	    strpack(BoomboxData[playerid][boomboxURL], url);

	    foreach (new i : Player) if (boombox[i][pBoombox] == playerid) {
	        StopAudioStreamForPlayer(i);
	        PlayAudioStreamForPlayer(i, url, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2], 30.0, 1);
		}
	}
	return 1;
}

stock Boombox_Destroy(playerid)
{
	if (BoomboxData[playerid][boomboxPlaced])
	{
		if (IsValidDynamicObject(BoomboxData[playerid][boomboxObject]))
		    DestroyDynamicObject(BoomboxData[playerid][boomboxObject]);

		if (IsValidDynamic3DTextLabel(BoomboxData[playerid][boomboxText3D]))
		    DestroyDynamic3DTextLabel(BoomboxData[playerid][boomboxText3D]);

		foreach (new i : Player) if (boombox[i][pBoombox] == playerid) {
		    StopAudioStreamForPlayer(i);
		}
        BoomboxData[playerid][boomboxPlaced] = false;
        BoomboxData[playerid][boomboxInterior] = 0;
        BoomboxData[playerid][boomboxWorld] = 0;
	}
	return 1;
}


YCMD:boombox(playerid, params[], help)
{
	static
	    type[24],
	    string[128];

	if(boombox[playerid][pBoombox] == 0) 
		return SendErrorMsg(playerid, "คุณยังไม่มีboombox กรุณาไปซื้อที่ 24/7 ด้วยบทบาทที่สมควร");

	if (sscanf(params, "s[24]S()[128]", type, string))
	{
	    SendSyntaxMsg(playerid, "/boombox [name]");
	    SendSyntaxMsg(playerid, "[NAMES]:{FFFFFF} place, pickup, url");
	    return 1;
	}
	if (!strcmp(type, "place", true))
	{
	    if (BoomboxData[playerid][boomboxPlaced])
	        return SendErrorMsg(playerid, "คุณได้วาง boombox ไปแล้วนะ");

		if (Boombox_Nearest(playerid) != INVALID_PLAYER_ID)
		    return SendErrorMsg(playerid, "คุณอยู่ใกล้กับ boombox คนอื่นเกินไปที่จะสามารถวางได้");

		if (IsPlayerInAnyVehicle(playerid))
		    return SendErrorMsg(playerid, "คุณต้องลงจากรถก่อนถึงจะวางมันได้");

		Boombox_Place(playerid);

		SendMsgLocalF(playerid, 30.0, 0xFFFFFF, "** %s ได้หยิบ boombox ขึ้นมาและวางลง", Player_GetCharacterName(playerid));
		SendServerMsg(playerid, "คุณได้วางboombox แล้ว (ใช้ \"/boombox\" เพื่อใช้ตัวเลือกอื่นๆ).");
	}
	else if (!strcmp(type, "pickup", true))
	{
	    if (!BoomboxData[playerid][boomboxPlaced])
	        return SendErrorMsg(playerid, "คุณยังไม่ได้วางboombox ของคุณเลย");

		if (!IsPlayerInRangeOfPoint(playerid, 3.0, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2]))
		    return SendErrorMsg(playerid, "คุณไม่ได้อยู่ในระยะของ boombox ของคุณ");

		Boombox_Destroy(playerid);
		StopAudioStreamForPlayer(playerid);
		SendMsgLocalF(playerid, 30.0, 0xFFFFFF, "** %s ได้เก็บ boombox ขึ้นจากพื้น", Player_GetCharacterName(playerid));
	}
	else if (!strcmp(type, "url", true))
	{
	    if (sscanf(string, "s[128]", string))
	        return SendSyntaxMsg(playerid, "/boombox [url] [song url]");

        if (!BoomboxData[playerid][boomboxPlaced])
	        return SendErrorMsg(playerid, "คุณยังไม่ได้วางboombox ของคุณเลย");

		if (!IsPlayerInRangeOfPoint(playerid, 3.0, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2]))
		    return SendErrorMsg(playerid, "คุณไม่ได้อยู่ในระยะของ boombox ของคุณ");

		foreach (new i : Player)
		if (boombox[i][pBoombox] == 1)
		{
	        StopAudioStreamForPlayer(i);
	        PlayAudioStreamForPlayer(i, string, BoomboxData[playerid][boomboxPos][0], BoomboxData[playerid][boomboxPos][1], BoomboxData[playerid][boomboxPos][2], 30.0, 1);
		}
		SendMsgLocalF(playerid, 30.0, 0xFFFFFF, "** %s เปลี่ยนคลื่น boombox ของเขา", Player_GetCharacterName(playerid));
	}
	return 1;
}

YCMD:giveboombox(playerid, params[], help) 
{
	boombox[playerid][pBoombox] +=1;
	SendServerMsgF(playerid, "Admin %s ได้เสกboombox ให้ตัวเอง", Player_GetCharacterName(playerid));
	return 1;
}