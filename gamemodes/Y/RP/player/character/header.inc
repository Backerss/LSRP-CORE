#if defined _inc_header
	#undef _inc_header
#endif

#include "ui\header"
#include "stats\header"
#include "location\header"
#include "skin\header"
#include "spawn\header"
#include "vip\header"

#include <YSI_Players\y_groups>

new
	Group:CharacterSelectedGroup
;

#define Character_IsLoggedIn(%0) Group_GetPlayer(CharacterSelectedGroup, %0)

enum e_ERROR_CHARACTER {
	e_ERROR_CHARACTER_NONE,
	e_ERROR_CHARACTER_CASE,
	e_ERROR_CHARACTER_LENGTH,
	e_ERROR_CHARACTER_NAME_LENGTH,
	e_ERROR_CHARACTER_VALID_NAME
}

#define	CHARACTER_TABLE	"characters"