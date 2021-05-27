#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>


static 
	gsPlayerInfoText[MAX_PLAYERS][Y_PLAYER_MAX_INFO][Y_PLAYER_INFO_TEXT_LIMIT + 1],
	STREAMER_TAG_PICKUP:gsPlayerInfoPickup[MAX_PLAYERS][Y_PLAYER_MAX_INFO],
	Iterator:gsPlayerInfoIndex[MAX_PLAYERS]<Y_PLAYER_MAX_INFO>;

hook OnGroupInit()
{
	GROUP_ADD<CharacterSelectedGroup>
	{
		@YCMD:createinfo;
		@YCMD:delinfo;
	}
}

hook OnPlayerConnect(playerid)
{
	Iter_Clear(gsPlayerInfoIndex[playerid]);
}

hook OnPlayerDisconnect(playerid, reason)
{
	foreach(new slot : gsPlayerInfoIndex[playerid])
	{
		if (IsValidDynamicPickup(gsPlayerInfoPickup[playerid][slot]))
		{
			DestroyDynamicPickup(gsPlayerInfoPickup[playerid][slot]);
			gsPlayerInfoPickup[playerid][slot] = -1;
		}
		gsPlayerInfoText[playerid][slot][0] = '\0';

		new 
			cur = slot;
		
		Iter_SafeRemove(gsPlayerInfoIndex[playerid], cur, slot);
	}
}

YCMD:cim(playerid, params[], help) = createinfo;
YCMD:createinfo(playerid, params[], help)
{

    if(isnull(params) || strlen(params) > Y_PLAYER_INFO_TEXT_LIMIT) 
        return SendSyntaxMsg(playerid, "/createinfo <ข้อความ>");

	new slot = Iter_Alloc(gsPlayerInfoIndex[playerid]);
	if (slot == ITER_NONE)
	{
		SendErrorMsg(playerid, "ไม่สามารถสร้าง information ได้มากกว่านี้แล้ว ใช้ /delinfo เพื่อลบสักอันก่อน");
		return 1;
	}

	new 
		Float:x, 
		Float:y, 
		Float:z, 
		worldid = GetPlayerVirtualWorld(playerid), 
		interiorid = GetPlayerInterior(playerid);

	GetPlayerPos(playerid, x, y, z);

	gsPlayerInfoPickup[playerid][slot] = CreateDynamicPickup(1239, 22, x, y, z, worldid, interiorid);

	gsPlayerInfoText[playerid][slot][0] = '\0';
	strcpy(gsPlayerInfoText[playerid][slot], params, Y_PLAYER_INFO_TEXT_LIMIT);

	return 1;
}

YCMD:dim(playerid, params[], help) = delinfo;
YCMD:delinfo(playerid, params[], help)
{
	if(isnull(params)) 
	{
		SendSyntaxMsg(playerid, "/delinfo <ไอดี>");

		foreach(new slot : gsPlayerInfoIndex[playerid])
		{
			SendMsgF(playerid, X11_YELLOW, "ไอดี %d: %s", slot + 1, gsPlayerInfoText[playerid][slot]);
		}
		return 1;
	}

	new id = strval(params) - 1;

	if (!Iter_Contains(gsPlayerInfoIndex[playerid], id))
	{
		return SendErrorMsg(playerid, "ไม่พบไอดี information ที่ระบุ");
	}

	if (IsValidDynamicPickup(gsPlayerInfoPickup[playerid][id]))
	{
		DestroyDynamicPickup(gsPlayerInfoPickup[playerid][id]);
		gsPlayerInfoPickup[playerid][id] = -1;
	}

	gsPlayerInfoText[playerid][id][0] = '\0';
	Iter_Remove(gsPlayerInfoIndex[playerid], id);
	return 1;
}

hook OnPlayerPickUpDynPickup(playerid, STREAMER_TAG_PICKUP:pickupid)
{
	foreach(new i :GroupMember(CharacterSelectedGroup))
	{
		for(new slot = 0; slot != Y_PLAYER_MAX_INFO; slot++)
		{
			if (gsPlayerInfoPickup[playerid][slot] == pickupid)
			{
				SendMsgF(playerid, X11_YELLOW, "[information] %s", gsPlayerInfoText[playerid][slot]);
				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return 1;
}