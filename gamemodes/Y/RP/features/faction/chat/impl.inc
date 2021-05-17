#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnGroupInit()
{
	GROUP_ADD<AllFactionGroup>
	{
		@YCMD:duty;
		@YCMD:uniform;
	}
}

YCMD:duty(playerid, params[], help)
{
	
	new 
		faction = INVALID_FACTION_ID,
		rank,
		Group:group
	;
	GetPlayerFactionMeta(playerid, .faction = faction, .group = group, .rank = rank);

	if (faction == INVALID_FACTION_ID) 
		return SendErrorMsg(playerid, "คุณไม่ได้อยู่ในแฟคชั่นใด ๆ");
    
	if (Faction.Meta[faction][@type] == GANG)
		return COMMAND_DENIED;

	new 
		pInt = GetPlayerInterior(playerid),
		pWorld = GetPlayerVirtualWorld(playerid),
		string[144 + 1]
	;

	foreach(new spawn:IFactionSpawn[faction]) {
		if (pWorld == Faction.Spawn[faction][spawn][@world] && pInt == Faction.Spawn[faction][spawn][@int] && IsPlayerInRangeOfPoint(playerid, 2.5, Faction.Spawn[faction][spawn][@x], Faction.Spawn[faction][spawn][@y], Faction.Spawn[faction][spawn][@z])) {

			switch(Faction.Meta[faction][@type]) {
				case POLICE: {

					PlayerDuty{playerid} = !PlayerDuty{playerid};

					if (PlayerDuty{playerid}) {

						ResetPlayerWeapons(playerid);

						GivePlayerWeapon(playerid, WEAPON_SPRAYCAN, 5000);
						GivePlayerWeapon(playerid, 3, 1);
						GivePlayerWeapon(playerid, 24, 150);
						SetPlayerArmour(playerid, 100.0);
						SetPlayerHealth(playerid, 100.0);
						SendMsgLocalF(playerid, 10.0, X11_PLUM3, "** %s หยิบตราและอาวุธจากล็อกเกอร์ของเขา", Player_GetCharacterName(playerid));

						format(string, sizeof string, "** HQ: %s %s ได้เริ่มปฏิบัติหน้าที่ในขณะนี้! **", Faction.Rank[faction][rank][@name], Player_GetCharacterName(playerid));
						foreach (new player:GroupMember(group)) 
						{
							SendClientMessage(player, X11_LIGHTSLATEBLUE, string);
						}
				
					} else {
	
						ResetPlayerWeapons(playerid);
						SetPlayerArmour(playerid, 0.0);
						SendMsgLocalF(playerid, 10.0, X11_PLUM3, "** %s เก็บตราและอาวุธเข้าล็อกเกอร์ของเขา", Player_GetCharacterName(playerid));

						format(string, sizeof string, "** HQ: %s %s ได้ออกจากการปฏิบัติหน้าที่ในขณะนี้! **", Faction.Rank[faction][rank][@name], Player_GetCharacterName(playerid));
						foreach (new player:GroupMember(group)) 
						{
							SendClientMessage(player, X11_LIGHTSLATEBLUE, string);
						}
					}

				}
				
				case MEDIC: {

					PlayerDuty{playerid} = !PlayerDuty{playerid};

					if (PlayerDuty{playerid}) {

						SetPlayerArmour(playerid, 50.0);
						SetPlayerHealth(playerid, 100.0);
						SendMsgLocalF(playerid, 10.0, X11_PLUM3, "** %s หยิบตราและอาวุธจากล็อกเกอร์ของเขา", Player_GetCharacterName(playerid));

						format(string, sizeof string, "** HQ: %s %s ได้เริ่มปฏิบัติหน้าที่ในขณะนี้! **", Faction.Rank[faction][rank][@name], Player_GetCharacterName(playerid));
						foreach (new player:GroupMember(group)) 
						{
							SendClientMessage(player, X11_LIGHTSLATEBLUE, string);
						}
				
					} else {
	
						SetPlayerArmour(playerid, 0.0);

						format(string, sizeof string, "** HQ: %s %s ได้ออกจากการปฏิบัติหน้าที่ในขณะนี้! **", Faction.Rank[faction][rank][@name], Player_GetCharacterName(playerid));
						foreach (new player:GroupMember(group)) 
						{
							SendClientMessage(player, X11_LIGHTSLATEBLUE, string);
						}
					}

				}
			}

			return 1;
		}
	}

	SendClientMessage(playerid, X11_GRAY71, "   คุณไม่ได้อยู่ในห้องล็อกเกอร์");

	return 1;
}

YCMD:uniform(playerid, params[], help)
{
		
	new 
		faction = INVALID_FACTION_ID,
		rank,
		Group:group
	;
	GetPlayerFactionMeta(playerid, .faction = faction, .group = group, .rank = rank);

	if (faction == INVALID_FACTION_ID)
		return SendErrorMsg(playerid, "คุณไม่ได้อยู่ในแฟคชั่นใด ๆ");
    
	if (Faction.Meta[faction][@type] == GANG)
		return COMMAND_DENIED;

	new 
		pInt = GetPlayerInterior(playerid),
		pWorld = GetPlayerVirtualWorld(playerid)
	;

	foreach(new spawn:IFactionSpawn[faction]) {
		if (pWorld == Faction.Spawn[faction][spawn][@world] && pInt == Faction.Spawn[faction][spawn][@int] && IsPlayerInRangeOfPoint(playerid, 2.5, Faction.Spawn[faction][spawn][@x], Faction.Spawn[faction][spawn][@y], Faction.Spawn[faction][spawn][@z])) {

			new skinid;
	
			switch(Faction.Meta[faction][@type]) {
				case POLICE: {

					if (sscanf(params, "d", skinid)) {
						SendSyntaxMsg(playerid, "/uniform <ไอดี>");
						SendClientMessage(playerid, X11_GRAY71, "0: ออกจากเครื่องแบบที่ปฏิบัติงาน");
						SendClientMessage(playerid, X11_GRAY71, "1-9: เครื่องแบบเจ้าหน้าที่\t\t10: เครื่องแบบมอเตอร์ไซค์");
						SendClientMessage(playerid, X11_GRAY71, "11: หน่วย SWAT\t\t12-14: เสื้อผ้าธรรมดา");
						return 1;
					}

					if (skinid < 0 || skinid > 14) return SendClientMessage(playerid, X11_INDIANRED, "ไอดีต้องไม่ต่ำกว่า 0 หรือมากกว่า 14!");

					switch(skinid)
					{
						case 0:
						{
							Player_SetSkin(playerid, Player_GetSkin(playerid));
						}
						case 1:
						{
							Player_SetSkin(playerid, 265);
						}
						case 2:
						{
							Player_SetSkin(playerid, 266);
						}
						case 3:
						{
							Player_SetSkin(playerid, 267);
						}
						case 4:
						{
							Player_SetSkin(playerid, 280);
						}
						case 5:
						{
							Player_SetSkin(playerid, 281);
						}
						case 6:
						{
							Player_SetSkin(playerid, 300);
						}
						case 7:
						{
							Player_SetSkin(playerid, 301);
						}
						case 8:
						{
							Player_SetSkin(playerid, 306);
						}
						case 9:
						{
							Player_SetSkin(playerid, 307);
						}
						case 10:
						{
							Player_SetSkin(playerid, 284);
						}
						case 11:
						{
							if (!PlayerSWAT{playerid})
								SendClientMessage(playerid, X11_INDIANRED, "คุณไม่ได้รับอนุญาต!");

							Player_SetSkin(playerid, 285);
						}
						case 12:
						{
							Player_SetSkin(playerid, 303);
						}
						case 13:
						{
							Player_SetSkin(playerid, 304);
						}
						case 14:
						{
							Player_SetSkin(playerid, 305);
						}
					}

				}
				case MEDIC: {

					if (sscanf(params, "d", skinid)) {
						SendSyntaxMsg(playerid, "/uniform <ไอดี>");
						SendClientMessage(playerid, X11_GRAY71, "0: Off-duty uniform ");
						SendClientMessage(playerid, X11_GRAY71, "1-7: Medic");
						return 1;
					}

					if (skinid < 0 || skinid > 7) return SendClientMessage(playerid, X11_INDIANRED, "ไอดีต้องไม่ต่ำกว่า 0 หรือมากกว่า 7!");

					switch(skinid)
					{
						case 0:
						{
							Player_SetSkin(playerid, Player_GetSkin(playerid));
						}
						case 1:
						{
							Player_SetSkin(playerid, 274);
						}
						case 2:
						{
							Player_SetSkin(playerid, 275);
						}
						case 3:
						{
							Player_SetSkin(playerid, 276);
						}
						case 4:
						{
							Player_SetSkin(playerid, 308);
						}
						case 5:
						{
							Player_SetSkin(playerid, 277);
						}
						case 6:
						{
							Player_SetSkin(playerid, 278);
						}
						case 7:
						{
							Player_SetSkin(playerid, 70);
						}
					}
				}
			}

			return 1;
		}
	}

	SendClientMessage(playerid, X11_GRAY71, "   คุณไม่ได้อยู่ในห้องล็อกเกอร์");

	return 1;
}