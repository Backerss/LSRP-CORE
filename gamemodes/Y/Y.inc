#if defined _inc_Y
	#undef _inc_Y
#endif

#if !defined Y_@@@@_HEADER

	#include "anti-cheat\header"
	#include "core\header"
	#include "logger\header"
	#include "discord\header"
	#include "mysql\header"
	#include "utils\header"
	#include "chat\header"
	#include "player\header"

	#include "RP\RP"

	#define Y_@@@@_HEADER
#elseif !defined Y_@@@@_END

	#include "anti-cheat\impl"
	#include "core\impl"
	#include "logger\impl"
	#include "discord\impl"
	#include "mysql\impl"
	#include "utils\impl"
	#include "chat\impl"
	#include "player\impl"

	#include "RP\RP"

	#define Y_@@@@_END
#else
	#error y core already set up
#endif