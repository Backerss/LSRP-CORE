#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static
	bool:PlayerClassState[MAX_PLAYERS] = true;

hook OnPlayerLogin(playerid) {
	PlayerClassState[playerid] = false;
	defer ForceSpawnPlayer(playerid);
	return 1;
}

hook OnPlayerRequestClass(playerid, classid) 
{
	// ����͡����� F4
	// �ӡ�� bypass ��� PlayerClassState �� false
	// ���������鹤�� true
	if(!Class_SelectionStatus(playerid)) {
		TogglePlayerSpectating(playerid, true);
		defer ForceSpawnPlayer(playerid);

		Logger_Dbg(PLAYER_DEBUG_HANDLER, "[player:class / OnPlayerRequestClass] skipping class selection",
			Logger_P(playerid)
    	);
	}
	return 1;
}

timer void:ForceSpawnPlayer[50](playerid) {

	// �� SetSpawnInfo ���к��
	// ��������� TogglePlayerSpectating �ѹ�кѧ�Ѻ�������� spawn ����������¡��
	// SetSpawnInfo ��͹ ��������������������Ҩ���� spawn 价���˹
	SetSpawnInfo(playerid, 0, Player_GetSkin(playerid), 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0);

	TogglePlayerSpectating(playerid, false);
}

hook OnPlayerConnect(playerid) {
	PlayerClassState[playerid] = true;

	TogglePlayerSpectating(playerid, true);
	return 1;
}

stock Class_SelectionStatus(playerid) {
	return PlayerClassState[playerid];
}

stock void:Class_SelectionState(playerid, bool:status) {
	PlayerClassState[playerid] = status;
}