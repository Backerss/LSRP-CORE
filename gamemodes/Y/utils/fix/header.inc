#if defined _inc_header
	#undef _inc_header
#endif

#define Kick(%0) SetTimerEx("kickfix", 40, false, "d", %0)
#define Ban(%0) SetTimerEx("banfix", 50, false, "d", %0)