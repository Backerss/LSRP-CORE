#if defined _inc_Y
	#undef _inc_Y
#endif

#if !defined Y_@@@@_HEADER

	#include "core\header"
	#include "logger\header"

	#include "RP\RP"

	#define Y_@@@@_HEADER
#elseif !defined Y_@@@@_END

	#include "core\impl"
	#include "logger\impl"

	#include "RP\RP"

	#define Y_@@@@_END
#else
	#error y core already set up
#endif