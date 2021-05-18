#if defined _inc_impl
	#undef _inc_impl
#endif

/*
//--------------------------------[DISCORD MODULE]--------------------------------

 * Copyright (c) 2020, Aktah, LLC
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are not permitted in any case.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 //--------------------------------[ DISCORD ]---------------------------

stock SendDiscordMessage(channel, const message[])
{
	switch(channel)
	{
		// #admin
		case 0:
		{
			g_AdminChannelId = DCC_FindChannelById("843485197848674354");
			DCC_SendChannelMessage(g_AdminChannelId, message);
		}
		// #admin-warnings
		case 1:
		{
			g_AdminWarningsChannelId = DCC_FindChannelById("844035130095173682");
			DCC_SendChannelMessage(g_AdminWarningsChannelId, message);
		}
		// #headadmin
		case 2:
		{
			g_HeadAdminChannelId = DCC_FindChannelById("843191954602197012");
			DCC_SendChannelMessage(g_HeadAdminChannelId, message);
		}
		// #server-errors
		case 3:
		{
			g_ServerErrorsChannelId = DCC_FindChannelById("844035161708429313");
			DCC_SendChannelMessage(g_ServerErrorsChannelId, message);
		}
	}
	return 1;
}

#if defined USING_DISCORD
forward DCC_OnChannelMessage(DCC_Channel:channel, DCC_User:author, const message[]);
public DCC_OnChannelMessage(DCC_Channel:channel, DCC_User:author, const message[])
{

	new channel_name[32], user_name[DCC_USERNAME_SIZE], discordMsg[128];
	DCC_GetChannelName(channel, channel_name);
	DCC_GetUserName(author, user_name);
	printf("[DCC] OnChannelMessage (Channel %s): Author %s sent message: %s", channel_name, user_name, message);
	if(!strcmp(channel_name, "tester-chat", true) && strcmp(user_name, "Southwood", true))
	{
		format(discordMsg, sizeof(discordMsg), "[Discord] Administrator %s: %s", user_name, message);
		SendAdminMsg(ADMIN_MSG_INFO, 1, discordMsg);
	}
	else if(!strcmp(channel_name, "staff-chat", true) && strcmp(user_name, "Southwood", true))
	{
		format(discordMsg, sizeof(discordMsg), "[Discord] Administrator %s: %s", user_name, message);
		SendAdminMsg(ADMIN_MSG_INFO, Y_PLAYER_ADMIN_STAFF_LEVEL, discordMsg);
	}

	return 1;
}
#endif
