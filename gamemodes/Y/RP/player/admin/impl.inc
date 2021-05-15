#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    Statement:stmt_checkBan;

hook OnMySQLConnected() {
    static const checkQuery[] = "\
        SELECT \
            *,  DATE_FORMAT(ban_date,'%d %M %Y ���� %r') AS date_ban, \
            DATE_FORMAT(unban_date,'%d %M %Y ���� %r') AS date_unban, \
            TIMESTAMPDIFF(SECOND, CURRENT_TIMESTAMP(), unban_date) AS expired \
        FROM \
            bans \
        WHERE \
            username = ? OR ip = ? LIMIT 1\
    ";
    stmt_checkBan = MySQL_PrepareStatement(MySQL_GetHandle(), checkQuery);
}

Admin_CheckBanUser(playerid)
{
    new
        playerIP[20],
		playerName[MAX_PLAYER_NAME];
	
    GetPlayerIp(playerid, playerIP, sizeof(playerIP));
	GetPlayerName(playerid, playerName, sizeof(playerName));

    inline OnPlayerBanLoad() {
	
        new
            banIP[16 + 1],
            banName[MAX_PLAYER_NAME],
            banAdmin[MAX_PLAYER_NAME],
            banReason[MAX_BAN_REASON],
            banDate[40],
            unbanDate[40],
            expired;

        if (cache_num_rows()) 
		{
            cache_get_value_name(0, "username", banName, MAX_PLAYER_NAME);
            cache_get_value_name(0, "ip",  banIP, 20);
            cache_get_value_name(0, "admin", banAdmin, MAX_PLAYER_NAME);
            cache_get_value_name(0, "reason", banReason, MAX_BAN_REASON);

            cache_get_value_name(0, "date_ban", banDate, 40);
            cache_get_value_name(0, "date_unban", unbanDate, 40);

            cache_get_value_name_int(0, "expired", expired);

            if (expired <= 0) 
			{
                SendMsgF(playerid, X11_WHITE, "�س���Ѻ��ûŴẹ��������� "GREY"%s "WHITE"\n�Թ�յ�͹�Ѻ��á�Ѻ�� %s(%i) ���ҡ�зӼԴ��� � �ա", unbanDate, playerName, playerid);
                // Admin_RemoveBanUser(banName, INVALID_PLAYER_ID);
                CallLocalFunction("OnPlayerPassedBanCheck", "i", playerid);
            }
            else 
			{
                new string[512];
                format(string, sizeof(string), "\
                    "WHITE"���ͼ����: "GREY"%s\n\
                    "WHITE"�;շ��١ẹ: "GREY"%s\n\
                    "WHITE"�;ջѨ�غѹ: "GREY"%s\n\
                    "WHITE"ẹ��: "GREY"%s\n\
                    "WHITE"���˵آͧ���ẹ: "GREY"%s\n\
                    "WHITE"�ѹ���١ẹ: "GREY"%s\n\
                    "WHITE"�ѹ������Ѻ��ûŴẹ: "GREY"%s", banName, banIP, playerIP, banAdmin, banReason, banDate, unbanDate);
                Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "�����š��ẹ", string, "�Դ", "");
                Kick(playerid);
            }
        } 
		else 
		{
            CallLocalFunction("OnPlayerPassedBanCheck", "i", playerid);
        }
    }

    MySQL_Bind(stmt_checkBan, 0, playerName);
    MySQL_Bind(stmt_checkBan, 1, playerIP);

    MySQL_ExecuteParallel_Inline(stmt_checkBan, using inline OnPlayerBanLoad);
}