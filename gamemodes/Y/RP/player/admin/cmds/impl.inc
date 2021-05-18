#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    Command_AddAlt(YCMD:setadminlevel, "sal");
	Command_AddAlt(YCMD:adminhelp, "ahelp");
    Command_AddAlt(YCMD:adminchat, "a");
}

hook OnGroupInit()
{
	GROUP_ADD<AdminGroup[0]>
	{
		@YCMD:adminhelp;
		@YCMD:ahelp;

        @YCMD:adminchat;
        @YCMD:a;

        @YCMD:aduty;
	}

	GROUP_ADD<AdminGroup[Y_PLAYER_ADMIN_STAFF_LEVEL - 1]>
	{
		@YCMD:tempban;

		@YCMD:offban;

        @YCMD:unban;

		@YCMD:givemoney;

		@YCMD:setmoney;  
	}

	GROUP_ADD<AdminGroup[Y_PLAYER_ADMIN_MANAGE_LEVEL - 1]>
	{
        @YCMD:gopos;
	}

	GROUP_ADD<AdminGroup[Y_PLAYER_ADMIN_MAX_LEVEL - 1]>
	{
		@YCMD:setadminlevel;
		@YCMD:sal;
	}
	return 1;
}

YCMD:adminhelp(playerid, params[], help)
{
    SendClientMessage(playerid, X11_FORESTGREEN,"__________คำสั่งแอดมิน__________");
    SendClientMessage(playerid, X11_SKYBLUE,"/ahelp /aduty");

    if (Admin_IsPlayerAdminLevel(playerid, Y_PLAYER_ADMIN_STAFF_LEVEL)) {
		SendClientMessage(playerid, X11_SKYBLUE,"/tempban /offban /unban /givemoney /setmoney");
	}
    
	if (Admin_IsPlayerAdminLevel(playerid, Y_PLAYER_ADMIN_MANAGE_LEVEL)) {

		SendClientMessage(playerid, X11_SKYBLUE,"/gopos /spawncar /despawncar /despawncars");
		SendClientMessage(playerid, X11_SKYBLUE, "/makehouse /gotohouse /gotohouseint");
		SendClientMessage(playerid, X11_SKYBLUE, "/createfaction /changefowner");
	}

	if (Admin_IsPlayerAdminLevel(playerid, Y_PLAYER_ADMIN_MAX_LEVEL)) {
		SendClientMessage(playerid, X11_SKYBLUE,"/setadminlevel");
	}

    return 1;
}

YCMD:adminchat(playerid, params[], help)
{

    if(isnull(params)) 
        return SendSyntaxMsg(playerid, "/adminchat <ข้อความ>");

    SendAdminMsgF(E_ADMIN_MSG:0, 1, "(Admin) %s(%i): "WHITE"%s", Player_GetCharacterName(playerid), playerid, params);

    return 1;
}

YCMD:aduty(playerid, params[], help)
{
    new bool:toggleDuty = !Admin_IsOnDuty(playerid);

    Admin_SetPlayerDuty(playerid, toggleDuty);

	if (!toggleDuty) {
		SendClientMessage(playerid, X11_KHAKI, "คุณได้ออกจากการปฏิบัติหน้าที่ผู้ดูแลระบบ");
		foreach(new i :GroupMember(LoggedGroup)) {
            SendServerMsgF(i, "ได้หยุดการปฏิบัติหน้าที่จากการเป็นผู้ดูแลระบบ", Player_GetCharacterName(playerid));
		}
	} 
    else 
    {
		SendClientMessage(playerid, X11_KHAKI, "คุณได้เริ่มปฏิบัติหน้าที่ผู้ดูแลระบบ");
		foreach(new i :GroupMember(LoggedGroup)) {
			SendServerMsgF(i, "ได้ปฏิบัติหน้าที่เป็นผู้ดูแลระบบในขณะนี้", Player_GetCharacterName(playerid));
		}
	}
    
    return 1;
}

YCMD:tempban(playerid, params[], help)
{
    if (isnull(params) || isnumeric(params))  {
        return SendSyntaxMsg(playerid, "/tempban <ตัวเลือก: d(ay), m(onth), y(ear)>");
    }

    new
        targetid = INVALID_PLAYER_ID,
        reason[MAX_BAN_REASON] = '\0',
        duration = -1,
        bool:success = false,
        interval = -1;

    if (!strcmp(params, "d", true, 1)) {
        if (sscanf(params, "{s[2]}riS(ไม่ระบุ)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban d <playerid> <ระยะเวลา> <เหตุผล>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "ผู้เล่นนั้นไม่ได้เชื่อมต่อกับเซิร์ฟเวอร์");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "ไม่สามารถใช้แบนตัวเองได้");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "เหตุผลมีความยาวเกินไป สูงสุดอยู่ที่ "#MAX_BAN_REASON" ตัวอักษร");
        }
        interval = 0;
    }
    if (!strcmp(params, "m", true, 1)) {
        if (sscanf(params, "{s[2]}riS(ไม่ระบุ)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban m <playerid> <ระยะเวลา> <เหตุผล>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "ผู้เล่นนั้นไม่ได้เชื่อมต่อกับเซิร์ฟเวอร์");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "ไม่สามารถใช้แบนตัวเองได้");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "เหตุผลมีความยาวเกินไป สูงสุดอยู่ที่ "#MAX_BAN_REASON" ตัวอักษร");
        }

        interval = 1;
    }
    if (!strcmp(params, "y", true, 1))  {
        if (sscanf(params, "{s[2]}riS(ไม่ระบุ)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban y <playerid> <ระยะเวลา> <เหตุผล>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "ผู้เล่นนั้นไม่ได้เชื่อมต่อกับเซิร์ฟเวอร์");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "ไม่สามารถใช้แบนตัวเองได้");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "เหตุผลมีความยาวเกินไป สูงสุดอยู่ที่ "#MAX_BAN_REASON" ตัวอักษร");
        }
        interval = 2;
    }

    if(interval != -1) {
        success = bool:Admin_AddBanUser(targetid, playerid, reason, interval, duration);
    }

    if(success) {

        if (!Character_IsLoggedIn(targetid))
        {
            SendAdminMsgF(ADMIN_MSG_INFO, 1, "บัญชีผู้ใช้ %s ถูกระงับโดย %s เนื่องจาก %s", Player_GetAccountName(targetid), Player_GetCharacterName(playerid), reason);
            SendMsgF(targetid, X11_INDIANRED, "คุณถูกแบนออกจากเซิร์ฟเวอร์เนื่องจาก %s", reason);

            SendMsgAllF(X11_INDIANRED, "บัญชี %s ถูกแบนออกจากเซิร์ฟเวอร์เนื่องจาก %s", Player_GetAccountName(targetid), reason);
        }
        else
        {
            SendAdminMsgF(ADMIN_MSG_INFO, 1, "บัญชีผู้ใช้ %s(%s) ถูกระงับโดย %s เนื่องจาก %s", Player_GetAccountName(targetid), Player_GetCharacterName(targetid), Player_GetCharacterName(playerid), reason);
            SendMsgF(targetid, X11_INDIANRED, "คุณถูกแบนออกจากเซิร์ฟเวอร์เนื่องจาก %s", reason);

            SendMsgAllF(X11_INDIANRED, "[%d]%s (บัญชี: %s) ถูกแบนออกจากเซิร์ฟเวอร์เนื่องจาก %s", targetid, Player_GetCharacterName(targetid), Player_GetAccountName(targetid), targetid, reason);
        }
        Kick(targetid);
    }
    return 1;
}

YCMD:offban(playerid, params[], help)
{
    if (isnull(params) || isnumeric(params))  {
        return SendSyntaxMsg(playerid, "/offban <ตัวเลือก: d(ay), m(onth), y(ear)>");
    }
    new
        targetname[MAX_PLAYER_NAME],
        reason[MAX_BAN_REASON] = '\0',
        duration = -1,
        bool:success = false,
        interval = -1;
    if (!strcmp(params, "d", true, 1)) {
        if (sscanf(params, "{s[2]}s[24]iS(ไม่ระบุ)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban d <playername> <ระยะเวลา> <เหตุผล>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "เหตุผลมีความยาวเกินไป สูงสุดอยู่ที่ "#MAX_BAN_REASON" ตัวอักษร");
        }
        interval = 0;
    }
    if (!strcmp(params, "m", true, 1)) {
        if (sscanf(params, "{s[2]}s[24]iS(ไม่ระบุ)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban m <playername> <ระยะเวลา> <เหตุผล>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "เหตุผลมีความยาวเกินไป สูงสุดอยู่ที่ "#MAX_BAN_REASON" ตัวอักษร");
        }

        interval = 1;
    }
    if (!strcmp(params, "y", true, 1))  {
        if (sscanf(params, "{s[2]}s[24]iS(ไม่ระบุ)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban y <playername> <ระยะเวลา> <เหตุผล>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "เหตุผลมีความยาวเกินไป สูงสุดอยู่ที่ "#MAX_BAN_REASON" ตัวอักษร");
        }
        interval = 2;
    }

    if(interval != -1) {
        success = !!Admin_AddOffBanUser(targetname, playerid, reason, interval, duration);
    }

    if(success) {
        SendAdminMsgF(ADMIN_MSG_INFO, 1, "[ADMIN]: บัญชีของ %s ถูกระงับเนื่องจาก %s", targetname, reason);
        SendMsgAllF(X11_INDIANRED, "บัญชี %s ถูกแบนออกจากเซิร์ฟเวอร์แบบออฟไลน์เนื่องจาก %s", targetname, reason);
    }
    return 1;
}

YCMD:unban(playerid, params[], help)
{
    if (isnull(params) || strval(params)) {
        return SendSyntaxMsg(playerid, "/unban <playername>");
    }
    if (strlen(params) > MAX_PLAYER_NAME) {
        return SendErrorMsg(playerid, "โปรดพิมพ์ชื่อใหม่อีกครั้ง!");
    }

    Admin_RemoveBanUser(params, playerid);
    return 1;
}


YCMD:setadminlevel(playerid, params[], help)
{
	new
		targetid,
		level
	;

	if (sscanf(params, "rd", targetid, level)) return SendSyntaxMsg(playerid, "/sal <ไอดี/ชื่อบางส่วน> <เลเวล>");
	if (targetid == playerid) return SendErrorMsg(playerid, "คุณไม่สามารถใช้การกระทำนี้กับตัวเองได้");
	if (!IsPlayerConnected(targetid)) return SendErrorMsg(playerid, "ไม่พบผู้เล่นที่ระบุ");

	if (Admin_GetPlayerLevel(playerid) <= Admin_GetPlayerLevel(targetid)) return SendSpecialMsg(playerid, X11_INDIANRED, "ACCESS DENIED", "คุณไม่ได้รับอนุญาต");

	new sal = Admin_SetPlayerLevel(targetid, level);
	if (sal) {
		if (sal == 1) return SendMsgF(playerid, X11_INDIANRED, "ระบุเลเวลไม่ถูกต้อง เลเวลที่ใช้ได้อยู่ระหว่าง 1 และ %d", Y_PLAYER_ADMIN_MAX_LEVEL);
		else if (sal == 2) return SendSpecialMsg(playerid, X11_INDIANRED, "ACCESS DENIED", "คุณไม่ได้รับอนุญาต");
	}

	SendClientMessage(playerid, X11_KHAKI, "ดำเนินการสำเร็จแล้ว");
    SendAdminMsgF(ADMIN_MSG_WARNING, 1, "เลเวลแอดมินบัญชี %s ถูกเปลี่ยนเป็นเลเวล %d โดย %s", Player_GetAccountName(targetid), level, Player_GetCharacterName(playerid));
	return 1;
}

YCMD:givemoney(playerid, params[], help)
{
    new 
        money = 0, 
        targetid = INVALID_PLAYER_ID;
    
    if (sscanf(params, "rd", targetid, money)) {
        return SendSyntaxMsg(playerid, "/givemoney <playername> <จำนวน>");
    }

    if (targetid == INVALID_PLAYER_ID)
        return SendErrorMsg(playerid, "ไม่พบผู้เล่นที่ระบุ");

    if (!Character_IsLoggedIn(targetid))
        return SendErrorMsg(playerid, "ผู้เล่นนั้นยังไม่ได้เข้าสู่ระบบ");

    new formatMoney[18];
    FormatMoney(money, formatMoney, sizeof(formatMoney));

    SendAdminMsgF(ADMIN_MSG_INFO, 1, "%s ได้เสกเงินให้กับ %s จำนวน %s", Player_GetCharacterName(playerid), Player_GetCharacterName(targetid), formatMoney);

    GivePlayerMoney(targetid, money);
    return 1;
}

YCMD:setmoney(playerid, params[], help)
{
    new 
        money = 0, 
        targetid = INVALID_PLAYER_ID;
    
    if (sscanf(params, "rd", targetid, money)) {
        return SendSyntaxMsg(playerid, "/setmoney <playername> <จำนวน>");
    }

    if (targetid == INVALID_PLAYER_ID)
        return SendErrorMsg(playerid, "ไม่พบผู้เล่นที่ระบุ");

    if (!Character_IsLoggedIn(targetid))
        return SendErrorMsg(playerid, "ผู้เล่นนั้นยังไม่ได้เข้าสู่ระบบ");

    new formatMoney[18];
    FormatMoney(money, formatMoney, sizeof(formatMoney));

    SendAdminMsgF(ADMIN_MSG_INFO, 1, "%s ได้ตั้งเงินของ %s เป็น %s", Player_GetCharacterName(playerid), Player_GetCharacterName(targetid), formatMoney);

    ResetPlayerMoney(targetid);
    GivePlayerMoney(targetid, money);
    return 1;
}

YCMD:gopos(playerid, params[], help)
{
	new 
		Float:pX, 
		Float:pY, 
		Float:pZ, 
		int,
		vw
	;

	if (sscanf(params, "fffI(0)I(0)", pX, pY, pZ, int, vw)) 
        return SendSyntaxMsg(playerid, "/gopos <X> <Y> <Z> <int ค่าเริ่มต้น 0> <world ค่าเริ่มต้น 0>");

	SetPlayerInterior(playerid, int);
	SetPlayerVirtualWorld(playerid, vw);
	SetPlayerPos(playerid, pX, pY, pZ);
	return 1;
}