#if defined _inc_impl
	#undef _inc_impl
#endif

#include "login\impl"
#include "register\impl"

#include <YSI_Coding\y_hooks>

static
    Statement: stmt_checkAccount;

hook OnMySQLConnected() {
    stmt_checkAccount = MySQL_PrepareStatement(MySQL_GetHandle(), "SELECT u_id, password FROM accounts WHERE username=?");
}

timer MoveCamera[500](playerid) {
    InterpolateCameraPos(playerid, 3.1435,-1955.6364,88.1194, 2106.8354,-1270.0221,100.7057, 1000 * 120, CAMERA_MOVE);
    InterpolateCameraLookAt(playerid, 2325.7061,-1063.7642,55.4202, 2325.7061,-1063.7642,45.4202, 1000 * 120, CAMERA_MOVE);
}

hook OnPlayerPassedBanCheck(playerid)
{
	defer MoveCamera(playerid);

    new playerName[MAX_PLAYERS];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    inline const OnDataLoad() {
        new
            accountUID,
            playerHash[62];

        MySQL_BindResultInt(stmt_checkAccount, 0, accountUID);
        MySQL_BindResult(stmt_checkAccount, 1, playerHash, sizeof(playerHash));

        if(MySQL_Statement_FetchRow(stmt_checkAccount)) {
            Player_SetAccountID(playerid, accountUID);

            Account_PromptLogin(playerid, playerHash);
        }
        else {
            Account_PromptRegister(playerid);
        }
    }

    MySQL_Bind(stmt_checkAccount, 0, playerName);
    MySQL_ExecuteParallel_Inline(stmt_checkAccount, using inline OnDataLoad);
}