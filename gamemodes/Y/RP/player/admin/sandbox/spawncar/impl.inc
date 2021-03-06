#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
	AdminVehicleHandle[Y_ADMIN_MAX_SPAWNCAR] = { INVALID_VEHICLE_ID, ... },
	Iterator:AdminVehicleIndex<Y_ADMIN_MAX_SPAWNCAR>;

hook OnGroupInit()
{
	GROUP_ADD<AdminGroup[Y_PLAYER_ADMIN_MANAGE_LEVEL - 1]>
	{
		@YCMD:spawncar;
		@YCMD:despawncar;
		@YCMD:despawncars;
	}
	return 1;
}

YCMD:spawncar(playerid, params[], help)
{
	new 
		modelid, 
		color1, 
		color2, 
		siren
	; 
	
	if (sscanf(params, "iI(-1)I(-1)I(0)", modelid, color1, color2, siren))
	{
		SendSyntaxMsg(playerid, "/spawncar <�ʹ�����> <��1> <��2> <��ù ���������� 0>");
		SendClientMessage(playerid, X11_GRAY71, "������ҹ��˹Ъ��Ǥ��� ��ù��͡��͹حҵ���س�Դ��ù��������ö¹��"); 
		return 1;
	}
	
	if (modelid < 400 || modelid > 611)
		return SendErrorMsg(playerid, "�س�к����ŷ�����١��ͧ");
		
	if (color1 < -1 || color2 < -1 || color1 > 255 || color2 > 255)
		return SendErrorMsg(playerid, "�շ��س�к����١��ͧ"); 
	
	new slot = Iter_Alloc(AdminVehicleIndex);
	if (slot == ITER_NONE)
		return SendErrorMsg(playerid, "�������ö���ҧ�ҹ��˹����ҡ���ҹ������"); 

	new Float:pX, Float:pY, Float:pZ, Float:pA;
	GetPlayerPos(playerid, pX, pY, pZ);
	GetPlayerFacingAngle(playerid, pA);

	AdminVehicleHandle[slot] = CreateVehicle(modelid, pX, pY, pZ, pA, color1, color2, -1, siren);
	// TODO: �к�����ѹö
	// Vehicle_SetMaxFuelEx(AdminVehicleHandle[slot], modelid);
	Iter_Add(AdminVehicleIndex, slot);

	PutPlayerInVehicle(playerid, AdminVehicleHandle[slot], 0);

	SendAdminMsgF(ADMIN_MSG_INFO, 1, "%s ���ҧ�ҹ��˹� %s (%d)", Player_GetCharacterName(playerid), GetVehicleNameByModel(modelid), AdminVehicleHandle[slot]);

	return 1;
}

YCMD:despawncar(playerid, params[], help)
{
	new vehicleid; 
	
	if (sscanf(params, "i", vehicleid))
	{
		SendSyntaxMsg(playerid, "/despawncar <�ʹ��ҹ��˹�> (/dl)");
		SendClientMessage(playerid, X11_GRAY71, "���ʹ� 0 ����ź�ѹ���������");
		return 1;
	}

	if (IsPlayerInAnyVehicle(playerid) && vehicleid == 0)
	{
		vehicleid = GetPlayerVehicleID(playerid);
	}

	new index = -1;

	foreach(new i : AdminVehicleIndex)
	{
		if (AdminVehicleHandle[i] == vehicleid)
		{
			index = i;
			break;
		}
	}
	
	if (index == -1)
		return SendErrorMsg(playerid, "��˹Фѹ��������١���ҧ�Ҩҡ�ʹ�Թ");

	Iter_Remove(AdminVehicleIndex, index);
	DestroyVehicle(AdminVehicleHandle[index]);
	AdminVehicleHandle[index] = INVALID_VEHICLE_ID;
	return 1;
}

YCMD:despawncars(playerid, params[], help)
{
	if (Iter_Count(AdminVehicleIndex) == 0)
		return SendErrorMsg(playerid, "��辺��˹з��١���ҧ������ʹ�Թ");
	
	new count = 0;

	foreach(new i : AdminVehicleIndex)
	{
		DestroyVehicle(AdminVehicleHandle[i]);
		AdminVehicleHandle[i] = INVALID_VEHICLE_ID;

		Iter_SafeRemove(AdminVehicleIndex, i, i);
		count++;
	}

	SendAdminMsgF(ADMIN_MSG_INFO, 1, "��˹��ʹ�Թ������ %d ��١ź�� %s", count, Player_GetCharacterName(playerid));
	return 1;
}