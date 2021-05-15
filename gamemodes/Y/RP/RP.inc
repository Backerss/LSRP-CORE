#if defined _inc_RP
	#undef _inc_RP
#endif

#if !defined Y_RP_HEADER

	#include "core\header"

	#define Y_RP_HEADER
#elseif !defined Y_RP_END

	#include "core\impl"

	#define Y_RP_END
#else
	#error Y rp already set up
#endif