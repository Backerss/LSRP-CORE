#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    PlayerAttempts[MAX_PLAYERS];

hook OnGameModeInit()
{
	LoggedGroup = Group_Create("Logged in");
	return 1;
}

void:Account_PromptLogin(playerid, const password[], len = sizeof(password)) {

    new
        playerHash[62];
    // ������ local ����Ѻ y_inline
    strcpy(playerHash, password, len);

    inline const PromptLoginResponse(response, listitem, string:inputtext[]) {
         #pragma unused listitem

        if(!response) {
            Kick(playerid);
            return;
        }

        inline const OnPasswordVerify(bool: success) {
            if(!success) {
                PlayerAttempts[playerid] ++;
                if(PlayerAttempts[playerid] >= MAX_PASSWORD_ATTEMPT) {
                    SendErrorMsg(playerid, "�س�١���͡�ҡ������������ͧ�ҡ�������������к����¤���");
                    Kick(playerid);

                    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:auth:login / Account_PromptLogin] player has been kicked for too many login attempts",
                        Logger_P(playerid),
                        Logger_I("uid", Player_GetAccountID(playerid))
                    );
                    return;
                }

                Account_PromptLogin(playerid, playerHash);
                SendErrorMsg(playerid, "�������! �Դ��ͼԴ��Ҵ�ҧ���ҧ�ô�ͧ�����ա����");

                Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:auth:login / Account_PromptLogin] player failed to login",
                    Logger_I("attemps", PlayerAttempts[playerid]),
                    Logger_P(playerid),
                    Logger_I("uid", Player_GetAccountID(playerid))
                );
                return;
            }

            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:auth:login / Account_PromptLogin] player successfully logged in",
                Logger_P(playerid),
                Logger_I("uid", Player_GetAccountID(playerid))
            );

            PlayerAttempts[playerid] = 0;
            StampAccountLastLogin(playerid);
        }
        BCrypt_CheckInline(inputtext, playerHash, using inline OnPasswordVerify);
    }

    new
        string[93 + MAX_PLAYER_NAME + 32];

    static const LoginMessage[] = "\
        "WHITE"���ʴ� "GREY"%s "WHITE"�Թ�յ�͹�Ѻ��á�Ѻ�Ңͧ�س���� \
        \n\n\
        "WHITE"�ô��͡���ʼ�ҹ�ͧ�س��ҹ��ҧ��������������к�: \
    ";

    format(
        string,
        sizeof(string),
        LoginMessage,
        Player_GetAccountName(playerid),
        SERVER_NAME
    );

    Dialog_ShowCallback(
        playerid,
        using inline PromptLoginResponse,   // Handler
        DIALOG_STYLE_PASSWORD,              // Style
        SERVER_NAME,                        // Title
        string,                             // Content
        "�������к�",                         // Button Left
        "�͡��"                            // Button Right
    );
}

stock StampAccountLastLogin(playerid) {

    new
        Statement: stmt_updatePlayerLastLogin;

    stmt_updatePlayerLastLogin = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE accounts SET last_login = CURRENT_TIMESTAMP() WHERE u_id = ?");
    MySQL_BindInt(stmt_updatePlayerLastLogin, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_updatePlayerLastLogin);
    MySQL_StatementClose(stmt_updatePlayerLastLogin);
    Group_SetPlayer(LoggedGroup, playerid, true);

    CallLocalFunction("OnPlayerAccountLogin", "i", playerid);
    return 1;
}


 // API

stock Player_SetLoginStatus(playerid, bool:status) {
    return Group_SetPlayer(LoggedGroup, playerid, status);
}
