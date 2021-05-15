#if defined _inc_impl
	#undef _inc_impl
#endif

static
    chat_msgOut[144];

stock SendSpecialMsg(playerid, title_color, const title[], const string[]) {
    return SendMsgF(playerid, title_color, "%s: "WHITE"%s", title, string);
}

stock SendSyntaxMsg(playerid, const string[]) {
    return SendMsgF(playerid, X11_GREY, "°“√„™È: "WHITE"%s", string);
}

stock SendErrorMsg(playerid, const string[]) {
    return SendMsgF(playerid, X11_INDIANRED, "¢ÈÕº‘¥æ≈“¥: "WHITE"%s", string);
}

stock SendServerMsg(playerid, const string[]) {
    return SendMsgF(playerid, X11_ORANGE, "‡´‘√Ïø‡«Õ√Ï: "WHITE"%s", string);
}

stock SendErrorMsgF(playerid, const fmat[], {Float,_}:...) {
    va_format(chat_msgOut, sizeof (chat_msgOut), fmat, va_start<2>);
    return SendErrorMsg(playerid, chat_msgOut);
}

stock SendServerMsgF(playerid, const fmat[], {Float,_}:...) {
    va_format(chat_msgOut, sizeof (chat_msgOut), fmat, va_start<2>);
    return SendServerMsg(playerid, chat_msgOut);
}

stock SendMsgF(playerid, colour, const fmat[], {Float,_}:...) {
    va_format(chat_msgOut, sizeof (chat_msgOut), fmat, va_start<3>);
    return SendClientMessage(playerid, colour, chat_msgOut);
}

stock SendMsgAllF(colour, const fmat[], {Float,_}:...) {
    va_format(chat_msgOut, sizeof (chat_msgOut), fmat, va_start<2>);
    return SendClientMessageToAll(colour, chat_msgOut);
}

stock SendMsgLocal(playerid, Float:radius = 10.0, colour, const string[]) {
    SendClientMessage(playerid, colour, string);
    foreach(new i: StreamedPlayer[playerid])
    {
        if (Player_IsNearPlayer(playerid, i, radius))
        {
            SendClientMessage(i, colour, string);
        }
    }
}

stock SendMsgLocalF(playerid, Float:radius = 15.0, colour, const fmat[], {Float,_}:...) {
    va_format(chat_msgOut, sizeof (chat_msgOut), fmat, va_start<4>);
    SendClientMessage(playerid, colour, chat_msgOut);
    foreach(new i: StreamedPlayer[playerid])
    {
        if (Player_IsNearPlayer(playerid, i, radius))
        {
            SendClientMessage(i, colour, chat_msgOut);
        }
    }
}