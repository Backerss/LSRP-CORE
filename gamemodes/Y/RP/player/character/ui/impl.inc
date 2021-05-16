#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

static const characterRace[5][] = {
	"Any Race",
	"Caucasian",
	"African American",
	"Latino",
	"Asian"
};

static const characterRaceTH[5][] = {
	"เชื้อชาติใดก็ได้",
	"คนผิวขาว",
	"แอฟริกันอเมริกัน",
	"ลาติน",
	"เอเชีย"
};

static const characterGender[3][] = {	
	"Any Gender",
	"Male",
	"Female"
};

static const characterGenderTH[3][] = {	
	"เพศใดก็ได้",
	"ชาย",
	"หญิง"
};


static const characterWeight[4][] = {
	"Any Weight",
	"Normal",
	"Overweight",
	"Muscular"
};

static const characterWeightTH[4][] = {
	"รูปร่างใดก็ได้",
	"ทั่วไป",
	"น้ำหนักเกิน",
	"แข็งแรง"
};

static const characterCategory[9][] = {	
	"Any Category",
	"Civilian",
	"Gang Member",
	"Country & Old people",
	"Prostitutes",
	"Sportsmen",
	"Specific Professions",
	"Beach Visitor",
	"Homeless person"
};

static const characterCategoryTH[9][] = {
	"หมวดหมู่ใดก็ได้",
	"พลเรือน",
	"สมาชิกแก๊ง",
	"คนชนบทและคนชรา",
	"โสเภณี",
	"นักกีฬา",
	"คนที่มีอาชีพเฉพาะทาง",
	"ผู้เยี่ยมชมชายหาด",
	"คนจรจัด"
};

static enum CHAR_CREATION_TD
{
	CHAR_CREATION_NAME = 1,
	CHAR_CREATION_AGE,
	CHAR_CREATION_RACE,
	CHAR_CREATION_GENDER,
	CHAR_CREATION_WEIGHT,
	CHAR_CREATION_CATEGORY,
	CHAR_CREATION_SKIN[10],
	CHAR_CREATION_REFRESH,
	CHAR_CREATION_CONFIRM,
	CHAR_CREATION_LEFT,
	CHAR_CREATION_RIGHT,
	CHAR_CREATION_SKIN_TEXT,

	CHAR_CREATION_PROG_NAME,
	CHAR_CREATION_PROG_AGE,
	CHAR_CREATION_PROG_SKIN,

	CHAR_SELECTION[MAX_CHARACTER_PER_PLAYER],
	CHAR_CREATION_SIZE
}

static enum E_CHARACTER_SELECTION
{
	E_CHARACTER_ID,
	E_CHARACTER_SKIN,
	E_CHARACTER_NAME[MAX_PLAYERS + 1]
}

// แชร์ตัวแปรสำหรับหน้าสร้าง/เลือกตัวละคร
static
    Statement: stmt_findSkin,
	PlayerText:characterTD[MAX_PLAYERS][MAX_CHARACTER_CREATION_TD],
	characterTDID[MAX_PLAYERS][CHAR_CREATION_TD],
	charSelectTextDrawCount[MAX_PLAYERS],

	characterFilterRace[MAX_PLAYERS],
	characterFilterGender[MAX_PLAYERS],
	characterFilterWeight[MAX_PLAYERS],
	characterFilterCategory[MAX_PLAYERS],
	characterName[MAX_PLAYERS][MAX_PLAYER_NAME + 1],
	characterAge[MAX_PLAYERS],
	BitArray:characterSkinModelList[MAX_PLAYERS]<MAX_CHARACTER_SKIN>,
	characterSkinSlot[MAX_PLAYERS][MAX_CHARACTER_SKIN],
	characterSkinCurrentPage[MAX_PLAYERS],
	characterSkinPages[MAX_PLAYERS],
	characterSelectSkin[MAX_PLAYERS],
	characterLastSelectSkinSlot[MAX_PLAYERS],

	CharacterData[MAX_PLAYERS][MAX_CHARACTER_PER_PLAYER][E_CHARACTER_SELECTION],
	Iterator:CharacterIndex[MAX_PLAYERS]<MAX_CHARACTER_PER_PLAYER>,
	characterLastClickTick[MAX_PLAYERS]
;

hook OnMySQLConnected()
{
    static const findSkinQuery[] = "\
    SELECT \
        id \
    FROM \
        skins \
    WHERE \
        (race = ? OR ? = 0) AND \
		(gender = ? OR ? = 0) AND \
		(weight = ? OR ? = 0) AND \
		(category = ? OR ? = 0) \
    ";
    stmt_findSkin = MySQL_PrepareStatement(MySQL_GetHandle(), findSkinQuery);
}

hook OnPlayerConnect(playerid)
{
	characterAge[playerid] = 0;
	characterName[playerid][0] = '\0';

	characterFilterRace[playerid] =
	characterFilterGender[playerid] =
	characterFilterWeight[playerid] =
	characterFilterCategory[playerid] = 0;

	charSelectTextDrawCount[playerid] = 0;
	characterSkinPages[playerid] = 0;
	characterSkinCurrentPage[playerid] = 0;

	characterLastSelectSkinSlot[playerid] = -1;
	characterSelectSkin[playerid] = 0;
}

static void:_CharacterUI_Clear(playerid)
{
	for(new i = 0; i != _:CHAR_CREATION_SIZE; i++)
	{
		characterTDID[playerid][CHAR_CREATION_TD:i] = -1;
	}

	while(charSelectTextDrawCount[playerid])
		PlayerTextDrawDestroy(playerid, characterTD[playerid][--charSelectTextDrawCount[playerid]]);

	CancelSelectTextDraw(playerid);
}

static void:_CharacterUI_RefreshSkin(playerid)
{
	Bit_SetAll(characterSkinModelList[playerid], false);

    inline const OnFindCharacterSkin() {
	
        new
			id,
            count = 0;
		
		MySQL_BindResultInt(stmt_findSkin, 0, id);
        while(MySQL_Statement_FetchRow(stmt_findSkin)) {
			Bit_Set(characterSkinModelList[playerid], id, true);
            count ++;
        }

		characterSkinCurrentPage[playerid] = 0;
		characterSkinPages[playerid] = count > 0 ? floatround(count / 10, floatround_ceil) : 0;

		_CharacterUI_SetCharacterPage(playerid, 0);
    }

    MySQL_BindInt(stmt_findSkin, 0, characterFilterRace[playerid]);
    MySQL_BindInt(stmt_findSkin, 1, characterFilterRace[playerid]);
    MySQL_BindInt(stmt_findSkin, 2, characterFilterGender[playerid]);
	MySQL_BindInt(stmt_findSkin, 3, characterFilterGender[playerid]);
	MySQL_BindInt(stmt_findSkin, 4, characterFilterWeight[playerid]);
	MySQL_BindInt(stmt_findSkin, 5, characterFilterWeight[playerid]);
	MySQL_BindInt(stmt_findSkin, 6, characterFilterCategory[playerid]);
	MySQL_BindInt(stmt_findSkin, 7, characterFilterCategory[playerid]);

    MySQL_ExecuteParallel_Inline(stmt_findSkin, using inline OnFindCharacterSkin);
}

static void:_CharacterUI_SetCharacterPage(playerid, page)
{
	if (page < 0) page = 0;
	else if (page >= characterSkinPages[playerid]) page = characterSkinPages[playerid] - 1;

	characterSkinCurrentPage[playerid] = page;

	new
		string[34],
		resultSlot = characterTDID[playerid][CHAR_CREATION_CONFIRM]-1
	;

	if (characterSkinPages[playerid] > 0)
	{
		format(string, sizeof(string), "Result: found %d skin(s) (%d/%d)", Bit_GetCount(characterSkinModelList[playerid]), characterSkinCurrentPage[playerid] + 1, characterSkinPages[playerid]);

		if (characterSkinCurrentPage[playerid] > 0) PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_LEFT]]);
		else PlayerTextDrawHide(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_LEFT]]);

		if (characterSkinCurrentPage[playerid] + 1 < characterSkinPages[playerid]) PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RIGHT]]);
		else PlayerTextDrawHide(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RIGHT]]);
	}
	else
	{
		format(string, sizeof(string), "Result: Not found!!");
		PlayerTextDrawHide(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_LEFT]]);
		PlayerTextDrawHide(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RIGHT]]);
	}
	PlayerTextDrawSetString(playerid, characterTD[playerid][resultSlot], string);
	PlayerTextDrawShow(playerid, characterTD[playerid][resultSlot]);

	_CharacterUI_RenderSkin(playerid);

}

static void:_CharacterUI_RenderSkin(playerid)
{
	for(new i = 0; i != 10; i++)
	{
		if(characterTDID[playerid][CHAR_CREATION_SKIN][i] != -1)
		{
			PlayerTextDrawDestroy(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN][i]]);
			characterTDID[playerid][CHAR_CREATION_SKIN][i] = -1;
			charSelectTextDrawCount[playerid]--;
		}
	}

	new 
		count = 0,
		skip = characterSkinCurrentPage[playerid] * 10,
		Float:offsetY = 0.0,
		Float:offsetX = 0.0
	;

	for(new i = 0; i != MAX_CHARACTER_SKIN; i++)
	{
		if (Bit_Get(characterSkinModelList[playerid], i))
		{
			if (skip)
			{
				skip--;
				continue;
			}

			if (count >= 10)
				break;

			characterTDID[playerid][CHAR_CREATION_SKIN][count] = charSelectTextDrawCount[playerid];
			characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 163.000000 + offsetX, 238.000000 + offsetY, "Preview_Model");
			PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 5);
			PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
			PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 60.000000, 60.000000);
			PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
			PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], characterSelectSkin[playerid] == i ? 0xFA8795EE : 0x87CEFA7D);
			PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
			PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
			PlayerTextDrawSetPreviewModel(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], i);
			PlayerTextDrawSetPreviewRot(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -44.000000, 1.000000, 16.000000, 0.899999);
			PlayerTextDrawShow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++]);
			
			if (characterSelectSkin[playerid] == i)
				characterLastSelectSkinSlot[playerid] = count;

			characterSkinSlot[playerid][count] = i;

			if (count == 4)
			{
				// Next row, Reset column
				offsetY += 61.0;
				offsetX = 0;
			}
			else
			{
				offsetX += 61.0;
			}

			count++;
		}
	}
}

CharacterUI_Creation(playerid)
{
	_CharacterUI_Clear(playerid);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 315.000000, 143.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 29.999977);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 363.000000, 317.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 158.000000, 145.000000, "Character Creation Panel");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 3);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.275000, 1.100000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 471.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 158.000000, 161.000000, "Name:");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.224998, 0.899999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 256.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 158.000000, 175.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 1.229998);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 257.000000, 92.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_NAME] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 159.000000, 177.000000, "Firstname Lastname");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 0.779999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 256.000000, 5.644927);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 158.000000, 194.000000, "Age:");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.224998, 0.899999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 256.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 188.000000, 192.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 1.229998);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 257.000000, 92.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_AGE] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 190.000000, 194.000000, "0");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 0.779999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255.778991, 5.644927);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 262.000000, 161.000000, "Filter:");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.224998, 0.899999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 326.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 263.000000, 175.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 1.229998);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 328.000000, 92.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0xFFFFFF80);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_RACE] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 265.000000, 177.000000, "Any Race");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 0.779999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 326.000000, 5.644927);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 332.000000, 175.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 1.229998);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 378.000000, 92.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0xFFFFFF80);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_GENDER] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 334.000000, 177.000000, "Any Gender");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 0.779999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 376.000000, 5.644927);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 382.000000, 175.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 1.229998);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 471.000000, 92.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0xFFFFFF80);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_CATEGORY] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 384.000000, 177.000000, "Any Category");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 0.779999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 469.000000, 5.644927);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);
	
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 263.000000, 192.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 1.229998);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 328.000000, 92.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0xFFFFFF80);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_WEIGHT] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 265.000000, 194.000000, "Any weight");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.195832, 0.779999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 326.000000, 5.644927);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTDID[playerid][CHAR_CREATION_REFRESH] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 401.000000, 192.000000, "Refresh");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.275000, 1.100000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 7.971000, 139.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -65336);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 158.000000, 222.000000, "Result:");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 3);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.275000, 1.100000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 471.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_CONFIRM] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 315.000000, 383.000000, "Confirm");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.258332, 1.750000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 16.500000, 90.500000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -65336);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTDID[playerid][CHAR_CREATION_LEFT] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 136.000000, 268.000000, "LD_BEAT:left");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 4);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 17.000000, 45.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTDID[playerid][CHAR_CREATION_RIGHT] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 478.000000, 268.000000, "LD_BEAT:right");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 4);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 17.000000, 45.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1);

	characterTDID[playerid][CHAR_CREATION_SKIN_TEXT] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 315.000000, 362.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.258332, 1.750000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 16.500000, 317.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 558.000000, 143.000000, "_");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 11.999977);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 363.000000, 157.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 200);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 482.000000, 145.000000, "Progress");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 3);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.275000, 1.100000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 634.500000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 489.000000, 167.000000, "- Name your character");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.224997, 0.899999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 627.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_PROG_NAME] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 613.000000, 161.000000, "LD_CHAT:thumbdn");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 4);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 17.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 489.000000, 190.000000, "- Choose Age");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.224997, 0.899999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 627.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_PROG_AGE] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 585.000000, 184.000000, "LD_CHAT:thumbdn");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 4);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 17.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 489.000000, 213.000000, "- Choose Skin");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.224997, 0.899999);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 627.000000, 11.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -16777166);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	characterTDID[playerid][CHAR_CREATION_PROG_SKIN] = charSelectTextDrawCount[playerid];
	characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, 585.000000, 207.000000, "LD_CHAT:thumbdn");
	PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 4);
	PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 17.000000, 17.000000);
	PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
	PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
	PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
	PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
	PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
	PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

	for(new i = 0; i != charSelectTextDrawCount[playerid]; i++)
	{
		PlayerTextDrawShow(playerid, characterTD[playerid][i]);
	}

	characterLastClickTick[playerid] = GetTickCount();

	_CharacterUI_RefreshSkin(playerid);

	SelectTextDraw(playerid, 0xFFFFFF95);
}

hook OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	if (clickedid == Text:INVALID_TEXT_DRAW && charSelectTextDrawCount[playerid])
	{
		SelectTextDraw(playerid, 0xFFFFFF95);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid)
{
	if (Dialog_Get(playerid) != -1 || charSelectTextDrawCount[playerid] == 0)
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	if (GetTickCount() - characterLastClickTick[playerid] < 250)
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	if (characterTDID[playerid][CHAR_CREATION_RACE] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RACE]])
	{
		inline const RaceFilterResponse(response, listitem, string:inputtext[]) {
			#pragma unused inputtext

			characterLastClickTick[playerid] = GetTickCount();

			if(!response) {
				@return 1;
			}
			
			characterFilterRace[playerid] = listitem;
			PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RACE]], characterRace[listitem]);
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RACE]]);
		}

		new
			string[256],
			formatString[32]
		;

		for(new i = 0, j = sizeof(characterRaceTH); i != j; i++)
		{
			format(formatString, sizeof(formatString), "%s\n", characterRaceTH[i]);
			strcat(string, formatString);
		}

		Dialog_ShowCallback(
			playerid,
			using inline RaceFilterResponse,
			DIALOG_STYLE_LIST,
			"ตัวกรอง: เชื้อชาติ",
			string,
			"เลือก",
			"ปิด"
		);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_GENDER] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_GENDER]])
	{
		inline const GenderFilterResponse(response, listitem, string:inputtext[]) {
			#pragma unused inputtext

			characterLastClickTick[playerid] = GetTickCount();

			if(!response) {
				@return 1;
			}
			
			characterFilterGender[playerid] = listitem;
			PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_GENDER]], characterGender[listitem]);
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_GENDER]]);
		}

		new
			string[256],
			formatString[32]
		;

		for(new i = 0, j = sizeof(characterGenderTH); i != j; i++)
		{
			format(formatString, sizeof(formatString), "%s\n", characterGenderTH[i]);
			strcat(string, formatString);
		}

		Dialog_ShowCallback(
			playerid,
			using inline GenderFilterResponse,
			DIALOG_STYLE_LIST,
			"ตัวกรอง: เพศ",
			string,
			"เลือก",
			"ปิด"
		);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_WEIGHT] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_WEIGHT]])
	{
		inline const WeightFilterResponse(response, listitem, string:inputtext[]) {
			#pragma unused inputtext

			characterLastClickTick[playerid] = GetTickCount();

			if(!response) {
				@return 1;
			}
			
			characterFilterWeight[playerid] = listitem;
			PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_WEIGHT]], characterWeight[listitem]);
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_WEIGHT]]);
		}

		new
			string[256],
			formatString[32]
		;

		for(new i = 0, j = sizeof(characterWeightTH); i != j; i++)
		{
			format(formatString, sizeof(formatString), "%s\n", characterWeightTH[i]);
			strcat(string, formatString);
		}

		Dialog_ShowCallback(
			playerid,
			using inline WeightFilterResponse,
			DIALOG_STYLE_LIST,
			"ตัวกรอง: รูปร่าง",
			string,
			"เลือก",
			"ปิด"
		);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_CATEGORY] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_CATEGORY]])
	{
		inline const CategoryFilterResponse(response, listitem, string:inputtext[]) {
			#pragma unused inputtext

			characterLastClickTick[playerid] = GetTickCount();

			if(!response) {
				@return 1;
			}
			
			characterFilterCategory[playerid] = listitem;
			PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_CATEGORY]], characterCategory[listitem]);
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_CATEGORY]]);
		}

		new
			string[256],
			formatString[32]
		;

		for(new i = 0, j = sizeof(characterCategoryTH); i != j; i++)
		{
			format(formatString, sizeof(formatString), "%s\n", characterCategoryTH[i]);
			strcat(string, formatString);
		}

		Dialog_ShowCallback(
			playerid,
			using inline CategoryFilterResponse,
			DIALOG_STYLE_LIST,
			"ตัวกรอง: หมวดหมู่",
			string,
			"เลือก",
			"ปิด"
		);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_REFRESH] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_REFRESH]])
	{
		_CharacterUI_RefreshSkin(playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_RIGHT] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_RIGHT]])
	{
		_CharacterUI_SetCharacterPage(playerid, characterSkinCurrentPage[playerid] + 1);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_LEFT] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_LEFT]])
	{
		_CharacterUI_SetCharacterPage(playerid, characterSkinCurrentPage[playerid] - 1);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_NAME] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_NAME]])
	{
		ShowSetCharacterNameDialog(playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_AGE] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]])
	{
		ShowSetCharacterAgeDialog(playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else if (characterTDID[playerid][CHAR_CREATION_CONFIRM] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_CONFIRM]])
	{
		PerformPlayerCreationCharacter(playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}
	else
	{
		for(new i = 0; i != 10; i++)
		{
			if(i < MAX_CHARACTER_PER_PLAYER)
			{
				if(characterTDID[playerid][CHAR_SELECTION][i] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_SELECTION][i]])
				{
					// S
					if (Iter_Contains(CharacterIndex[playerid], i))
					{
						// SpawnPlayer
						if (SetPlayerName(playerid, CharacterData[playerid][i][E_CHARACTER_NAME]) == -1)
						{
							Logger_Err("[OnPlayerClickPlayerTD / character/ui] Failed to SetPlayerName",
								Logger_P(playerid),
								Logger_S("Name", CharacterData[playerid][i][E_CHARACTER_NAME]));
						}
						else
						{
							Player_SetCharacterID(playerid, CharacterData[playerid][i][E_CHARACTER_ID]);
							Player_SetCharacterName(playerid, CharacterData[playerid][i][E_CHARACTER_NAME]);
							Player_SetCharacterRPName(playerid, CharacterData[playerid][i][E_CHARACTER_NAME]);
							StampCharacterLastLogin(playerid);

							Logger_Dbg(PLAYER_DEBUG_HANDLER, "player successfully login to character",
								Logger_P(playerid),
								Logger_I("uid", Player_GetAccountID(playerid)),
								Logger_I("cid", Player_GetCharacterID(playerid))
							);
	
							_CharacterUI_Clear(playerid);
							CallLocalFunction("OnPlayerLogin", "i", playerid);
						}
					}
					else
					{
						// Creation
						CharacterUI_Creation(playerid);
					}
					return Y_HOOKS_BREAK_RETURN_1;
				}
			}
			if(characterTDID[playerid][CHAR_CREATION_SKIN][i] != -1 && playertextid == characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN][i]])
			{
				// คืนสี
				if (characterLastSelectSkinSlot[playerid] != -1)
				{
					PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN][characterLastSelectSkinSlot[playerid]]], 0x87CEFA7D);
					PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN][characterLastSelectSkinSlot[playerid]]]);
				}

				characterSelectSkin[playerid] = characterSkinSlot[playerid][i];
				characterLastSelectSkinSlot[playerid] = i;

				new string[32];
				format(string, sizeof(string), "You choose Skin ID: %d", characterSelectSkin[playerid]);
				PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN_TEXT]], string);
				PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN_TEXT]]);

				PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN][i]], 0xFA8795EE);
				PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_SKIN][i]]);

				PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_SKIN]], "LD_CHAT:thumbup");
				PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_SKIN]]);

				return Y_HOOKS_BREAK_RETURN_1;
			}
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

ShowSetCharacterAgeDialog(playerid)
{

	inline const HandleCharacterAge(response, listitem, string:inputtext[])
	{
		#pragma unused listitem

		characterLastClickTick[playerid] = GetTickCount();

		if (!response) {
			@return 1;
		}

		new age = strval(inputtext);

		characterAge[playerid] = age;

		new string[4];
		format(string, sizeof(string), "%d", characterAge[playerid]);
		PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]], string);
		PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]]);

		if (age < 18 || age > 80)
		{
			PlayerTextDrawBoxColor(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]-1], -16777166);
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]-1]);

			PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_AGE]], "LD_CHAT:thumbdn");
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_AGE]]);
		}
		else
		{
			PlayerTextDrawBoxColor(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]-1], 852308615);
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_AGE]-1]);

			PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_AGE]], "LD_CHAT:thumbup");
			PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_AGE]]);
		}

		
	}

	new 
		string[71 + 16];

	static const HintMessage[] = "\
		"WHITE"ป้อนอายุตัวละครที่ต้องการด้านล่างนี้: \
		\n\
		"WHITE"- ระหว่าง 18 - 80 ปีเท่านั้น \
	";

    format(
        string,
        sizeof(string),
        HintMessage
    );

	Dialog_ShowCallback(
		playerid,
		using inline HandleCharacterAge,
		DIALOG_STYLE_INPUT,
		"ตั้งอายุตัวละคร",
		string,
		"เปลี่ยน",
		"ปิด"
	);
	return 1;
}

ShowSetCharacterNameDialog(playerid, e_ERROR_CHARACTER:errorCode = e_ERROR_CHARACTER_NONE)
{
	new
		error[75]
	;

	if (errorCode != e_ERROR_CHARACTER_NONE) {
		switch (errorCode) {
			case e_ERROR_CHARACTER_CASE: {
				format(error, sizeof error, ""INDIANRED"ตัวอักษรแรกของชื่อและนามสกุลต้องเป็นพิมพ์ใหญ่\n\n");
			}
			case e_ERROR_CHARACTER_LENGTH: {
				format(error, sizeof error, ""INDIANRED"ความยาวของชื่อทั้งหมดต้องไม่ต่ำกว่า 7 หรือมากกว่า 20 ตัวอักษร\n\n");
			}
			case e_ERROR_CHARACTER_NAME_LENGTH: {
				format(error, sizeof error, ""INDIANRED"ชื่อและนามสกุลต้องมีอย่างน้อย 3 ตัวอักษร\n\n");
			}
			case e_ERROR_CHARACTER_VALID_NAME: {
				format(error, sizeof error, ""INDIANRED"โปรดใช้ตัวอักษร A-z และ '_' ในการตั้งชื่อตัวละคร\n\n");
			}
		}
	}

	inline const HandleCharacterName(response, listitem, string:inputtext[])
	{
		#pragma unused listitem

		characterLastClickTick[playerid] = GetTickCount();

		if (!response) {
			@return 1;
		}

		new ret = Character_ValidName(inputtext);
		if (e_ERROR_CHARACTER:ret != e_ERROR_CHARACTER_NONE)
			@return ShowSetCharacterNameDialog(playerid, e_ERROR_CHARACTER:ret);

		strcpy(characterName[playerid], inputtext);
		PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_NAME]], characterName[playerid]);
		PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_NAME]]);

		PlayerTextDrawBoxColor(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_NAME]-1], 852308615);
		PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_NAME]-1]);

		PlayerTextDrawSetString(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_NAME]], "LD_CHAT:thumbup");
		PlayerTextDrawShow(playerid, characterTD[playerid][characterTDID[playerid][CHAR_CREATION_PROG_NAME]]);
	}

	new 
		string[229 + 88 + 75];

	static const HintMessage[] = "\
		%s \
		"GAMETEXT_BHHH"รูปแบบ: "WHITE"Firstname_Lastname \
		\n\n\
		"WHITE"- พิมพ์ใหญ่เฉพาะตัวอักษรแรกของชื่อและนามสกุล "ORANGE"F"WHITE"irstname_"ORANGE"L"WHITE"astname\n \
		"WHITE"- ต้องมี Underscore '_' ขั้นระหว่างชื่อและนามสกุล Firstname"ORANGE"_"WHITE"Lastname\n \
		"WHITE"- ความยาวของชื่อรวมทั้งหมดต้องไม่ต่ำกว่า 7 หรือเกิน 20 ตัวอักษร \
	";

    format(
        string,
        sizeof(string),
        HintMessage,
        error
    );

	Dialog_ShowCallback(
		playerid,
		using inline HandleCharacterName,
		DIALOG_STYLE_INPUT,
		"ตั้งชื่อตัวละคร",
		string,
		"ตั้งชื่อ",
		"ปิด"
	);
	return 1;
}

PerformPlayerCreationCharacter(playerid)
{
	if (characterName[playerid][0] == '\0' || (characterAge[playerid] < 18 || characterAge[playerid] >= 80) || characterSelectSkin[playerid] == 0)
	{
		if(Character_GetCount(playerid) == 0)
		{
			SendErrorMsg(playerid, "แย่แล้ว! เกิดข้อผิดพลาดบางอย่างโปรดลองใหม่อีกครั้ง");
		}
		else
		{
			inline const HandleExitingCreation(response, listitem, string:inputtext[])
			{
				#pragma unused listitem, inputtext

				characterLastClickTick[playerid] = GetTickCount();
				
				if (!response) {
					@return 1;
				}

				CharacterUI_Selection(playerid);
			}

			Dialog_ShowCallback(
				playerid,
				using inline HandleExitingCreation,
				DIALOG_STYLE_MSGBOX,
				"กลับสู่หน้าเลือกตัวละคร",
				"เกิดข้อผิดพลาดในการสร้างตัวละคร!! หรือคุณต้องการที่จะกลับไปยังหน้าเลือกตัวละคร?",
				"ใช่",
				"ไม่ต้องการ"
			);
		}
		return;
	}
	_CharacterUI_Clear(playerid);

    Character_InsertToDatabase(playerid, characterName[playerid]);
}

CharacterUI_GetSkin(playerid)
{
	return characterSelectSkin[playerid];
}

CharacterUI_GetAge(playerid)
{
	return characterAge[playerid];
}

// Character selection
CharacterUI_Clear(playerid)
{
	Iter_Clear(CharacterIndex[playerid]);
	for(new i = 0; i != MAX_CHARACTER_PER_PLAYER; i++)
	{
		CharacterData[playerid][i][E_CHARACTER_NAME][0] = '\0';
		CharacterData[playerid][i][E_CHARACTER_SKIN] = 0;
		CharacterData[playerid][i][E_CHARACTER_ID] = 0;
	}
}

CharacterUI_Add(playerid, character_id, character_skin, const character_name[])
{
    new id = Iter_Alloc(CharacterIndex[playerid]);

    if(id == ITER_NONE) {
        Logger_Err("MAX CHARACTER PER PLAYER REACHED",
			Logger_P(playerid),
            Logger_S("Name", character_name),
            Logger_I("ID", character_id));
        return ITER_NONE;
    }
	
	strcpy(CharacterData[playerid][id][E_CHARACTER_NAME], character_name);
	CharacterData[playerid][id][E_CHARACTER_SKIN] = character_skin;
	CharacterData[playerid][id][E_CHARACTER_ID] = character_id;

	return 1;
}

CharacterUI_Selection(playerid)
{
	_CharacterUI_Clear(playerid);

	new
		str[40],
		numChars = Iter_Count(CharacterIndex[playerid]),
		Float:td_posY = 121.0,
		Float:td_posX= 318.0 - (85.0 * float((numChars > 3 ? 3 : numChars) - ((numChars < 3) ? 0 : 1))) + (5.0 * float((numChars > 3 ? 3 : numChars) - ((numChars < 3) ? 0 : 1)))
	;

	foreach(new i : CharacterIndex[playerid])
	{
		characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, td_posX, td_posY, "_");
		PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.595833, 16.450000);
		PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255.500000, 148.500000);
		PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
		PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
		PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
		PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 70);
		PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

		format(str, sizeof(str), "%s", CharacterData[playerid][i][E_CHARACTER_NAME]);
		characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, td_posX - 73.0, td_posY, str);
		PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.183329, 0.849995);
		PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], td_posX + 82.0, 17.000000);
		PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
		PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
		PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
		PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

		characterTDID[playerid][CHAR_SELECTION][i] = charSelectTextDrawCount[playerid];
		characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, td_posX - 83.0, td_posY + 13.0, "Preview_Model");
		PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 5);
		PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 166.500000, 140.500000);
		PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
		PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
		PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetPreviewModel(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], CharacterData[playerid][i][E_CHARACTER_SKIN]);
		PlayerTextDrawSetPreviewRot(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -8.000000, 0.000000, -1.000000, 0.979999);
		PlayerTextDrawSetPreviewVehCol(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1, 1);

		td_posX += 160.0;

		if (i != 0 && i%2 == 0) {
			td_posY += 164;
			td_posX= 318.0 - (65.0 * float(numChars - 3) - ((numChars < 3) ? 0 : 1)) + (5.0 * float(numChars - 3) - ((numChars < 3) ? 0 : 1));
		}
	}

	if (numChars + 1 < MAX_CHARACTER_PER_PLAYER) {

		new i  = numChars + 1;
		characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, td_posX, td_posY, "_");
		PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.595833, 16.450000);
		PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255.500000, 148.500000);
		PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 2);
		PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
		PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
		PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 70);
		PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

		characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, td_posX - 73.0, td_posY, "NEW CHARACTER");
		PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.183329, 0.849995);
		PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], td_posX + 82.0, 17.000000);
		PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
		PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
		PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 50);
		PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 0);

		characterTDID[playerid][CHAR_SELECTION][i] = charSelectTextDrawCount[playerid];
		characterTD[playerid][charSelectTextDrawCount[playerid]] = CreatePlayerTextDraw(playerid, td_posX - 83.0, td_posY + 13.0, "Preview_Model");
		PlayerTextDrawFont(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 5);
		PlayerTextDrawLetterSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0.600000, 2.000000);
		PlayerTextDrawTextSize(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 166.500000, 140.500000);
		PlayerTextDrawSetOutline(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetShadow(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawAlignment(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -1);
		PlayerTextDrawBackgroundColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawBoxColor(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 255);
		PlayerTextDrawUseBox(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 0);
		PlayerTextDrawSetProportional(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetSelectable(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 1);
		PlayerTextDrawSetPreviewModel(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], 264);
		PlayerTextDrawSetPreviewRot(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]], -8.000000, 0.000000, -1.000000, 0.979999);
		PlayerTextDrawSetPreviewVehCol(playerid, characterTD[playerid][charSelectTextDrawCount[playerid]++], 1, 1);

		td_posX += 160.0;

		if (i != 0 && i%2 == 0) {
			td_posY += 164;
			td_posX= 318.0 - (65.0 * float(numChars - 3)) + (5.0 * float(numChars - 3));
		}
	}
				
	for (new i; i < charSelectTextDrawCount[playerid]; i++)
		PlayerTextDrawShow(playerid, characterTD[playerid][i]);

	SelectTextDraw(playerid, 0xFFFFFF95);
}