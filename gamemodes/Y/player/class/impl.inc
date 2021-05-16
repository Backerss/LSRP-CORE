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
	// เมื่อกดปุ่ม F4
	// ทำการ bypass ถ้า PlayerClassState เป็น false
	// ค่าเริ่มต้นคือ true
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

	// ใช้ SetSpawnInfo แก้เตะบั๊ก
	// คือเวลาใช้ TogglePlayerSpectating มันจะบังคับให้ผู้เล่น spawn แต่ไม่ได้เรียกใช้
	// SetSpawnInfo ก่อน เซิร์ฟเวอร์เลยไม่รู้ว่าจะสั่ง spawn ไปที่ไหน
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