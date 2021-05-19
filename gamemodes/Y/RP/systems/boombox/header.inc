#if defined _inc_header
	#undef _inc_header
#endif

enum E_PLAYER_BOOMBOX
{
	pBoombox
};
new boombox[MAX_PLAYERS][E_PLAYER_BOOMBOX];

enum boomboxData {
	boomboxPlaced,
	Float:boomboxPos[3],
	boomboxInterior,
	boomboxWorld,
	boomboxObject,
	boomboxURL[128 char],
	Text3D:boomboxText3D
};
new BoomboxData[MAX_PLAYERS][boomboxData];
