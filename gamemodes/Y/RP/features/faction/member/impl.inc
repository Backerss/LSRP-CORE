#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	AllFactionGroup = Group_Create("In faction");
	FactionLeaderGroup = Group_Create("Faction leader");
	for (new i = 0; i != Y_RP_FACTION_LIMIT; ++i) {
		new name[32];
		format(name, sizeof name, "Faction %d", i + 1);
		FactionGroup[i] = Group_Create(name);

		for (new j = 0; j != Y_RP_FACTION_RANK_LIMIT; ++j) {
			format(name, sizeof name, "Faction %d Rank %d", i + 1, j);
			FactionRankGroup[i][j] = Group_Create(name);

			if (j > 0) {
				Group_AddChild(FactionRankGroup[i][j], FactionRankGroup[i][j - 1]);
			}
		}
	}
	return 1;
}

hook OnPlayerLogin(playerid)
{
	new
		bool:breakOut = false,
		bool:inAnyGroup = false
	;

	foreach (new faction:IFaction) {

		new
			bool:isCurrentLeader = false
		;

		if (Faction.Meta[faction][@owner] == DBID:Player_GetCharacterID(playerid)) {
			if (inAnyGroup) {
				Logger_Err("[rp:features:faction:member / OnPlayerLogin] Multiple faction",
					Logger_P(playerid)
				);
				break;
			}

			Iter_Add(IFactionLeader, faction);
			Group_SetPlayer(FactionLeaderGroup, playerid, true);
			inAnyGroup = true;
			isCurrentLeader = true;
		}

		foreach (new member:IFactionMember[faction]) {
			if (Faction.Member[faction][member][@id] == DBID:Player_GetCharacterID(playerid)) {
				if (!isCurrentLeader && inAnyGroup) {

					Logger_Err("[rp:features:faction:member / OnPlayerLogin] Multiple faction",
						Logger_P(playerid)
					);
					
					breakOut = true;
					break;
				}

				Iter_Add(IFactionOnline[faction], member);
				Faction.Member[faction][member][@player] = playerid;
				Group_SetPlayer(FactionGroup[faction], playerid, true);
				inAnyGroup = true;
				break;
			}
		}

		if (breakOut) {
			break;
		}
	}

	if (inAnyGroup) {
		Group_SetPlayer(AllFactionGroup, playerid, true);
	}

	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
	foreach (new faction:IFaction) {
		foreach (new member:IFactionOnline[faction]) {
			if (Faction.Member[faction][member][@player] == playerid) {
				// No need for same remove as we break out the loop
				Iter_Remove(IFactionOnline[faction], member);
				Faction.Member[faction][member][@player] = INVALID_PLAYER_ID;
				break;
			}
		}
	}

	return 1;
}

ListFactionMembers(playerid, factionid)
{
	new
		members[2048],
		addMemberIndex = 0
	;

	format(members, sizeof members, "ชื่อ\tยศ/ตำแหน่ง\tออนไลน์");

	foreach (new member:IFactionMember[factionid]) {
		new
			DBID:playerDBid = Faction.Member[factionid][member][@id],
			playername[MAX_PLAYER_NAME + 1],
			rank = Faction.Member[factionid][member][@rank],
			online[16]
		;

		addMemberIndex++;

		strcpy(online, Iter_Contains(IFactionOnline[factionid], member) ? "ใช่" : "ไม่");
		strcpy(playername, Faction.Member[factionid][member][@name]);

		format(members, sizeof members, "%s\nไอดี: %d) %s\t%d\t%s", members, _:playerDBid, playername, rank + 1, online);
	}

	if (
		(
			Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) ||
			IsFactionAccess(playerid, factionid, RECRUIT)
		) &&
		Iter_Free(IFactionMember[factionid]) != INVALID_ITERATOR_SLOT
	) {
		format(members, sizeof members, "%s\nเพิ่มสมาชิกใหม่", members);
	}

	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			InspectFaction(playerid, factionid);

			@return 1;
		}

		if (listitem == addMemberIndex) {
			FactionAddMemberDialog(playerid, factionid);

			@return 1;
		}


		new i;

		foreach (new member:IFactionMember[factionid]) {
			if (listitem == i++) {
				@return FactionInspectMember(playerid, factionid, member);
			}
		}

		@return 1;
	}

	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_TABLIST_HEADERS, "สมาชิกของแฟคชั่น", members, "ตรวจสอบ", "กลับ");
	return 1;
}

GetPlayerFactionMeta(playerid, &Group:group = INVALID_GROUP, &faction = INVALID_FACTION_ID, &rank = 0, &member = -1)
{
	foreach (new factionid:IFaction) {
		foreach (new memberid:IFactionOnline[factionid]) {
	  	if (Faction.Member[factionid][memberid][@player] == playerid) {
	  		group = FactionGroup[factionid];
	  		faction = factionid;
			member = memberid;
	  		rank = Faction.Member[factionid][memberid][@rank];
	  		return;
	  	}
	  }
	}
}

IsFactionAccess(playerid, factionid, flag) {

	foreach (new memberid:IFactionMember[factionid]) {
		if (Faction.Member[factionid][memberid][@player] == playerid) {
			new rank = Faction.Member[factionid][memberid][@rank];
			return IsFlagged(Faction.Rank[factionid][rank][@flags], flag);
		}
	}
	return false;
}


FactionAddMemberDialog(playerid, factionid, error = 0)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			ListFactionMembers(playerid, factionid);

			@return 1;
		}

		if (isnull(inputtext) || strlen(inputtext) > MAX_PLAYER_NAME) {
			FactionAddMemberDialog(playerid, factionid, E_FACTION_INVALID_PLAYER);			

			@return 1;
		}

		new
			targetid = INVALID_PLAYER_ID,
			probableName[MAX_PLAYER_NAME + 1]
		;

		strcpy(probableName, inputtext);
		UnRPizeName(probableName);

		if (sscanf(probableName, "r", targetid) || targetid == INVALID_PLAYER_ID) {

			if (!Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL))
			{
				return FactionAddMemberDialog(playerid, factionid, E_FACTION_PLAYER_OFFLINE_INVITE);
			}

			inline Handler()
			{
				new 
					DBID:targetDBid = INVALID_DB_ID,
					memberAnywhere,
					leaderAnywhere
				;	

				if (!cache_num_rows()) {
					@return	FactionAddMemberDialog(playerid, factionid, E_FACTION_INVALID_PLAYER);
				}

				cache_get_value_name_int(0, "id", _:targetDBid);
				cache_get_value_name_int(0, "member", memberAnywhere);
				cache_get_value_name_int(0, "leader", leaderAnywhere);

				if (memberAnywhere || leaderAnywhere) {
					@return	FactionAddMemberDialog(playerid, factionid, E_FACTION_PLAYER_IN_ANY_FACTION);
				}

				RPizeName(probableName);

				if (!FactionAddMember(factionid, targetDBid, probableName)) {
					SendErrorMsg(playerid, "ไม่มีที่ว่างสำหรับสมาชิกใหม่ในแฟคชั่นนี้");
					@return ListFactionMembers(playerid, factionid);
				}

				SendMsgF(playerid, X11_GRAY71, "คุณได้เพิ่ม "LIGHTBLUE"%s"GRAY71" เข้าแฟคชั่น "LIGHTBLUE"%s", probableName, Faction.Meta[factionid][@name]);
				ListFactionMembers(playerid, factionid);
			}

			new query[203 + MAX_PLAYER_NAME + 1];
			mysql_format(MySQL_GetHandle(), query, sizeof query, "SELECT p.c_id, (SELECT COUNT(*) FROM " FACTION_MEMBER_TABLE " WHERE player_id = p.c_id) AS member, (SELECT COUNT(*) FROM " FACTION_TABLE " WHERE owner_id = p.c_id) AS leader FROM " CHARACTER_TABLE " p WHERE p.player_name = '%e'", probableName);
			MySQL_TQueryInline(MySQL_GetHandle(), using inline Handler, query);

			@return 1;
		}

        if(!Player_IsNearPlayer(playerid, targetid, 4.0) && !Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL)) 
            @return SendErrorMsg(playerid, "คุณไม่ได้อยู่ใกล้ผู้เล่นนั้น");

		if (IsPlayerInAnyFaction(targetid)) {
			@return FactionAddMemberDialog(playerid, factionid, E_FACTION_PLAYER_IN_ANY_FACTION);
		}

		foreach (new slot:IFactionOnline[factionid]) {
			if (Faction.Member[factionid][slot][@player] == targetid) {
				@return FactionAddMemberDialog(playerid, factionid, E_FACTION_PLAYER_ALREADY_ADDED);
			}
		}

		if (!FactionAddMember(factionid, .targetid = targetid)) {
			SendErrorMsg(playerid, "ไม่มีที่ว่างสำหรับสมาชิกใหม่ในแฟคชั่นนี้");

			@return ListFactionMembers(playerid, factionid);
		}

		SendMsgF(playerid, X11_GRAY71, "คุณได้เพิ่ม "LIGHTBLUE"%s"GRAY71" เข้าแฟคชั่น "LIGHTBLUE"%s", probableName, Faction.Meta[factionid][@name]);
		SendMsgF(targetid, X11_GRAY71, ""LIGHTBLUE"%s"GRAY71" ได้รับคุณเข้าแฟคชั่น "LIGHTBLUE"%s", probableName, Faction.Meta[factionid][@name]);
		ListFactionMembers(playerid, factionid);
		@return 1;
	}

	new errorText[64];
	if (error == E_FACTION_INVALID_PLAYER) {
		format(errorText, sizeof errorText, ""FIREBRICK"ไม่พบผู้เล่นที่ระบุ\n\n");
	} else if (error == E_FACTION_PLAYER_IN_ANY_FACTION) {
		format(errorText, sizeof errorText, ""FIREBRICK"ผู้เล่นนั้นได้อยู่ในแฟคชั่นอยู่แล้ว\n\n");
	} else if (error == E_FACTION_PLAYER_ALREADY_ADDED) {
		format(errorText, sizeof errorText, ""FIREBRICK"ผู้เล่นนั้นได้อยู่ในแฟคชั่นอยู่แล้ว\n\n");
	} else if (error == E_FACTION_PLAYER_OFFLINE_INVITE) {
		format(errorText, sizeof errorText, ""FIREBRICK"ฟังก์ชั่นเพิ่มสมาชิกแบบออฟไลน์นี้สำหรับแอดมินเท่านั้น\n\n");
	}

	new dialog_string[180];
	format(dialog_string, sizeof dialog_string, "%s"WHITE"โปรดเพิ่มสมาชิกใหม่ด้วยการป้อน "LIGHTBLUE"ไอดี"WHITE" หรือ "LIGHTBLUE"ชื่อ"WHITE" (หากออนไลน์), หรือ "LIGHTBLUE"ชื่อ"WHITE" หากออฟไลน์", errorText);
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_INPUT, "เพิ่มสมาชิกใหม่", dialog_string, "เลือก", "กลับ");
 
	return 1;
}

FactionAddMember(factionid, DBID:targetDBid = INVALID_DB_ID, const targetName[] = "", targetid = INVALID_PLAYER_ID)
{
	new slot = Iter_Free(IFactionMember[factionid]);
	if (slot == INVALID_ITERATOR_SLOT) return 0;

	Iter_Add(IFactionMember[factionid], slot);
	
	if (targetid != INVALID_PLAYER_ID) {
		Iter_Add(IFactionOnline[factionid], slot);
		Faction.Member[factionid][slot][@id] = DBID:Player_GetCharacterID(targetid);
		Faction.Member[factionid][slot][@player] = targetid;
		strcpy(Faction.Member[factionid][slot][@name], Player_GetCharacterName(targetid));

		Group_SetPlayer(AllFactionGroup, targetid, true);
		Group_SetPlayer(FactionGroup[factionid], targetid, true);

	} else {
		Faction.Member[factionid][slot][@id] = targetDBid;
		Faction.Member[factionid][slot][@player] = INVALID_PLAYER_ID;
		strcpy(Faction.Member[factionid][slot][@name], targetName);
	}

	Faction.Member[factionid][slot][@rank] = 0;

	new query[128];
	mysql_format(
		MySQL_GetHandle(),
		query,
		sizeof query,
		"INSERT INTO " FACTION_MEMBER_TABLE " VALUES (null, %d, %d, %d, null)",
		_:Faction.Meta[factionid][@id],
		_:Faction.Member[factionid][slot][@id],
		Faction.Member[factionid][slot][@rank]		
	);
	mysql_tquery(MySQL_GetHandle(), query);

	return 1;
}

FactionInspectMember(playerid, factionid, member)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			@return ListFactionMembers(playerid, factionid);
		}

		if (listitem == 0) {
			if (
				Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) ||
				IsFactionAccess(playerid, factionid, PROMOTE)
			) {
				@return FactionChangeMemberRankDialog(playerid, factionid, member);
			}

			SendErrorMsg(playerid, "ACCESS DENIED: "WHITE"คุณไม่ได้รับอนุญาต");

			@return FactionInspectMember(playerid, factionid, member);
		}

		if (
			Admin_IsPlayerAdminLevel(playerid, Y_RP_FACTION_ADMIN_MANAGE_LEVEL) ||
			(IsFactionAccess(playerid, factionid, KICK) && Faction.Meta[factionid][@owner] != Faction.Member[factionid][member][@id])
		) {
			@return FactionConfirmRemove(playerid, factionid, member);
		}
	
		@return SendErrorMsg(playerid, "ACCESS DENIED: "WHITE"คุณไม่ได้รับอนุญาต");
	}
	
	Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_LIST, "ตรวจสอบสมาชิก", "แก้ไขยศ/ตำแหน่ง\nเชิญออกจากแฟคชั่น", "เลือก", "กลับ");
	return 1;
}

FactionChangeMemberRankDialog(playerid, factionid, member, error = 0)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			FactionInspectMember(playerid, factionid, member);

			@return 1;
		}

		new rank;
		if (sscanf(inputtext, "d", rank) || !(1 <= rank <= Y_RP_FACTION_RANK_LIMIT)) {
			@return FactionChangeMemberRankDialog(playerid, factionid, member, 1);
		}

		rank -= 1;

		FactionChangeMemberRank(factionid, member, rank);
		SendMsgF(playerid, -1, "คุณได้เปลี่ยนยศของ "LIGHTBLUE"%s"WHITE" เป็นยศ/ตำแหน่ง "LIGHTBLUE"%s #%d", Faction.Member[factionid][member][@name], Faction.Rank[factionid][rank][@name], rank + 1);
		if (Iter_Contains(IFactionOnline[factionid], member)) {
			SendMsgF(playerid, X11_LIGHT_BLUE, "%s"WHITE" ได้เปลี่ยนยศ/ตำแหน่งของคุณเป็น "LIGHTBLUE"%s #%d", Player_GetCharacterName(playerid), Faction.Rank[factionid][rank][@name], rank + 1);
		}

		ListFactionMembers(playerid, factionid);

		@return 1;
	}

	new errorText[64];
	if (error == 1) {
		format(errorText, sizeof errorText, ""FIREBRICK"ยศ/ตำแหน่ง ไม่อยู่ในระยะที่ถูกต้อง\n\n");
	}
	
	new dialog_string[180];
	format(dialog_string, sizeof dialog_string, "%s"WHITE"เลือกยศ/ตำแหน่งให้สมาชิก "LIGHTBLUE"%s"WHITE" ระหว่าง "FIREBRICK"%d"WHITE" และ "FIREBRICK"%d", errorText, Faction.Member[factionid][member][@name], 1, Y_RP_FACTION_RANK_LIMIT);

	return Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_INPUT, "เปลี่ยนยศ/ตำแหน่งสมาชิก", dialog_string, "เลือก", "กลับ");
}

FactionChangeMemberRank(factionid, member, rank)
{
	Faction.Member[factionid][member][@rank] = rank;

	new query[128];
	mysql_format(MySQL_GetHandle(), query, sizeof query, "UPDATE " FACTION_MEMBER_TABLE " SET rankId = %d WHERE faction_id = %d AND player_id = %d", rank, _:Faction.Meta[factionid][@id], _:Faction.Member[factionid][member][@id]);
	mysql_tquery(MySQL_GetHandle(), query);

	return 1;
}

FactionConfirmRemove(playerid, factionid, member)
{
	inline Response(pid, dialogid, response, listitem, string:inputtext[])
	{
		#pragma unused pid, dialogid, response, listitem, inputtext
	
		if (!response) {
			FactionInspectMember(playerid, factionid, member);
			@return 1;
		}

		if (DBID:Player_GetCharacterID(playerid) && Faction.Member[factionid][member][@id]) {
			ListFactionMembers(playerid, factionid);
			@return SendErrorMsg(playerid, "คุณไม่สามารถเชิญตัวเองให้ออกจากแฟคชั่นได้ ลองใช้ /leavefaction");
		}

		FactionRemovePlayer(factionid, member);
		SendMsgF(playerid, -1, "คุณได้เชิญ "LIGHTBLUE"%s "WHITE"ออกจากแฟคชั่น "LIGHTBLUE"%s", Faction.Member[factionid][member][@name], Faction.Meta[factionid][@name]);
		if (Iter_Contains(IFactionOnline[factionid], member)) {
			SendMsgF(playerid, -1, "คุณถูกเชิญออกจากแฟคชั่น "LIGHTBLUE"%s "WHITE"โดย "LIGHTBLUE"%s", Faction.Member[factionid][member][@name], Player_GetCharacterName(playerid));
		}
	
		ListFactionMembers(playerid, factionid);

		@return 1;
	}
	new dialog_string[90];
	format(dialog_string, sizeof dialog_string, ""WHITE"โปรดยืนยันว่าคุณจะลบสมาชิกนี้ออกจากแฟคชั่น "LIGHTBLUE"%s", Faction.Member[factionid][member][@name]);
	return Dialog_ShowCallback(playerid, using inline Response, DIALOG_STYLE_MSGBOX, "คุณต้องการลบสมาชิกคนนี้?", dialog_string, "ใช่", "ไม่");
}

FactionRemovePlayer(factionid, member)
{
	Iter_Remove(IFactionMember[factionid], member);

	if (Iter_Contains(IFactionOnline[factionid], member)) {
		new playerid = Faction.Member[factionid][member][@player];
		Group_SetPlayer(FactionGroup[factionid], playerid, false);

		// 1 faction for only one character
		/*new bool:isInAnyGroup = false;

		foreach (new faction:IFaction) {
			if (Group_GetPlayer(FactionGroup[faction], playerid)) {
				isInAnyGroup = true;
				break;
			}
		}

		if (!isInAnyGroup) {
			Group_SetPlayer(AllFactionGroup, playerid, false);
		}*/
		Group_SetPlayer(AllFactionGroup, playerid, false);
	}

	Iter_Remove(IFactionOnline[factionid], member);

	new query[128];
	mysql_format(MySQL_GetHandle(), query, sizeof query, "DELETE FROM " FACTION_MEMBER_TABLE " WHERE faction_id = %d AND player_id = %d", _:Faction.Meta[factionid][@id], _:Faction.Member[factionid][member][@id]);
	mysql_tquery(MySQL_GetHandle(), query);
}