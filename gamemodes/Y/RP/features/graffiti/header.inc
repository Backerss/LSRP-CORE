#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI_Visual\y_commands>
#include <YSI_Visual\y_dialog>
#include <YSI_Coding\y_inline>
#include <YSI_Extra\y_inline_mysql>
#include <YSI_Coding\y_timers>
#include <YSI_Core\y_utils>

#define Graffiti. OO_TYPE(GRAFFITI,Graffiti)

#define	INVALID_GRAFFITI_ID	-1
#define	MAX_GRAFFITI_IMAGE	9
#define MAX_GRAFFITI_NAME	64
#define	MAX_GRAFFITI_FONT	24
#define	MAX_GRAFFITI_OBJECT	100

enum E_GRAFFITI_IMAGE 
{
	E_GRAFFITI_Image_modelid,
	E_GRAFFITI_Image_name[MAX_GRAFFITI_NAME char]
}

new const GraffitiImage[MAX_GRAFFITI_IMAGE][E_GRAFFITI_IMAGE] = {
	{18659, !"Grove St 4 Life"},
	{18660, !"Seville B.V.V.D Families"},
	{18661, !"Varrio Los Aztecas"},
	{18662, !"Kilo"},
	{18663, !"San Fiero Rifa"},
	{18664, !"Temple Drive Ballas"},
	{18665, !"Los Santos Vargos"},
	{18666, !"Front Yard Balas"},
	{18667, !"Rolling Heights Ballas"}
};

enum E_GRAFFITI_SETUP
{
	E_GRAFFITI_Setup_spraying,					// กำลังสเปรย์
	E_GRAFFITI_Setup_spraytime,					// เวลาที่ใช้ในการสเปรย์
	E_GRAFFITI_Setup_sprayed,					// ที่สเปรย์ไปแล้ว

	E_GRAFFITI_Setup_modelid,
	E_GRAFFITI_Setup_font[MAX_GRAFFITI_FONT],
	E_GRAFFITI_Setup_text[MAX_GRAFFITI_NAME],

	Timer:E_GRAFFITI_Setup_timer
}

new GraffitiSetup[MAX_PLAYERS][E_GRAFFITI_SETUP];

enum E_GRAFFITI_OBJECT
{
	DBID:E_GRAFFITI_Object_id,
	DBID:E_GRAFFITI_Object_owner,
	E_GRAFFITI_Object_ownerName[MAX_PLAYER_NAME + 1],

	E_GRAFFITI_Object_name[MAX_GRAFFITI_NAME],
	Float:E_GRAFFITI_Object_x,
	Float:E_GRAFFITI_Object_y,
	Float:E_GRAFFITI_Object_z,
	Float:E_GRAFFITI_Object_rx,
	Float:E_GRAFFITI_Object_ry,
	Float:E_GRAFFITI_Object_rz,

	E_GRAFFITI_Object_modelid,
	E_GRAFFITI_Object_font[MAX_GRAFFITI_FONT],

	E_GRAFFITI_Object_object,
}

new GraffitiObject[MAX_GRAFFITI_OBJECT][E_GRAFFITI_OBJECT];

new PlayerSpray[MAX_PLAYERS char];

#define GRAFFITI_TABLE "rp_graffitis"

stock bool:IsValidGraffitiObject(id) {
	return (0 <= id < MAX_GRAFFITI_OBJECT);
}

stock bool:IsValidGraffitiImage(id) {
	return (0 <= id < sizeof(GraffitiImage));
}

stock bool:GetGraffitiImageName(id, name[], size = sizeof(name)) {
	if (!IsValidGraffitiImage(id)) {
		return false;
	}
	strunpack(name, Graffiti.Image[id][@name], size);
	return true;
}