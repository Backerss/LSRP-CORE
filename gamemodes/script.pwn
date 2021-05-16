/**
    Y-Core

    Github: https://github.com/aktah/Y-Core

    TODO: ????
    
*/

#include <a_samp>

// YSI
#define CGEN_MEMORY (20000) // needs looking at, no clue why we had to increase this. YSI said so.
#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_YES_VERSION_CHECK
#define YSI_NO_CACHE_MESSAGE

#define YSI_NO_HEAP_MALLOC
#define YSI_NO_MODE_CACHE

// Libraries
#include "sscanf2"


#include "Pawn.RakNet"
#include "cec"

// Legacy Includes

#include "config"

#include "Y\Y"

#if SETUP_TABLE
	#include "install"
#endif

#include "Y\Y"

main() { }

public OnGameModeInit()
{
    // ...
    return 1;
}