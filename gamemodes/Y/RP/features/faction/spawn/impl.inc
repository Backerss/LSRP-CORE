#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnPlayerReadySpawn(playerid)
{
	if (IsPlayerInAnyFaction(playerid)) {
		new member = -1, faction = INVALID_FACTION_ID;
		GetPlayerFactionMeta(playerid, .faction = faction, .member = member);
		if (faction != INVALID_FACTION_ID && member != -1) {

			if (Faction.Meta[faction][@type] == GANG) {
				GivePlayerWeapon(playerid, WEAPON_SPRAYCAN, 9999);
			}

			foreach(new spawn:IFactionSpawn[faction]) {
				if (Faction.Member[faction][member][@spawn] == _:Faction.Spawn[faction][spawn][@id]) {
					SetPlayerPos(playerid, Faction.Spawn[faction][spawn][@x], Faction.Spawn[faction][spawn][@y], Faction.Spawn[faction][spawn][@z]);
					SetPlayerFacingAngle(playerid, Faction.Spawn[faction][spawn][@a]);
					SetPlayerInterior(playerid, Faction.Spawn[faction][spawn][@int]);
					SetPlayerVirtualWorld(playerid, Faction.Spawn[faction][spawn][@world]);
					return Y_HOOKS_BREAK_RETURN_1;
				}
			}
		}
	}
	return 1;
}

FactionAddSpawnDialog(playerid, factionid, bool:error = false)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			ListFactionSpawn(playerid, factionid);

			@return 1;
		}

		if (isnull(inputtext) || strlen(inputtext) > Y_RP_FACTION_MAX_SPAWN_NAME) {
			FactionAddSpawnDialog(playerid, factionid, true);			
			@return 1;
		}

		new 
			Float:pX, 
			Float:pY, 
			Float:pZ,
			Float:pA,
			pInt,
			pWorld,
			name[Y_RP_FACTION_MAX_SPAWN_NAME + 1],
			spawnid = Iter_Free(IFactionSpawn[factionid])
		;

		if (spawnid == INVALID_ITERATOR_SLOT) {
			@return SendErrorMsg(playerid, "����շ����ҧ����Ѻ�ش�Դ�����ῤ��蹹��");
		}

		strcpy(name, inputtext);

		GetPlayerPos(playerid, pX, pY, pZ);
		GetPlayerFacingAngle(playerid, pA);
		pInt = GetPlayerInterior(playerid);
		pWorld = GetPlayerVirtualWorld(playerid);
		
		inline FactionSpawnCreated()
		{
			if (!cache_affected_rows()) {
				Logger_Err("[rp:features:faction:spawn / FactionAddSpawnDialog] can't create faction spawn",
					Logger_I("faction name", Faction.Meta[factionid][@name]),
					Logger_I("faction index", factionid)
				);
				@return 1;
			}
			Iter_Add(IFactionSpawn[factionid], spawnid);
			Faction.Spawn[factionid][spawnid][@id] = DBID:cache_insert_id();
			strcpy(Faction.Spawn[factionid][spawnid][@name], name);
			Faction.Spawn[factionid][spawnid][@x] = pX;
			Faction.Spawn[factionid][spawnid][@y] = pY;
			Faction.Spawn[factionid][spawnid][@z] = pZ;
			Faction.Spawn[factionid][spawnid][@a] = pA;
			Faction.Spawn[factionid][spawnid][@int] = pInt;
			Faction.Spawn[factionid][spawnid][@world] = pWorld;
			ListFactionSpawn(playerid, factionid);
			@return 1;
		}

		new query[185 + Y_RP_FACTION_MAX_SPAWN_NAME + 1];
		mysql_format(MySQL_GetHandle(), query, sizeof query, "INSERT INTO "FACTION_SPAWN_TABLE"(faction_id,spawn_name,spawn_x,spawn_y,spawn_z,spawn_a,spawn_int,spawn_world) VALUES(%d,'%e',%f,%f,%f,%f,%i,%d)", _:Faction.Meta[factionid][@id], name, pX, pY, pZ, pA, pInt, pWorld);
		MySQL_PQueryInline(MySQL_GetHandle(), using inline FactionSpawnCreated, query);
	}

	new errorText[128];
	if (error) {
		format(errorText, sizeof errorText, ""INDIANRED"���ͨش�Դ��ͧ����繤����ҧ �դ�����������ҧ 1 ��� %d\n\n", Y_RP_FACTION_MAX_SPAWN_NAME);
	}

	new dialog_string[200];
	format(dialog_string, sizeof dialog_string, "%s"WHITE"��͹���ͨش�Դῤ��蹷���ͧ��ô�ҹ��ҧ���:", errorText);
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_INPUT, "�����ش�Դ����", dialog_string, "����", "��Ѻ");
 
	return 1;
}

ListFactionSpawn(playerid, factionid)
{
	new
		spawns[2048],
		addSpawnIndex = 0
	;

	format(spawns, sizeof spawns, "#\t����");

	foreach (new spawn:IFactionSpawn[factionid]) {
		
		new
			DBID:spawnDBID = Faction.Spawn[factionid][spawn][@id],
			spawnname[Y_RP_FACTION_MAX_SPAWN_NAME + 1]
		;
		addSpawnIndex++;
		strcpy(spawnname, Faction.Spawn[factionid][spawn][@name]);
		format(spawns, sizeof spawns, "%s\n�ʹ�: %d) %s", spawns, _:spawnDBID, spawnname);
	}

	if (Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) && Iter_Free(IFactionSpawn[factionid]) != INVALID_ITERATOR_SLOT) {
		format(spawns, sizeof spawns, "%s\n�����ش�Դ", spawns);
	}

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			InspectFaction(playerid, factionid);

			@return 1;
		}

		if (listitem == addSpawnIndex) {
			FactionAddSpawnDialog(playerid, factionid);
			@return 1;
		}


		new i;

		foreach (new spawn:IFactionSpawn[factionid]) {
			if (listitem == i++) {
				@return FactionInspectSpawn(playerid, factionid, spawn);
			}
		}

		@return 1;
	}

	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_TABLIST_HEADERS, "�ش�Դ�ͧῤ���", spawns, "��Ǩ�ͺ", "��Ѻ");
	return 1;
}

FactionInspectSpawn(playerid, factionid, spawn)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return ListFactionSpawn(playerid, factionid);
		}

		if (Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
			switch(listitem) {
				case 0: {
					@return RenameSpawn(playerid, factionid, spawn);
				}
				case 1: {
					@return ChangeSpawnPoint(playerid, factionid, spawn);
				}
				case 2: {
					@return RemoveSpawn(playerid, factionid, spawn);
				}
				default: {
					Logger_Err("[rp:features:faction:spawn / FactionInspectSpawn] Invalid listitem",
						Logger_P(playerid),
						Logger_I("listitem", listitem)
					);
				}
			}
		}

		@return SendErrorMsg(playerid, "ACCESS DENIED: "WHITE"�س������Ѻ͹حҵ");
	}
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_LIST, "��Ǩ�ͺ�ش�Դ", "��䢪���\n����¹�ش�Դ\nź", "���͡", "��Ѻ");
	return 1;
}

RenameSpawn(playerid, factionid, spawnid, bool:error = false)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			FactionInspectSpawn(playerid, factionid, spawnid);
			@return 1;
		}

		if (isnull(inputtext) || strlen(inputtext) > Y_RP_FACTION_MAX_SPAWN_NAME) {
			RenameSpawn(playerid, factionid, true);
			@return 1;
		}
	
		SendMsgF(playerid, X11_GRAY71, "�ش�Դῤ��蹶١����¹������ "LIGHTBLUE"%s", inputtext);

		strcpy(Faction.Spawn[factionid][spawnid][@name], inputtext);

		new query[64 + Y_RP_FACTION_MAX_SPAWN_NAME + 12 + 1];
		mysql_format(MySQL_GetHandle(), query, sizeof query, "UPDATE " FACTION_SPAWN_TABLE " SET spawn_name = '%e' WHERE id = %d", inputtext, _:Faction.Spawn[factionid][spawnid][@id]);
		mysql_tquery(MySQL_GetHandle(), query);

		FactionInspectSpawn(playerid, factionid, spawnid);
		@return 1;
	}
	
	new errorText[128];
	if (error) {
		format(errorText, sizeof errorText, ""FIREBRICK"���͵�ͧ���������ҧ "LIGHTBLUE"%d"FIREBRICK" ��� "LIGHTBLUE"%d"FIREBRICK" ����ѡ��\n\n", 1, Y_RP_FACTION_MAX_SPAWN_NAME);
	}

	new dialog_string[188 + Y_RP_FACTION_MAX_SPAWN_NAME + 1];
	format(dialog_string, sizeof dialog_string, "%s"WHITE"����¹���ͨش�Դῤ��� "LIGHTBLUE"%s"WHITE" �������ͷ���ͧ���", errorText, Faction.Spawn[factionid][spawnid][@name]);
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_INPUT, "��䢪��ͨش�Դῤ���", dialog_string, "�׹�ѹ", "��Ѻ");
	return 1;
}

ChangeSpawnPoint(playerid, factionid, spawnid)
{
	new query[200];

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext

		if (response) {
			SendClientMessage(playerid, X11_GRAY71, "�ش�Դῤ��蹶١����¹���ѧ���˹觻Ѩ�غѹ�ͧ�س");

			GetPlayerPos(playerid, Faction.Spawn[factionid][spawnid][@x], Faction.Spawn[factionid][spawnid][@y], Faction.Spawn[factionid][spawnid][@z]);
			GetPlayerFacingAngle(playerid, Faction.Spawn[factionid][spawnid][@a]);
			Faction.Spawn[factionid][spawnid][@int] = GetPlayerInterior(playerid);
			Faction.Spawn[factionid][spawnid][@world] = GetPlayerVirtualWorld(playerid);

			mysql_format(MySQL_GetHandle(), query, sizeof query, "UPDATE " FACTION_SPAWN_TABLE " SET spawn_x = %f, spawn_y = %f, spawn_z = %f, spawn_a = %f, spawn_int = %i, spawn_world = %d WHERE id = %d", Faction.Spawn[factionid][spawnid][@x], Faction.Spawn[factionid][spawnid][@y], Faction.Spawn[factionid][spawnid][@z], Faction.Spawn[factionid][spawnid][@a], Faction.Spawn[factionid][spawnid][@int], Faction.Spawn[factionid][spawnid][@world], _:Faction.Spawn[factionid][spawnid][@id]);
			mysql_tquery(MySQL_GetHandle(), query);
		}

		@return FactionInspectSpawn(playerid, factionid, spawnid);
	}

	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_MSGBOX, "����¹�ش�Դῤ���", ""WHITE"�س������ͷ��л�Ѻ�ش�Դ������ѧ���˹觻Ѩ�غѹ�ͧ�س", "��", "��Ѻ");
	return 1;
}

RemoveSpawn(playerid, factionid, spawnid)
{
	new query[128];

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			FactionInspectSpawn(playerid, factionid, spawnid);
			@return 1;
		}

		inline HandleDoorRemove()
		{
			if (!cache_affected_rows()) {
				Logger_Err("[rp:features:faction:spawn / RemoveSpawn] can't remove faction spawn",
					Logger_I("faction name", Faction.Meta[factionid][@name]),
					Logger_I("spawn index", spawnid)
				);
				@return 1;
			}

			memset(Faction.Spawn[factionid][spawnid][@name], 0);
			Iter_Remove(IFactionSpawn[factionid], spawnid);
			@return ListFactionSpawn(playerid, factionid);
		}
		mysql_format(MySQL_GetHandle(), query, sizeof query, "DELETE FROM " FACTION_SPAWN_TABLE" WHERE id = %d", _:Faction.Spawn[factionid][spawnid][@id]);
		MySQL_TQueryInline(MySQL_GetHandle(), using inline HandleDoorRemove, query);
		@return 1;
	}
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_MSGBOX, "ź�ش�Դῤ���", ""WHITE"�س������ͷ���ź�ش�Դῤ��蹹��\n\n"FIREBRICK"(��á�зӹ���������ö��͹��Ѻ��)", "�׹�ѹ", "��Ѻ");
	return 1;
}