#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
    Statement:stmt_loadPlayerStats,
    Statement:stmt_insertPlayerCharacterStats,
    Statement:stmt_savePlayerScore,
    Statement:stmt_savePlayerXP,
    Statement:stmt_savePlayerKills,
    Statement:stmt_savePlayerMoney,
    Statement:stmt_savePlayerDeaths;

static
    PlayerScore[MAX_PLAYERS],
    PlayerXP[MAX_PLAYERS],
    PlayerKills[MAX_PLAYERS],
    PlayerDeaths[MAX_PLAYERS]
;

hook OnMySQLConnected() {

    static const PlayerStatsQuery[] ="\
    SELECT \
        developer, \
        kills, \
        deaths, \
        job_id, \
        wanted_level, \
        money, \
        score \
        xp \
    FROM \
        character_stats \
    WHERE \
        c_id = ?";


    stmt_loadPlayerStats = MySQL_PrepareStatement(MySQL_GetHandle(), PlayerStatsQuery);

    stmt_insertPlayerCharacterStats = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO character_stats (c_id, age, skin) VALUES (?, ?, ?)");

    stmt_savePlayerMoney = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE character_stats SET money=? WHERE c_id=?");

    static const savePlayerScoreQuery[] = "\
    UPDATE \
        character_stats \
    SET \
        score = ? \
    WHERE \
        c_id = ? \
    ";
    stmt_savePlayerScore = MySQL_PrepareStatement(MySQL_GetHandle(), savePlayerScoreQuery);

    static const savePlayerXPQuery[] = "\
    UPDATE \
        character_stats \
    SET \
        xp = ? \
    WHERE \
        c_id = ? \
    ";
    stmt_savePlayerXP = MySQL_PrepareStatement(MySQL_GetHandle(), savePlayerXPQuery);


    static const savePlayerKillQuery[] = "\
    UPDATE \
        character_stats \
    SET \
        kills = ? \
    WHERE \
        c_id = ? \
    ";
    stmt_savePlayerKills = MySQL_PrepareStatement(MySQL_GetHandle(), savePlayerKillQuery);

    static const savePlayerDeathQuery[] = "\
    UPDATE \
        character_stats \
    SET \
        deaths = ? \
    WHERE \
        c_id = ? \
    ";
    stmt_savePlayerDeaths = MySQL_PrepareStatement(MySQL_GetHandle(), savePlayerDeathQuery);
}

hook OnPlayerLogin(playerid) {
    Character_LoadStats(playerid);
    return 1;
}

CreateCharacterStats(playerid) {

    inline const OnCharacterStatsCreated() {
        FetchAccountCharacter(playerid);
    }

    MySQL_BindInt(stmt_insertPlayerCharacterStats, 0, Player_GetCharacterID(playerid));
	MySQL_BindInt(stmt_insertPlayerCharacterStats, 1, CharacterUI_GetAge(playerid));
	MySQL_BindInt(stmt_insertPlayerCharacterStats, 2, CharacterUI_GetSkin(playerid));
    MySQL_ExecuteThreaded_Inline(stmt_insertPlayerCharacterStats, using inline OnCharacterStatsCreated);
}


static Character_LoadStats(playerid) {
    inline OnDataLoad() {
        new
            isDeveloper,
            kills,
            deaths,
            jobID,
            wantedLevel,
            playerMoney,
            playerScore,
            playerXP;

        MySQL_BindResultInt(stmt_loadPlayerStats, 0, isDeveloper);
        MySQL_BindResultInt(stmt_loadPlayerStats, 1, kills);
        MySQL_BindResultInt(stmt_loadPlayerStats, 2, deaths);
        MySQL_BindResultInt(stmt_loadPlayerStats, 3, jobID);
        MySQL_BindResultInt(stmt_loadPlayerStats, 4, wantedLevel);
        MySQL_BindResultInt(stmt_loadPlayerStats, 5, playerMoney);
        MySQL_BindResultInt(stmt_loadPlayerStats, 6, playerScore);
        MySQL_BindResultInt(stmt_loadPlayerStats, 7, playerXP);

        if(MySQL_Statement_FetchRow(stmt_loadPlayerStats)) {

            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:character:stats / Character_LoadStats]",
                Logger_P(playerid)
            );

            // TODO: รับยศ dev
            /*if (isDeveloper == 1)
            {
                Admin_SetPlayerDeveloper(playerid, true);
            }*/

            Player_GiveKill(playerid, kills);
            Player_SetDeaths(playerid, deaths);

            // TODO: อาชีพ
            // Player_SetJob(playerid, jobID);
            

            SetPlayerWantedLevel(playerid, wantedLevel);

            defer DelayGiveMoney(playerid, playerMoney);

            Player_GiveScore(playerid, playerScore);

            Player_SetXP(playerid, playerXP);
        }
    }

    MySQL_BindInt(stmt_loadPlayerStats, 0, Player_GetCharacterID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadPlayerStats, using inline OnDataLoad);
}

timer DelayGiveMoney[100](playerid, money){

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:player:character:stats / timer:DelayGiveMoney]",
        Logger_P(playerid),
        Logger_I("money", money)
    );

    GivePlayerMoney(playerid, money);
}

stock Player_GiveScore(playerid, score, bool:save = false)
{
    new
        currentScore = Player_GetScore(playerid),
        newScore = currentScore + score;
    
    SetPlayerScore(playerid, newScore);
    PlayerScore[playerid] = newScore;

    if(save) {
        MySQL_BindInt(stmt_savePlayerScore, 0, newScore);
        MySQL_BindInt(stmt_savePlayerScore, 1, Player_GetCharacterID(playerid));
        MySQL_ExecuteThreaded(stmt_savePlayerScore);
    }
}

stock Player_RemoveScore(playerid, score) {
    new
        currentScore = Player_GetScore(playerid),
        newScore = currentScore - score;
    SetPlayerScore(playerid, newScore);
    PlayerScore[playerid] = newScore;
    MySQL_BindInt(stmt_savePlayerScore, 0, newScore);
    MySQL_BindInt(stmt_savePlayerScore, 1, Player_GetCharacterID(playerid));
    MySQL_ExecuteThreaded(stmt_savePlayerScore);
}

stock Player_SetXP(playerid, xp, bool:save = false)
{
    PlayerXP[playerid] = xp;

    if(save) {
        MySQL_BindInt(stmt_savePlayerXP, 0, xp);
        MySQL_BindInt(stmt_savePlayerXP, 1, Player_GetCharacterID(playerid));
        MySQL_ExecuteThreaded(stmt_savePlayerXP);
    }
}

stock Player_GetScore(playerid)
    return PlayerScore[playerid];

stock Player_GetXP(playerid)
    return PlayerXP[playerid];

stock Player_GetKills(playerid)
    return PlayerKills[playerid];

stock Player_GiveKill(playerid, kills, bool:save = false) {
    new
        currentKills = Player_GetKills(playerid),
        newKills = currentKills + kills;
    
    PlayerKills[playerid] = newKills;

    if(save) {
        MySQL_BindInt(stmt_savePlayerKills, 0, newKills);
        MySQL_BindInt(stmt_savePlayerKills, 1, Player_GetCharacterID(playerid));
        MySQL_ExecuteThreaded(stmt_savePlayerKills);
    }
}

stock Player_GetDeaths(playerid)
    return PlayerDeaths[playerid];

stock Player_SetDeaths(playerid, deaths, bool:save = false) {
    new
        currentDeaths = Player_GetDeaths(playerid),
        newDeaths = currentDeaths + deaths;
    
    PlayerDeaths[playerid] = newDeaths;

    if(save) {
        MySQL_BindInt(stmt_savePlayerDeaths, 0, newDeaths);
        MySQL_BindInt(stmt_savePlayerDeaths, 1, Player_GetCharacterID(playerid));
        MySQL_ExecuteThreaded(stmt_savePlayerDeaths);
    }
}

forward OnPlayerMoneyChange(playerid, previous, current);
public OnPlayerMoneyChange(playerid, previous, current) {

    // ถ้ายังไม่เลือกตัวละครก็อย่าพึ่งอัพเดตอะไร
    if (!Character_IsLoggedIn(playerid))
        return 0;

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:account:character:stats / OnPlayerMoneyChange]",
        Logger_P(playerid),
        Logger_I("previous", previous),
        Logger_I("current", current)
    );
    
    MySQL_BindInt(stmt_savePlayerMoney, 0, current);
    MySQL_BindInt(stmt_savePlayerMoney, 1, Player_GetCharacterID(playerid));
    MySQL_ExecuteThreaded(stmt_savePlayerMoney);
    return 1;
}
