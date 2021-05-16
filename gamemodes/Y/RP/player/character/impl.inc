#if defined _inc_impl
	#undef _inc_impl
#endif

#include "ui\impl"
#include "stats\impl"
#include "location\impl"
#include "skin\impl"
#include "spawn\impl"
#include "vip\impl"

#include <YSI_Coding\y_hooks>

static
    Statement:stmt_checkCharacter,
    Statement:stmt_insertPlayerCharacter,
    Character_Nums[MAX_PLAYERS],
    Character_ID[MAX_PLAYERS],
    Character_Name[MAX_PLAYERS][MAX_PLAYER_NAME + 1],
    Character_RPName[MAX_PLAYERS][MAX_PLAYER_NAME + 1];

hook OnMySQLConnected() {

    static const checkCharacterQuery[] = "\
    SELECT \
        c.c_id, \
        c.player_name, \
        cs.skin \
    FROM \
        characters AS c \
    LEFT JOIN \
        character_stats AS cs \
    ON \
        cs.c_id = c.c_id \
    WHERE \
        c.u_id = ? \
    ";
    stmt_checkCharacter = MySQL_PrepareStatement(MySQL_GetHandle(), checkCharacterQuery);


    stmt_insertPlayerCharacter = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO characters (u_id, player_name) VALUES (?, ?)");

}

hook OnGameModeInit()
{
	CharacterSelectedGroup = Group_Create("Character selected");
	return 1;
}

hook OnPlayerConnect(playerid)
{
    Character_Nums[playerid] = 0;
    return 1;
}

hook OnPlayerAccountLogin(playerid)
{
    FetchAccountCharacter(playerid);
}

FetchAccountCharacter(playerid)
{
    // fetchCharacter
    CharacterUI_Clear(playerid);

    inline const OnDataLoad() {
        new
            characterUID,
            playerName[MAX_PLAYER_NAME + 1],
            playerSkin;

        MySQL_BindResultInt(stmt_checkCharacter, 0, characterUID);
        MySQL_BindResult(stmt_checkCharacter, 1, playerName, sizeof(playerName));
        MySQL_BindResultInt(stmt_checkCharacter, 2, playerSkin);

        new
            count = 0;
        while(MySQL_Statement_FetchRow(stmt_checkCharacter)) {
            CharacterUI_Add(playerid, characterUID, playerSkin, playerName);
            count ++;
        }

        if (count == 0)
        {
            // ไม่มีตัวละครอยู่ในบัญชี
            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:account:character / FetchAccountCharacter] force player to creation character",
                Logger_P(playerid)
            );

            CharacterUI_Creation(playerid);
        }
        else
        {
            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[rp:account:character / FetchAccountCharacter] character selection",
                Logger_P(playerid),
                Logger_I("characters", count)
            );
            Character_Nums[playerid] = count;
            CharacterUI_Selection(playerid);
        }
    }

    MySQL_BindInt(stmt_checkCharacter, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_checkCharacter, using inline OnDataLoad);
}

stock Player_SetCharacterID(playerid, value) {
    return Character_ID[playerid] = value;
}

stock Player_GetCharacterID(playerid) {
    return Character_ID[playerid];
}

stock Player_SetCharacterName(playerid, const name[], len = sizeof(name)) {
    Character_Name[playerid][0] = '\0';
    strcpy(Character_Name[playerid], name, len);
}

stock Player_GetCharacterName(playerid) {
    return Character_Name[playerid];
}

stock Player_SetCharacterRPName(playerid, const name[], len = sizeof(name)) {
    Character_RPName[playerid][0] = '\0';
    strcpy(Character_RPName[playerid], name, len);
    RPizeName(Character_RPName[playerid]);
}

stock Player_GetCharacterRPName(playerid) {
    return Character_RPName[playerid];
}

stock StampCharacterLastLogin(playerid) {
    new
        Statement: stmt_updatePlayerLastLogin;

    stmt_updatePlayerLastLogin = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE characters SET last_login = CURRENT_TIMESTAMP() WHERE c_id = ?");
    MySQL_BindInt(stmt_updatePlayerLastLogin, 0, Player_GetCharacterID(playerid));
    MySQL_ExecuteThreaded(stmt_updatePlayerLastLogin);
    MySQL_StatementClose(stmt_updatePlayerLastLogin);

    Group_SetPlayer(CharacterSelectedGroup, playerid, true);
    return 1;
}

/**
    Format: Firstname_Lastname
        - พิมพ์ใหญ่ตัวอักษรแรกของชื่อและนามสกุล
        - ชื่อและนามสกุลต้องมีตัวอักษรไม่ต้องกว่า 3
        - ต้องมี Underscore '_' ขั้นระหว่างชื่อและนามสกุล
        - ความยาวของชื่อและนามสกุลรวมกันต้องไม่ต่ำกว่า 7 หรือเกิน 20 ตัวอักษร
 */
forward Character_ValidName(const string[]);
public Character_ValidName(const string[])
{
    new 
        len = strlen(string),
        underScore = -1;

    if (len < 7 || len > 20)
        return e_ERROR_CHARACTER_LENGTH;

    for( new i = 0; string[ i ] != EOS; ++i )
	{
        switch( string[ i ] ) {
            case '_':
            {
                underScore = i + 1;
                if (3 >= underScore || 2 >= len - underScore) // Firstname and Lastname length must be greater than 2
                    return e_ERROR_CHARACTER_NAME_LENGTH;
            }
            case 'A'..'Z', 'a'..'z':
            {
                if (i != 0 && underScore == -1 && string[ i ] >= 'A' && string[ i ] <= 'Z')
                    return e_ERROR_CHARACTER_CASE;

                if (underScore != -1 && i != underScore && string[ i ] >= 'A' && string[ i ] <= 'Z')
                    return e_ERROR_CHARACTER_CASE;      

                continue;
            }
            default: return e_ERROR_CHARACTER_VALID_NAME;
        }
    }
    
    if (underScore == -1)
        return e_ERROR_CHARACTER_VALID_NAME;

    if (string[ 0 ] < 'A' || string[ 0 ] > 'Z' || string[ underScore ] < 'A' || string[ underScore ] > 'Z')
        return e_ERROR_CHARACTER_CASE;

    return e_ERROR_CHARACTER_NONE;
}

Character_InsertToDatabase(playerid, const name[]) {
    inline const OnCharacterCreated() {
        Player_SetCharacterID(playerid, cache_insert_id());
        CreateCharacterStats(playerid);
        
        CallLocalFunction("OnCharacterCreated", "i", playerid);
    }

    new
        character_name[MAX_PLAYER_NAME + 1];
    strcpy(character_name, name);

    MySQL_BindInt(stmt_insertPlayerCharacter, 0, Player_GetAccountID(playerid));
    MySQL_Bind(stmt_insertPlayerCharacter, 1, character_name);
    MySQL_ExecuteThreaded_Inline(stmt_insertPlayerCharacter, using inline OnCharacterCreated);
}

Character_GetCount(playerid)
{
    return Character_Nums[playerid];
}

 // API

stock Character_SetLogin(playerid, bool:status) {
    return Group_SetPlayer(CharacterSelectedGroup, playerid, status);
}