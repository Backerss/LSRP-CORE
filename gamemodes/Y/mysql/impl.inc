#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
	new errorQuery[1024];
	switch(errorid) {
		case 1136: format(errorQuery, sizeof(errorQuery), "[MySQL] Error 1136 | Column count does not match value count => \"%s\"", query);
		case 1054: format(errorQuery, sizeof(errorQuery), "[MySQL] Error 1054 | Invalid field name => \"%s\"", query);
		case 1065: format(errorQuery, sizeof(errorQuery), "[MySQL] Error 1065 | Query was empty (variable's size too small?) => \"%s\" from \"%s\"", query, callback);
		case 1058: format(errorQuery, sizeof(errorQuery), "[MySQL] Error 1058 | Column count doesn't match value count => \"%s\" from \"%s\"", query, callback);
		case 1203: format(errorQuery, sizeof(errorQuery), "[MySQL] Error 1203 | User already has more than 'max_user_connections' active connections => \"%s\" from \"%s\"", query, callback);
		case 1045: format(errorQuery, sizeof(errorQuery), "[MySQL] Error 1045 | Access denied");
		case ER_SYNTAX_ERROR: format(errorQuery, sizeof(errorQuery), "[MySQL] Syntax Error => \"%s\"",query);

		default : format(errorQuery, sizeof(errorQuery), "[MySQL] Error %d | Callback: %s | \"%s\"", errorid, callback, query);
	}

	SendDiscordMessage(3, errorQuery);

	Logger_Log("MySQL Error", Logger_S("msg", errorQuery));

	return 1;
}

hook OnGameModeInit() {

    Logger_Log("[db:OnGameModeInit] setting up...");

    new
        MySQLOpt: option_id = mysql_init_options();
	mysql_set_option(option_id, AUTO_RECONNECT, true);

	MySQL_gHandle = mysql_connect_file("mysql.ini");
	if (MySQL_gHandle == MYSQL_INVALID_HANDLE || mysql_errno(MySQL_gHandle) != 0)
	{
		Logger_Fatal("MySQL failed to connect. Server shutting down...");
		return 1;
	}

	mysql_log(ALL);

    mysql_set_charset(MYSQL_CHARSET, MySQL_gHandle);

    #if SETUP_TABLE
        CallLocalFunction("OnMySQLSetupTable", "");
	#else
		CallLocalFunction("OnMySQLConnected", "");
    #endif

    Logger_Log("MySQL connection is successful.");
    return 1;
}

hook OnGameModeExit() {
    CallLocalFunction("OnMySQLPreClose", "");
    Logger_Log("MySQL connection has been closed.");
    mysql_close(MySQL_gHandle);
}