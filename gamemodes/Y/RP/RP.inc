#if defined _inc_RP
	#undef _inc_RP
#endif

#if !defined Y_RP_HEADER

	#include "core\header"
	#include "player\header"
	#include "utils\header"

	#define Y_RP_HEADER
#elseif !defined Y_RP_END

	#include "core\impl"
	#include "player\impl"
	#include "utils\impl"

	#define Y_RP_END
#else
	#error Y rp already set up
#endif