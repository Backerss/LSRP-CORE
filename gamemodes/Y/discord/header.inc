#if defined _inc_header
	#undef _inc_header
#endif

#include <discord-connector>

#if defined USING_DISCORD
new 
    DCC_Channel:g_AdminChannelId, 
    DCC_Channel:g_AdminWarningsChannelId, 
    DCC_Channel:g_HeadAdminChannelId, 
    DCC_Channel:g_ServerErrorsChannelId;
#endif