#if defined _inc_impl
	#undef _inc_impl
#endif

EditFactionRanks(playerid, factionid)
{
	new ranks[1600];

	for (new i = 0; i != Y_RP_FACTION_RANK_LIMIT; ++i) {

     	new permString[250], bool:isMax = IsFlagged(Faction.Rank[factionid][i][@flags], MAX);
     	if (!isMax) {
     	    if (IsFlagged(Faction.Rank[factionid][i][@flags], RECRUIT)) {
     	        format(permString, sizeof permString, "%s%sรับสมาชิก|", strlen(permString) == 0 ? ("|") : (""), permString);
     	    }
     	    if (IsFlagged(Faction.Rank[factionid][i][@flags], KICK)) {
     	        format(permString, sizeof permString, "%s%sเตะสมาชิก|", strlen(permString) == 0 ? ("|") : (""), permString);
     	    }
     	    if (IsFlagged(Faction.Rank[factionid][i][@flags], PROMOTE)) {
     	        format(permString, sizeof permString, "%s%sเลื่อนขั้น|", strlen(permString) == 0 ? ("|") : (""), permString);
     	    }
     	    if (IsFlagged(Faction.Rank[factionid][i][@flags], EDITORS)) {
     	        format(permString, sizeof permString, "%s%sแก้ไขแฟคชั่น|", strlen(permString) == 0 ? ("|") : (""), permString);
     	    }
     	    if (IsFlagged(Faction.Rank[factionid][i][@flags], RESPAWN)) {
     	        format(permString, sizeof permString, "%s%sรียานพาหนะ|", strlen(permString) == 0 ? ("|") : (""), permString);
     	    }
     	    if (IsFlagged(Faction.Rank[factionid][i][@flags], PERMISSIONS)) {
     	        format(permString, sizeof permString, "%s%sแก้ไขสิทธิ์|", strlen(permString) == 0 ? ("|") : (""), permString);
     	    }
     	}
     	else {
     	    format(permString, sizeof permString, "เข้าถึงได้ทั้งหมด");
     	}
		format(ranks, sizeof ranks, "%s%d\t%s\t%s\n", ranks, i + 1, Faction.Rank[factionid][i][@name], permString);
    }

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return InspectFaction(playerid, factionid);
		}

		@return InspectFactionRank(playerid, factionid, listitem);
	}

	return Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_LIST, "จัดการยศ/ตำแหน่งแฟคชั่น", ranks, "เลือก", "กลับ");
}

InspectFactionRank(playerid, factionid, rank)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, inputtext
	
		if (!response) {
			@return EditFactionRanks(playerid, factionid);
		}
	
		if (listitem == E_FACTION_INSPECT_RANK_NAME) {
			@return EditFactionRankName(playerid, factionid, rank);
		}
		else if (listitem == E_FACTION_INSPECT_RANK_FLAGS) {
			if (!IsFactionAccess(playerid, factionid, PERMISSIONS) && !Group_GetPlayer(FactionLeaderGroup, playerid)) {
				SendErrorMsg(playerid, "ACCESS DENIED: "WHITE"คุณไม่ได้รับอนุญาต");
				@return EditFactionRanks(playerid, factionid);
			}
			@return EditFactionRankFlags(playerid, factionid, rank);
		}
	
		@return 1;
	}
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_LIST, "แก้ไขชื่อยศ/ตำแหน่ง", "เปลี่ยนชื่อ\nแก้ไขสิทธิ์", "เลือก", "กลับ");
	return 1;
}

EditFactionRankName(playerid, factionid, rank, error = 0)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return InspectFactionRank(playerid, factionid, rank);
		}

		if (isnull(inputtext)) {
			@return EditFactionRankName(playerid, factionid, rank, ERR_RANK_NAME_EMPTY);			
		}

		if (strlen(inputtext) > Y_RP_FACTION_MAX_RANK_NAME) {
			@return EditFactionRankName(playerid, factionid, rank, ERR_RANK_NAME_TOO_LONG);			
		}

		UpdateRankName(factionid, rank, inputtext);

		SendMsgF(playerid, X11_GRAY71, "เปลี่ยนชื่อยศ/ตำแหน่งเป็น "LIGHTBLUE"%s", inputtext);
		@return InspectFactionRank(playerid, factionid, rank);
	}

	new errorText[80];
	if (error == ERR_RANK_NAME_EMPTY) {
		format(errorText, sizeof errorText, ""FIREBRICK"ชื่อยศ/ตำแหน่งต้องไม่เป็นค่าว่าง\n\n");
	} else if (error == ERR_RANK_NAME_TOO_LONG) {
		format(errorText, sizeof errorText, ""FIREBRICK"ชื่อยศ/ตำแหน่งต้องมีความยาวไม่เกิน "LIGHTBLUE"%d"FIREBRICK" ตัวอักษร\n\n", Y_RP_FACTION_MAX_RANK_NAME);
	}

	new body[256];
	format(body, sizeof body, "%s"WHITE"เลือกชื่อใหม่ให้กับยศ/ตำแหน่ง "LIGHTBLUE"%s", errorText, Faction.Rank[factionid][rank][@name]);
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_INPUT, "แก้ไขชื่อยศ/ตำแหน่ง", body, "เลือก", "กลับ");
	return 1;
}

EditFactionRankFlags(playerid, factionid, rank)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, listitem, inputtext

		if (!response) {
			@return InspectFactionRank(playerid, factionid, rank);
		}

		switch(listitem) {
			case 0:  Faction.Rank[factionid][rank][@flags] ^= RECRUIT;
			case 1:  Faction.Rank[factionid][rank][@flags] ^= KICK;
			case 2:  Faction.Rank[factionid][rank][@flags] ^= PROMOTE;
			case 3:  Faction.Rank[factionid][rank][@flags] ^= EDITORS;
			case 4:  Faction.Rank[factionid][rank][@flags] ^= RESPAWN;
			case 5:  Faction.Rank[factionid][rank][@flags] ^= PERMISSIONS;
		}

		UpdateRankFlag(factionid, rank, Faction.Rank[factionid][rank][@flags]);
		@return EditFactionRankFlags(playerid, factionid, rank);
	}

	new body[300];
    format(body, sizeof body, "สิทธิ์\tสถานะ\n");
	format(body, sizeof body, "%sรับสมาชิก\t"WHITE"[%s"WHITE"]\n", body, !IsFlagged(Faction.Rank[factionid][rank][@flags], RECRUIT) ? ("  ") : (""LIGHTGREEN"X"));
	format(body, sizeof body, "%sเตะสมาชิก\t"WHITE"[%s"WHITE"]\n", body, !IsFlagged(Faction.Rank[factionid][rank][@flags], KICK) ? ("  ") : (""LIGHTGREEN"X"));
	format(body, sizeof body, "%sเลื่อนขั้น\t"WHITE"[%s"WHITE"]\n", body, !IsFlagged(Faction.Rank[factionid][rank][@flags], PROMOTE) ? ("  ") : (""LIGHTGREEN"X"));
	format(body, sizeof body, "%sแก้ไขแฟคชั่น\t"WHITE"[%s"WHITE"]\n", body, !IsFlagged(Faction.Rank[factionid][rank][@flags], EDITORS) ? ("  ") : (""LIGHTGREEN"X"));
	format(body, sizeof body, "%sรียานพาหนะ\t"WHITE"[%s"WHITE"]\n", body, !IsFlagged(Faction.Rank[factionid][rank][@flags], RESPAWN) ? ("  ") : (""LIGHTGREEN"X"));
	format(body, sizeof body, "%sแก้ไขสิทธิ์\t"WHITE"[%s"WHITE"]\n", body, !IsFlagged(Faction.Rank[factionid][rank][@flags], PERMISSIONS) ? ("  ") : (""LIGHTGREEN"X"));

	return Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_TABLIST_HEADERS, "กำหนดสิทธิ์ยศ/ตำแหน่ง", body, "สลับ", "กลับ");
}

UpdateRankName(factionid, rank, name[])
{
	strcpy(Faction.Rank[factionid][rank][@name], name);

	new query[128];
	mysql_format(MySQL_GetHandle(), query, sizeof query, "INSERT INTO " FACTION_RANK_TABLE " VALUES(%d, %d, '%e', %d) ON DUPLICATE KEY UPDATE name = '%e'", _:Faction.Meta[factionid][@id], rank, name, Faction.Rank[factionid][rank][@flags], name);
	mysql_tquery(MySQL_GetHandle(), query);
}


UpdateRankFlag(factionid, rank, flag)
{
	new query[128];
	mysql_format(MySQL_GetHandle(), query, sizeof query, "INSERT INTO " FACTION_RANK_TABLE " VALUES(%d, %d, '%e', %d) ON DUPLICATE KEY UPDATE flags = %d", _:Faction.Meta[factionid][@id], rank, Faction.Rank[factionid][rank][@name], flag, flag);
	mysql_tquery(MySQL_GetHandle(), query);
}