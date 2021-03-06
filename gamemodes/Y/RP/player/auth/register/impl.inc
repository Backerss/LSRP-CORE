#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    Statement:stmt_insertPlayerAccount;

hook OnMySQLConnected() {
    stmt_insertPlayerAccount = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO accounts (username, password) VALUES (?, ?)");
}

// Prompts the player the register dialog.
// return n/a
stock Account_PromptRegister(playerid) {
    inline const _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, listitem
        if (!response) {
            Kick(playerid);
            return;
        }
        Account_Create(playerid, inputtext);
    }

    new
        string[245 + MAX_PLAYER_NAME + 1 + 112];

    static
        const RegisterMessage[] = "\
        "WHITE"���ʴ� "GREY"%s "YELLOW"(�ѧ�����ŧ����¹)"WHITE" \
        \n \
        "WHITE"** �ô�������� ���ͷ����ŧ����¹���駹�� ���������͵���Фâͧ�س \
        \n\n \
        "WHITE"�ô��͡���ʼ�ҹ�ͧ�س��ҹ��ҧ�������ŧ����¹: \
        \n\n \
        "WHITE"- ���ʼ�ҹ��ͧ "ORANGE"\"��������Ţ\" "WHITE"������ \
        \n \
        "WHITE"- ���ʼ�ҹ��ͧ�դ�����ǵ���� "ORANGE"3 "WHITE"�֧ "ORANGE"20 "WHITE"����ѡ�� \
    ";

    format(string, sizeof(string), RegisterMessage, Player_GetAccountName(playerid), SERVER_NAME);

    Dialog_ShowCallback(playerid,
        using inline _response,
        DIALOG_STYLE_PASSWORD,
        SERVER_NAME,
        string,
        "ŧ����¹",
        "�͡��");
}

static Account_Create(playerid, const password[]) {

    if (!(3 <= strlen(password) <= 20)) {
        SendErrorMsg(playerid, "������Ǣͧ���ʼ�ҹ���١��ͧ �ѹ������������ҧ 3-20 ����ѡ��");
        Account_PromptRegister(playerid);
        return;
    }
    if (isnumeric(password)) {
        SendErrorMsg(playerid, "���ʼ�ҹ�ͧ�س���١��ͧ ���ʼ�ҹ����յ���ѡ������������");
        Account_PromptRegister(playerid);
        return;
    }

    inline const OnPasswordHash(string:hash[]) {
        Account_InsertToDatabase(playerid, hash);
    }
    BCrypt_HashInline(password, BCRYPT_COST, using inline OnPasswordHash);
}

static Account_InsertToDatabase(playerid, const hash[]) {
    inline const OnRegister() {
        Player_SetAccountID(playerid, cache_insert_id());

        Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:auth:register / Account_InsertToDatabase] player auto logged",
            Logger_P(playerid),
            Logger_I("uid", Player_GetAccountID(playerid))
        );

        StampAccountLastLogin(playerid);
    }

    new
        username[MAX_PLAYER_NAME];
    GetPlayerName(playerid, username, sizeof(username));

    // insert account data.
    MySQL_Bind(stmt_insertPlayerAccount, 0, username);
    MySQL_Bind(stmt_insertPlayerAccount, 1, hash);
    MySQL_ExecuteThreaded_Inline(stmt_insertPlayerAccount, using inline OnRegister);
}