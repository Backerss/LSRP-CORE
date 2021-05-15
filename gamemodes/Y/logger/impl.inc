#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	Logger_Log("[logger:OnGameModeInit] setting up...");
    Logger_ToggleDebug(PLAYER_DEBUG_HANDLER, true);
	Logger_ToggleDebug(SERVER_DEBUG_HANDLER, true);
    Logger_ToggleDebug(ADMIN_DEBUG_HANDLER, true);
    return 1;
}