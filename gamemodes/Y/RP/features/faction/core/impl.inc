#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	FactionPolice = Group_Create("Is Leo");
}

hook OnGroupInit()
{
	GROUP_ADD<AdminGroup[Y_PLAYER_ADMIN_MANAGE_LEVEL - 1]>
	{
		@YCMD:createfaction;
		@YCMD:changefowner;
	}

	GROUP_ADD<CharacterSelectedGroup>
	{
		@YCMD:factions;
		@YCMD:leavefaction;
	}
}

hook OnMySQLConnected()
{
	inline Handler()
	{
		new
			rows = cache_num_rows(),
			idx = INVALID_ITERATOR_SLOT,
			lastId = 0,
			lastMember,
			lastSpawn,
			lastRank = -1,
			midx,
			sidx
		;

		for (new row = 0; row != rows; ++row) {

			new id;
			cache_get_value_name_int(row, "id", id);

			if (id != lastId) {
				idx = Iter_Free(IFaction);
				if (idx == INVALID_ITERATOR_SLOT) {
					Logger_Err("[rp:features:faction:core] Faction exceeds the limit",
						Logger_S("faction", Faction.Meta[idx][@name]),
						Logger_I("num", rows),
						Logger_I("max", Y_RP_FACTION_LIMIT)
					);
					break;
				}
				Faction.Meta[idx][@id] = DBID:id;
				cache_get_value_name(row, "name", Faction.Meta[idx][@name]);
				cache_get_value_name_int(row, "creator_id", _:Faction.Meta[idx][@creator]);
				cache_get_value_name(row, "creator", Faction.Meta[idx][@creatorName]);
				cache_get_value_name_int(row, "owner_id", _:Faction.Meta[idx][@owner]);
				cache_get_value_name(row, "owner", Faction.Meta[idx][@ownerName]);
				cache_get_value_name_int(row, "color", Faction.Meta[idx][@color]);
				cache_get_value_name_int(row, "subtype", Faction.Meta[idx][@type]);
				cache_get_value_name(row, "initial", Faction.Meta[idx][@abbrv]);
						
				// Initial Faction Police Type
				for(new x = 0; x != sizeof(PoliceTypes); x++) {
					if (Faction.Meta[idx][@type] == PoliceTypes[x]) {
						Group_AddChild(FactionGroup[idx], FactionPolice);
						break;
					}
				}

				Iter_Add(IFaction, idx);
				lastId = id;
			}

			new
				bool:hasMember,
				bool:spawnMember,
				member
			;
			cache_is_value_name_null(row, "player_id", hasMember);
			hasMember = !hasMember;

			if (hasMember) {
				midx = Iter_Free(IFactionMember[idx]);
				
				cache_get_value_name_int(row, "player_id", member);

				if (lastMember != member) {
					lastMember = member;
					
					Faction.Member[idx][midx][@id] = DBID:member;
					cache_get_value_name_int(row, "player_id", _:Faction.Member[idx][midx][@id]);
					cache_get_value_name(row, "member_name", Faction.Member[idx][midx][@name]);
					cache_get_value_name_int(row, "rankId", Faction.Member[idx][midx][@rank]);

					cache_is_value_name_null(row, "faction_spawn", spawnMember);
					if (spawnMember) {
						Faction.Member[idx][midx][@spawn] = -1;
					} else {
						cache_get_value_name_int(row, "faction_spawn", Faction.Member[idx][midx][@spawn]);
					}
	
					Iter_Add(IFactionMember[idx], midx);
				}
			}

			new
				bool:hasRank,
				ridx
			;
			cache_is_value_name_null(row, "rank_meta", hasRank);
			hasRank = !hasRank;

			if (hasRank) {
				cache_get_value_name_int(row, "rank_meta", ridx);

				if (lastRank != ridx) {
					lastRank = ridx;

					if (!(0 <= ridx < Y_RP_FACTION_RANK_LIMIT)) {
						Logger_Err("[rp:features:faction:core] Faction rank exceeds the limit",
							Logger_S("faction", Faction.Meta[idx][@name]),
							Logger_I("index", ridx),
							Logger_I("max", Y_RP_FACTION_RANK_LIMIT)
						);
						continue;
					}

					cache_get_value_name(row, "rank_name", Faction.Rank[idx][ridx][@name]);
					cache_get_value_name_int(row, "rank_flags", Faction.Rank[idx][ridx][@flags]);
				}
			}

			new
				bool:hasSpawn,
				sindex
			;
			cache_is_value_name_null(row, "spawn_meta", hasSpawn);
			hasSpawn = !hasSpawn;

			if (hasSpawn) {
				
				sidx = Iter_Free(IFactionSpawn[idx]);

				cache_get_value_name_int(row, "spawn_meta", sindex);
				
				if (sidx == INVALID_ITERATOR_SLOT) {
					Logger_Err("[rp:features:faction:core] Faction spawn exceeds the limit",
						Logger_S("faction", Faction.Meta[idx][@name]),
						Logger_I("index", sindex),
						Logger_I("max", Y_RP_FACTION_SPAWN_LIMIT)
					);
					continue;
				}	
		
				if (lastSpawn != sindex) {
					lastSpawn = sindex;

					Faction.Spawn[idx][sidx][@id] = DBID:sindex;
					cache_get_value_name(row, "spawn_name", Faction.Spawn[idx][sidx][@name]);
					cache_get_value_name_float(row, "spawn_x", Faction.Spawn[idx][sidx][@x]);
					cache_get_value_name_float(row, "spawn_y", Faction.Spawn[idx][sidx][@y]);
					cache_get_value_name_float(row, "spawn_z", Faction.Spawn[idx][sidx][@z]);
					cache_get_value_name_float(row, "spawn_a", Faction.Spawn[idx][sidx][@a]);

					cache_get_value_name_int(row, "spawn_int", Faction.Spawn[idx][sidx][@int]);
					cache_get_value_name_int(row, "spawn_world", Faction.Spawn[idx][sidx][@world]);
					// cache_get_value_name_int(row, "spawn_door", Faction.Spawn[idx][sidx][@door]);

					Iter_Add(IFactionSpawn[idx], sidx);
				}
			}
		}

        Logger_Log(
            "Faction Loaded",
            Logger_I("total", Faction_GetTotal())
        );
	}

	new query[756];

	mysql_format(
		MySQL_GetHandle(),
		query,
		sizeof query,
		"SELECT f.*, \
		fm.player_id, fm.rankId, p3.player_name AS member_name, fm.spawn as faction_spawn, \
		fr.name AS rank_name, fr.id AS rank_meta, fr.flags AS rank_flags, \
		p1.player_name AS creator, \
		p2.player_name AS owner, \
		fs.id AS spawn_meta, fs.spawn_name, fs.spawn_x, fs.spawn_y, fs.spawn_z, fs.spawn_z, fs.spawn_a, fs.spawn_int, fs.spawn_world \
		FROM " FACTION_TABLE " f \
		LEFT JOIN " FACTION_MEMBER_TABLE " fm ON fm.faction_id = f.id \
		LEFT JOIN " FACTION_RANK_TABLE " fr ON fr.faction_id = f.id \
		LEFT JOIN " FACTION_SPAWN_TABLE " fs ON fs.faction_id = f.id \
		LEFT JOIN " CHARACTER_TABLE " p1 ON p1.c_id = f.creator_id \
		LEFT JOIN " CHARACTER_TABLE " p2 ON p2.c_id = f.owner_id \
		LEFT JOIN " CHARACTER_TABLE " p3 ON p3.c_id = fm.player_id \
		GROUP BY fm.id, fr.id, fs.id, f.id \
		ORDER BY f.id ASC, fm.id ASC, fr.id ASC, fs.id ASC");

	MySQL_PQueryInline(MySQL_GetHandle(), using inline Handler, query);
	return 1;
}

stock Faction_GetTotal()
{
	return Iter_Count(IFaction);
}

InspectFaction(playerid, factionid)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			ListFactions(playerid);

			@return 1;
		}

		new
			bool:authorizedName = false,
			bool:authorizedRanks = false,
			bool:authorizedRemove = false,
			bool:authorizedSpawn = false
		;

		if (Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
			authorizedName = true;
			authorizedRanks = true;
			authorizedRemove = true;
			authorizedSpawn = true;
		}

		if (IsFactionAccess(playerid, factionid, EDITORS | PROMOTE)) {
			authorizedRanks = true;
		}

		switch (listitem) {
			case E_FACTION_INSPECT_NAME: {
				if (authorizedName) {
					@return RenameFaction(playerid, factionid);
				}
			}
			case E_FACTION_INSPECT_MEMBERS: {
				@return ListFactionMembers(playerid, factionid);
			}
			case E_FACTION_INSPECT_REMOVE: {
				if (authorizedRemove) {
					@return RemoveFaction(playerid, factionid);
				}
			}
			case E_FACTION_INSPECT_SPAWN: {
				if (authorizedSpawn) {
					@return ListFactionSpawn(playerid, factionid);
				}
			}
			case E_FACTION_INSPECT_RANKS: {
				if (authorizedRanks) {
					@return EditFactionRanks(playerid, factionid);
				}
			}
			default: {
				Logger_Err("[rp:features:faction:core / InspectFaction] Invalid listitem",
					Logger_P(playerid),
					Logger_I("listitem", listitem)
				);
			}
		}

		SendErrorMsg(playerid, "ACCESS DENIED: "WHITE"คุณไม่ได้รับอนุญาต");
	
		@return InspectFaction(playerid, factionid);
	}

	if (Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
		Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_LIST, "เลือกสิ่งที่ต้องการทำ", "ชื่อ\nสมาชิก\nยศ/ตำแหน่ง\nจุดเกิด\nลบ", "เลือก", "กลับ");
	}
	else {
		Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_LIST, "เลือกสิ่งที่ต้องการทำ", "ชื่อ\nสมาชิก\nยศ/ตำแหน่ง", "เลือก", "กลับ");
	}
	return 1;
}

RemoveFaction(playerid, factionid)
{
	new query[60];

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
		if (!response) {
			@return InspectFaction(playerid, factionid);
		}

		inline DeleteFaction()
		{
			if (!cache_affected_rows()) {
				Logger_Err("[rp:features:faction:core / RemoveFaction] No affected rows",
					Logger_I("dbid", _:Faction.Meta[factionid][@id])
				);
				@return 1;
			}
			
			// clear member
			foreach (new member:IFactionMember[factionid]) {
				new
					cur = member;

				if (Iter_Contains(IFactionOnline[factionid], member)) {
					new kicked = Faction.Member[factionid][member][@player];
					Group_SetPlayer(FactionGroup[factionid], kicked, false);
					
					// 1 faction for only one character
					/*new bool:isInAnyGroup = false;
					foreach (new faction:IFaction) {
						if (Group_GetPlayer(FactionGroup[faction], kicked)) {
							isInAnyGroup = true;
							break;
						}
					}

					if (!isInAnyGroup) {
						Group_SetPlayer(AllFactionGroup, kicked, false);
					}*/
					Group_SetPlayer(AllFactionGroup, kicked, false);

					Iter_Remove(IFactionOnline[factionid], member);
					SendMsgF(kicked, X11_GRAY71, "   คุณถูกเตะออกจากแฟคชั่น %s", Faction.Meta[factionid][@name]);
				}
				
				Faction.Member[factionid][member][@id] = INVALID_DB_ID;
				memset(Faction.Member[factionid][member][@name], 0);
				Faction.Member[factionid][member][@rank] = 0;

				Iter_SafeRemove(IFactionMember[factionid], cur, member);
			}

			// clear rank
			for (new rank = 0; rank != Y_RP_FACTION_RANK_LIMIT; ++rank) {
				memset(Faction.Rank[factionid][rank][@name], 0);
				Faction.Rank[factionid][rank][@flags] = 0;
			}

			// clear spawn
			foreach (new spawn:IFactionSpawn[factionid]) {
				new
					cur = spawn;

				memset(Faction.Spawn[factionid][spawn][@name], 0);
				Faction.Spawn[factionid][spawn][@x] = 0.0;
				Faction.Spawn[factionid][spawn][@y] = 0.0;
				Faction.Spawn[factionid][spawn][@z] = 0.0;
				Faction.Spawn[factionid][spawn][@a] = 0.0;

				Iter_SafeRemove(IFactionSpawn[factionid], cur, spawn);
			}

			if (Group_IsChild(FactionGroup[factionid], FactionPolice))
				Group_RemoveChild(FactionGroup[factionid], FactionPolice);

			Faction.Meta[factionid][@color] = 0;
			Faction.Meta[factionid][@type] = 0;
			memset(Faction.Meta[factionid][@abbrv], 0);

			Iter_Remove(IFaction, factionid);
			
			Logger_Log("[rp:features:faction:core / RemoveFaction] Faction has been removed",
				Logger_I("faction index", factionid)
			);

			@return 1;
		}
		mysql_format(MySQL_GetHandle(), query, sizeof query, "DELETE FROM " FACTION_TABLE" WHERE id = %d", _:Faction.Meta[factionid][@id]);
		MySQL_TQueryInline(MySQL_GetHandle(), using inline DeleteFaction, query);
	}
	new dialog_string[Y_RP_FACTION_MAX_NAME + 1 + 90];
	format(dialog_string, sizeof dialog_string, "โปรดยืนยันที่จะลบแฟคชั่น "LIGHTBLUE"%s\n\n"INDIANRED"(การกระทำนี้ไม่สามารถย้อนกลับได้)", Faction.Meta[factionid][@name]);
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_MSGBOX, "การยืนยัน", dialog_string, "ยืนยัน", "กลับ");
	return 1;
}

RenameFaction(playerid, factionid, bool:error = false)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			InspectFaction(playerid, factionid);
			@return 1;
		}

		if (isnull(inputtext) || strlen(inputtext) > Y_RP_FACTION_MAX_NAME) {
			RenameFaction(playerid, factionid, true);
			@return 1;
		}
	
		SendMsgF(playerid, X11_GRAY71, "แฟคชั่นถูกเปลี่ยนชื่อเป็น "LIGHTBLUE"%s", inputtext);

		Logger_Log("[rp:features:faction:core / RenameFaction] Faction renamed",
			Logger_S("from", Faction.Meta[factionid][@name]),
			Logger_S("to", inputtext)
		);

		strcpy(Faction.Meta[factionid][@name], inputtext);

		new query[60 + Y_RP_FACTION_MAX_NAME + 12 + 1];
		mysql_format(MySQL_GetHandle(), query, sizeof query, "UPDATE " FACTION_TABLE " SET name = '%e' WHERE id = %d", inputtext, _:Faction.Meta[factionid][@id]);
		mysql_tquery(MySQL_GetHandle(), query);

		InspectFaction(playerid, factionid);
		@return 1;
	}
	
	new errorText[128];
	if (error) {
		format(errorText, sizeof errorText, ""FIREBRICK"ชื่อต้องอยู่ระหว่าง "LIGHTBLUE"%d"FIREBRICK" และ "LIGHTBLUE"%d"FIREBRICK" ตัวอักษร\n\n", 1, Y_RP_FACTION_MAX_NAME);
	}

	new dialog_string[188 + Y_RP_FACTION_MAX_NAME + 1];
	format(dialog_string, sizeof dialog_string, "%s"WHITE"เปลี่ยนชื่อแฟคชั่น "LIGHTBLUE"%s"WHITE" พิมพ์ชื่อที่ต้องการ", errorText, Faction.Meta[factionid][@name]);
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_INPUT, "แก้ไขชื่อแฟคชั่น", dialog_string, "ยืนยัน", "กลับ");
	return 1;
}

ListFactions(playerid)
{
	inline Noop(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	}

	if (Iter_Count(IFaction) == 0) {
		return Dialog_ShowCallback(playerid, using inline Noop, DIALOG_STYLE_MSGBOX, "ไม่พบแฟคชั่น", "ไม่พบแฟคชั่น", "โอเค", "ปิด");
	}

	new factions[2048];

	if (Admin_IsPlayerAdminLevel(playerid, 1)) {
		format(factions, sizeof factions, "#\tชื่อ\tผู้นำ\tออนไลน์");
	} else {
		format(factions, sizeof factions, "#\tชื่อ\tออนไลน์");
	}

	foreach(new faction:IFaction) {
		new
			id = _:Faction.Meta[faction][@id],
			numMemebers = Iter_Count(IFactionMember[faction]),
			numOnline = Iter_Count(IFactionOnline[faction]),
			name[Y_RP_FACTION_MAX_NAME],
			ownerName[MAX_PLAYER_NAME + 1]
		;

		strcpy(name, Faction.Meta[faction][@name]);
		strcpy(ownerName, Faction.Meta[faction][@ownerName]);

		if (Admin_IsPlayerAdminLevel(playerid, 1)) {
			format(factions, sizeof factions, "%s\n"SKYBLUE"%d\t"LIGHTBLUE"%s\t%s\t"LIGHTBLUE"["SKYBLUE"%d จาก %d"LIGHTBLUE"]", factions, id, name, ownerName, numOnline, numMemebers);
		} else {
			format(factions, sizeof factions, "%s\n"SKYBLUE"%d\t"LIGHTBLUE"%s\t"LIGHTBLUE"["SKYBLUE"%d จาก %d"LIGHTBLUE"]", factions, id, name, numOnline, numMemebers);				
		}
	}

	inline Inspect(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) 
			@return 1;
	
		new
			idx = 0,
			faction = INVALID_FACTION_ID
		;

		foreach (new factionid:IFaction) {
			if (idx++ == listitem) {
				faction = factionid;
			}
		}

		if (faction == INVALID_FACTION_ID) {
			Logger_Err("[rp:features:faction:core / ListFactions] Something went wrong while inspecting factions",
				Logger_P(playerid)
			);
			@return 1;
		}

		if (!IsFactionAccess(playerid, faction, EDITORS | RECRUIT | KICK | PROMOTE) && !Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
			SendErrorMsg(playerid, "ACCESS DENIED: "WHITE"คุณไม่ได้รับอนุญาต");
			@return 1;
		}

		InspectFaction(playerid, faction);
		@return 1;
	}

	if (!IsPlayerInAnyFaction(playerid) && !Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) {
		Dialog_ShowCallback(playerid, using inline Inspect, DIALOG_STYLE_TABLIST_HEADERS, "แฟคชั่น", factions, "<< ออก", "");
	} else {
		Dialog_ShowCallback(playerid, using inline Inspect, DIALOG_STYLE_TABLIST_HEADERS, "แฟคชั่น", factions, "ตรวจสอบ", "<< ออก");
	}
	return 1;
}

IsPlayerInAnyFaction(playerid)
{
	return !!Group_GetPlayer(AllFactionGroup, playerid);
}

stock Faction_GetType(factionid)
{
	return Faction.Meta[factionid][@type];
}

YCMD:factions(playerid, params[], help)
{
	ListFactions(playerid);
	return 1;
}

YCMD:changefowner(playerid, params[], help)
{
	new
		ownerid,
		id
	;

	if (sscanf(params, "rd", ownerid, id)) {
		SendErrorMsg(playerid, "การใช้: "WHITE"/changefowner <ไอดี/ชื่อบางส่วน> <ไอดีแฟคชั่น>");
		return 1;
	}

	if (ownerid == INVALID_PLAYER_ID) return SendClientMessage(playerid, X11_GRAY71, "   ไม่พบผู้เล่นที่ระบุ");
	if (IsPlayerInAnyFaction(ownerid) || IsFactionLeader(ownerid)) return SendErrorMsg(playerid, "ผู้เล่นนั้นมีแฟคชั่นอยู่แล้ว ๆ");

	new slot = INVALID_ITERATOR_SLOT;

	foreach (new faction:IFaction) {
		if (id == _:Faction.Meta[faction][@id]) {
			slot = faction;
			break;
		}
	}

	if (slot == INVALID_ITERATOR_SLOT) return SendClientMessage(playerid, X11_GRAY71, "   ไม่พบแฟคชั่นที่ระบุ");

	foreach (new member:IFactionMember[slot]) {

		// find faction owner and replace
		if (Faction.Meta[slot][@owner] == Faction.Member[slot][member][@id]) {

			if (Iter_Contains(IFactionOnline[slot], member)) {
				new kicked = Faction.Member[slot][member][@player];
				Group_SetPlayer(FactionGroup[slot], kicked, false);

				// 1 faction for only one character
				/*new bool:isInAnyGroup = false;
				foreach (new faction:IFaction) {
					if (Group_GetPlayer(FactionGroup[faction], kicked)) {
						isInAnyGroup = true;
						break;
					}
				}

				if (!isInAnyGroup) {
					Group_SetPlayer(AllFactionGroup, kicked, false);
				}*/
				Group_SetPlayer(AllFactionGroup, kicked, false);

				Iter_Remove(IFactionOnline[slot], member);
				SendMsgF(kicked, X11_GRAY71, "   คุณถูกเตะออกจากแฟคชั่น %s", Faction.Meta[slot][@name]);
			}
			
			Faction.Member[slot][member][@id] = INVALID_DB_ID;
			memset(Faction.Member[slot][member][@name], 0);
			Faction.Member[slot][member][@rank] = 0;

			Iter_Remove(IFactionMember[slot], member);
			
			break;
		}
	}

	Faction.Meta[slot][@owner] = DBID:Player_GetCharacterID(ownerid);
	strcpy(Faction.Meta[slot][@ownerName], Player_GetCharacterName(ownerid));
	Group_SetPlayer(FactionLeaderGroup, ownerid, true);
	Group_SetPlayer(AllFactionGroup, ownerid, true);
	FactionAddMember(slot, .targetid = ownerid);

	new query[80];
	mysql_format(MySQL_GetHandle(), query, sizeof query, "UPDATE " FACTION_TABLE " SET owner_id = %d WHERE id = %d", _:Faction.Meta[slot][@owner], _:Faction.Meta[slot][@id]);
	mysql_tquery(MySQL_GetHandle(), query);

	SendClientMessage(playerid, X11_KHAKI, "ดำเนินการเสร็จสมบูรณ์แล้ว");
	SendMsgF(ownerid, -1, "คุณได้เป็นเจ้าของแฟคชั่น "LIGHTBLUE"%s"WHITE" แล้ว", Faction.Meta[slot][@name]);
	return 1;
}

YCMD:createfaction(playerid, params[], help)
{
	new
		ownerid,
		type,
		name[Y_RP_FACTION_MAX_NAME + 1]
	;

	if (sscanf(params, "rds[" #Y_RP_FACTION_MAX_NAME "]", ownerid, type, name)) {
		SendErrorMsg(playerid, "การใช้: "WHITE"/createfaction <ไอดี/ชื่อบางส่วน> <ประเภท> <ชื่อแฟคชั่น>");
		SendClientMessage(playerid, X11_GRAY71, "ประเภท: 1-Police, 2-Medic, 3-Gang, 4-SASD");
		return 1;
	}
	if (ownerid == INVALID_PLAYER_ID) return SendClientMessage(playerid, X11_GRAY71, "   ไม่พบผู้เล่นที่ระบุ");
	if (IsPlayerInAnyFaction(ownerid)) return SendErrorMsg(playerid, "ผู้เล่นนั้นมีแฟคชั่นอยู่แล้ว");
		
	new slot = Iter_Free(IFaction);
	if (slot == INVALID_ITERATOR_SLOT) return SendErrorMsg(playerid, "มีแฟคชั่นถึงจำนวนสูงสุดแล้ว");

	inline FactionCreated()
	{
		if (!cache_affected_rows()) {
			Logger_Err("[rp:features:faction:core / CMD:createfaction] Can't create faction",
				Logger_P(playerid)
			);
			@return 1;
		}

		Iter_Add(IFaction, slot);
		Faction.Meta[slot][@id] = DBID:cache_insert_id();
		Faction.Meta[slot][@type] = type;
		Faction.Meta[slot][@owner] = DBID:Player_GetCharacterID(ownerid);
		Faction.Meta[slot][@creator] = DBID:Player_GetCharacterID(playerid);

		strcpy(Faction.Meta[slot][@ownerName], Player_GetCharacterName(ownerid));
		strcpy(Faction.Meta[slot][@name], name);

		strcpy(Faction.Meta[slot][@creatorName], Player_GetCharacterName(playerid));
		
		Group_SetPlayer(FactionLeaderGroup, ownerid, true);
		Group_SetPlayer(AllFactionGroup, ownerid, true);
	
		for(new x = 0; x != sizeof(PoliceTypes); x++) {
			if (Faction.Meta[slot][@type] == PoliceTypes[x]) {
				Group_AddChild(FactionGroup[slot], FactionPolice);
				break;
			}
		}

		FactionAddMember(slot, .targetid = ownerid);
	}

	new query[80 + 32 + Y_RP_FACTION_MAX_NAME + 1];
	mysql_format(MySQL_GetHandle(), query, sizeof query, "INSERT INTO "FACTION_TABLE"(creator_id, owner_id, subtype, name) VALUES(%d, %d, %d, '%e')", _:Player_GetCharacterID(playerid), _:Player_GetCharacterID(ownerid), type, name);
	MySQL_PQueryInline(MySQL_GetHandle(), using inline FactionCreated, query);

	SendClientMessage(playerid, X11_KHAKI, "ดำเนินการเสร็จสมบูรณ์แล้ว");
	SendMsgF(ownerid, -1, "คุณได้เป็นเจ้าของแฟคชั่น "LIGHTBLUE"%s"WHITE" แล้ว", name);
	return 1;
}

YCMD:leavefaction(playerid, params[], help)
{
	if (!IsPlayerInAnyFaction(playerid)) return SendErrorMsg(playerid, "คุณไม่ได้อยู่ในแฟคชั่นใด ๆ");
	if (IsFactionLeader(playerid)) return SendErrorMsg(playerid, "คุณไม่สามารถออกจากแฟคชั่นที่คุณเป็นผู้นำได้");

	new factionid = INVALID_FACTION_ID, member = -1;
	GetPlayerFactionMeta(playerid, .faction = factionid, .member = member);

	if (factionid == INVALID_FACTION_ID || member == - 1)
	{
		Logger_Err("[rp:features:faction:core / CMD:leavefaction] Can't leave faction",
			Logger_P(playerid)
		);
	}

	inline LeaveFaction(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
		if (!response) @return 1;

		FactionRemovePlayer(factionid, member);
		SendClientMessage(playerid, X11_GRAY71, "   คุณได้ออกจากแฟคชั่นเรียบร้อยแล้ว");

		Logger_Log("[rp:features:faction:core / CMD:leavefaction] Player leave faction",
			Logger_P(playerid),
			Logger_S("faction name", Faction.Meta[factionid][@name])
		);

		@return 1;
	}
	new dialog_string[Y_RP_FACTION_MAX_NAME + 1 + 90];
	format(dialog_string, sizeof dialog_string, "โปรดยืนยันที่จะออกจากแฟคชั่น "LIGHTBLUE"%s\n\n"INDIANRED"(การกระทำนี้ไม่สามารถย้อนกลับได้)", Faction.Meta[factionid][@name]);
	Dialog_ShowCallback(playerid, using inline LeaveFaction, DIALOG_STYLE_MSGBOX, "การยืนยัน", dialog_string, "ยืนยัน", "ไม่");

	return 1;
}

stock Faction_IsPlayerLEO(playerid)
{
	return Group_GetPlayer(FactionPolice, playerid);
}