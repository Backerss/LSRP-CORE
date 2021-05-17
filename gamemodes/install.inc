#include <YSI_Coding\y_hooks>

hook OnMySQLSetupTable() {

    Logger_Log("TABLE SETUP BEGIN");

    // --------------------------------------------------------------------------------------------
    // PLAYERS
    // --------------------------------------------------------------------------------------------

    // accounts table.
    mysql_tquery_file(MySQL_GetHandle(), "db/accounts.sql");
    Logger_Log("setting up 'accounts' table...");

    // characters table.
    mysql_tquery_file(MySQL_GetHandle(), "db/characters.sql");
    Logger_Log("setting up 'characters' table...");

    // character stats table.
    mysql_tquery_file(MySQL_GetHandle(), "db/character_stats.sql");
    Logger_Log("setting up 'character stats' table...");

    // --------------------------------------------------------------------------------------------
    // Factions
    // --------------------------------------------------------------------------------------------

    mysql_tquery_file(MySQL_GetHandle(), "db/rp_factions.sql");
    Logger_Log("setting up 'factions' table...");

    mysql_tquery_file(MySQL_GetHandle(), "db/rp_faction_ranks.sql");
    Logger_Log("setting up 'faction ranks' table...");

    mysql_tquery_file(MySQL_GetHandle(), "db/rp_faction_members.sql");
    Logger_Log("setting up 'faction members' table...");

    mysql_tquery_file(MySQL_GetHandle(), "db/rp_faction_spawn.sql");
    Logger_Log("setting up 'faction spawn' table...");

    mysql_tquery_file(MySQL_GetHandle(), "db/rp_graffitis.sql");
    Logger_Log("setting up 'graffitis' table...");


    // --------------------------------------------------------------------------------------------
    // Utils
    // --------------------------------------------------------------------------------------------
 
    mysql_tquery_file(MySQL_GetHandle(), "db/skins.sql");
    Logger_Log("setting up 'skins' table...");


    // --------------------------------------------------------------------------------------------
    // It's a magic
    // --------------------------------------------------------------------------------------------
 
     // Admin accounts table.
    mysql_tquery_file(MySQL_GetHandle(), "db/admins.sql");
    Logger_Log("setting up 'admins' table...");

     // bans table.
    mysql_tquery_file(MySQL_GetHandle(), "db/bans.sql");
    Logger_Log("setting up 'bans' table...");


    inline Handler()
    {
        CallLocalFunction("OnMySQLConnected", "");
    }
    MySQL_TQueryInline(MySQL_GetHandle(), using inline Handler, "SELECT 1=1");

    Logger_Log("TABLE SETUP END");
    return 1;
}