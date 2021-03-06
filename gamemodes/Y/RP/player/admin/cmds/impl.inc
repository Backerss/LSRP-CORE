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
    SendClientMessage(playerid, X11_FORESTGREEN,"__________������ʹ�Թ__________");
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
        return SendSyntaxMsg(playerid, "/adminchat <��ͤ���>");

    SendAdminMsgF(E_ADMIN_MSG:0, 1, "(Admin) %s(%i): "WHITE"%s", Player_GetCharacterName(playerid), playerid, params);

    return 1;
}

YCMD:aduty(playerid, params[], help)
{
    new bool:toggleDuty = !Admin_IsOnDuty(playerid);

    Admin_SetPlayerDuty(playerid, toggleDuty);

	if (!toggleDuty) {
		SendClientMessage(playerid, X11_KHAKI, "�س���͡�ҡ��û�Ժѵ�˹�ҷ��������к�");
		foreach(new i :GroupMember(LoggedGroup)) {
            SendServerMsgF(i, "����ش��û�Ժѵ�˹�ҷ��ҡ����繼������к�", Player_GetCharacterName(playerid));
		}
	} 
    else 
    {
		SendClientMessage(playerid, X11_KHAKI, "�س���������Ժѵ�˹�ҷ��������к�");
		foreach(new i :GroupMember(LoggedGroup)) {
			SendServerMsgF(i, "�黯Ժѵ�˹�ҷ���繼������к�㹢�й��", Player_GetCharacterName(playerid));
		}
	}
    
    return 1;
}

YCMD:tempban(playerid, params[], help)
{
    if (isnull(params) || isnumeric(params))  {
        return SendSyntaxMsg(playerid, "/tempban <������͡: d(ay), m(onth), y(ear)>");
    }

    new
        targetid = INVALID_PLAYER_ID,
        reason[MAX_BAN_REASON] = '\0',
        duration = -1,
        bool:success = false,
        interval = -1;

    if (!strcmp(params, "d", true, 1)) {
        if (sscanf(params, "{s[2]}riS(����к�)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban d <playerid> <��������> <�˵ؼ�>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "�����蹹��������������͡Ѻ���������");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "�������ö��ẹ����ͧ��");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "�˵ؼ��դ�������Թ� �٧�ش������ "#MAX_BAN_REASON" ����ѡ��");
        }
        interval = 0;
    }
    if (!strcmp(params, "m", true, 1)) {
        if (sscanf(params, "{s[2]}riS(����к�)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban m <playerid> <��������> <�˵ؼ�>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "�����蹹��������������͡Ѻ���������");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "�������ö��ẹ����ͧ��");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "�˵ؼ��դ�������Թ� �٧�ش������ "#MAX_BAN_REASON" ����ѡ��");
        }

        interval = 1;
    }
    if (!strcmp(params, "y", true, 1))  {
        if (sscanf(params, "{s[2]}riS(����к�)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban y <playerid> <��������> <�˵ؼ�>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "�����蹹��������������͡Ѻ���������");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "�������ö��ẹ����ͧ��");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "�˵ؼ��դ�������Թ� �٧�ش������ "#MAX_BAN_REASON" ����ѡ��");
        }
        interval = 2;
    }

    if(interval != -1) {
        success = bool:Admin_AddBanUser(targetid, playerid, reason, interval, duration);
    }

    if(success) {

        if (!Character_IsLoggedIn(targetid))
        {
            SendAdminMsgF(ADMIN_MSG_INFO, 1, "�ѭ�ռ���� %s �١�ЧѺ�� %s ���ͧ�ҡ %s", Player_GetAccountName(targetid), Player_GetCharacterName(playerid), reason);
            SendMsgF(targetid, X11_INDIANRED, "�س�١ẹ�͡�ҡ������������ͧ�ҡ %s", reason);

            SendMsgAllF(X11_INDIANRED, "�ѭ�� %s �١ẹ�͡�ҡ������������ͧ�ҡ %s", Player_GetAccountName(targetid), reason);
        }
        else
        {
            SendAdminMsgF(ADMIN_MSG_INFO, 1, "�ѭ�ռ���� %s(%s) �١�ЧѺ�� %s ���ͧ�ҡ %s", Player_GetAccountName(targetid), Player_GetCharacterName(targetid), Player_GetCharacterName(playerid), reason);
            SendMsgF(targetid, X11_INDIANRED, "�س�١ẹ�͡�ҡ������������ͧ�ҡ %s", reason);

            SendMsgAllF(X11_INDIANRED, "[%d]%s (�ѭ��: %s) �١ẹ�͡�ҡ������������ͧ�ҡ %s", targetid, Player_GetCharacterName(targetid), Player_GetAccountName(targetid), targetid, reason);
        }
        Kick(targetid);
    }
    return 1;
}

YCMD:offban(playerid, params[], help)
{
    if (isnull(params) || isnumeric(params))  {
        return SendSyntaxMsg(playerid, "/offban <������͡: d(ay), m(onth), y(ear)>");
    }
    new
        targetname[MAX_PLAYER_NAME],
        reason[MAX_BAN_REASON] = '\0',
        duration = -1,
        bool:success = false,
        interval = -1;
    if (!strcmp(params, "d", true, 1)) {
        if (sscanf(params, "{s[2]}s[24]iS(����к�)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban d <playername> <��������> <�˵ؼ�>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "�˵ؼ��դ�������Թ� �٧�ش������ "#MAX_BAN_REASON" ����ѡ��");
        }
        interval = 0;
    }
    if (!strcmp(params, "m", true, 1)) {
        if (sscanf(params, "{s[2]}s[24]iS(����к�)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban m <playername> <��������> <�˵ؼ�>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "�˵ؼ��դ�������Թ� �٧�ش������ "#MAX_BAN_REASON" ����ѡ��");
        }

        interval = 1;
    }
    if (!strcmp(params, "y", true, 1))  {
        if (sscanf(params, "{s[2]}s[24]iS(����к�)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban y <playername> <��������> <�˵ؼ�>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "�˵ؼ��դ�������Թ� �٧�ش������ "#MAX_BAN_REASON" ����ѡ��");
        }
        interval = 2;
    }

    if(interval != -1) {
        success = !!Admin_AddOffBanUser(targetname, playerid, reason, interval, duration);
    }

    if(success) {
        SendAdminMsgF(ADMIN_MSG_INFO, 1, "[ADMIN]: �ѭ�բͧ %s �١�ЧѺ���ͧ�ҡ %s", targetname, reason);
        SendMsgAllF(X11_INDIANRED, "�ѭ�� %s �١ẹ�͡�ҡ���������Ẻ�Ϳ�Ź����ͧ�ҡ %s", targetname, reason);
    }
    return 1;
}

YCMD:unban(playerid, params[], help)
{
    if (isnull(params) || strval(params)) {
        return SendSyntaxMsg(playerid, "/unban <playername>");
    }
    if (strlen(params) > MAX_PLAYER_NAME) {
        return SendErrorMsg(playerid, "�ô�������������ա����!");
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

	if (sscanf(params, "rd", targetid, level)) return SendSyntaxMsg(playerid, "/sal <�ʹ�/���ͺҧ��ǹ> <�����>");
	if (targetid == playerid) return SendErrorMsg(playerid, "�س�������ö���á�зӹ��Ѻ����ͧ��");
	if (!IsPlayerConnected(targetid)) return SendErrorMsg(playerid, "��辺�����蹷���к�");

	if (Admin_GetPlayerLevel(playerid) <= Admin_GetPlayerLevel(targetid)) return SendSpecialMsg(playerid, X11_INDIANRED, "ACCESS DENIED", "�س������Ѻ͹حҵ");

	new sal = Admin_SetPlayerLevel(targetid, level);
	if (sal) {
		if (sal == 1) return SendMsgF(playerid, X11_INDIANRED, "�к���������١��ͧ ����ŷ���������������ҧ 1 ��� %d", Y_PLAYER_ADMIN_MAX_LEVEL);
		else if (sal == 2) return SendSpecialMsg(playerid, X11_INDIANRED, "ACCESS DENIED", "�س������Ѻ͹حҵ");
	}

	SendClientMessage(playerid, X11_KHAKI, "���Թ������������");
    SendAdminMsgF(ADMIN_MSG_WARNING, 1, "������ʹ�Թ�ѭ�� %s �١����¹������� %d �� %s", Player_GetAccountName(targetid), level, Player_GetCharacterName(playerid));
	return 1;
}

YCMD:givemoney(playerid, params[], help)
{
    new 
        money = 0, 
        targetid = INVALID_PLAYER_ID;
    
    if (sscanf(params, "rd", targetid, money)) {
        return SendSyntaxMsg(playerid, "/givemoney <playername> <�ӹǹ>");
    }

    if (targetid == INVALID_PLAYER_ID)
        return SendErrorMsg(playerid, "��辺�����蹷���к�");

    if (!Character_IsLoggedIn(targetid))
        return SendErrorMsg(playerid, "�����蹹���ѧ������������к�");

    new formatMoney[18];
    FormatMoney(money, formatMoney, sizeof(formatMoney));

    SendAdminMsgF(ADMIN_MSG_INFO, 1, "%s ���ʡ�Թ���Ѻ %s �ӹǹ %s", Player_GetCharacterName(playerid), Player_GetCharacterName(targetid), formatMoney);

    GivePlayerMoney(targetid, money);
    return 1;
}

YCMD:setmoney(playerid, params[], help)
{
    new 
        money = 0, 
        targetid = INVALID_PLAYER_ID;
    
    if (sscanf(params, "rd", targetid, money)) {
        return SendSyntaxMsg(playerid, "/setmoney <playername> <�ӹǹ>");
    }

    if (targetid == INVALID_PLAYER_ID)
        return SendErrorMsg(playerid, "��辺�����蹷���к�");

    if (!Character_IsLoggedIn(targetid))
        return SendErrorMsg(playerid, "�����蹹���ѧ������������к�");

    new formatMoney[18];
    FormatMoney(money, formatMoney, sizeof(formatMoney));

    SendAdminMsgF(ADMIN_MSG_INFO, 1, "%s �����Թ�ͧ %s �� %s", Player_GetCharacterName(playerid), Player_GetCharacterName(targetid), formatMoney);

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
        return SendSyntaxMsg(playerid, "/gopos <X> <Y> <Z> <int ���������� 0> <world ���������� 0>");

	SetPlayerInterior(playerid, int);
	SetPlayerVirtualWorld(playerid, vw);
	SetPlayerPos(playerid, pX, pY, pZ);
	return 1;
}