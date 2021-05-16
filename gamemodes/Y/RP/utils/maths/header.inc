#if defined _inc_header
	#undef _inc_header
#endif

#define PAGES(%0,%1) \
	(((%0) - 1) / (%1) + 1)