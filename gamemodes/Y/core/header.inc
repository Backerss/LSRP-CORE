#if defined _inc_header
	#undef _inc_header
#endif

#include <YSI_Coding\y_hooks>
#include <YSI_Players\y_groups>
#include <YSI_Server\y_colours>
#include <YSI_Visual\y_commands>
#include <YSI_Coding\y_va>
#include <YSI_Core\y_master>


// OO-STYLE
#define OO_TYPE(%6,%7)%0[%1] (_:@vb:@va:%6:%7@%0[%1]
#define @va:%6:%7@%0[%1][@%2] %7%0[%1])[E_%6_%0_%2]
#define @vb:@va:%6:%7@%0[%1][%2][@%3] %7%0[%1])[%2][E_%6_%0_%3]

/*
    ตัวอย่าง:

	// ระบุ Module Player ให้สามารถใช้ OO TYPE ได้
	#define Player. OO_TYPE(PLAYER,Player)

    enum E_PLAYER_INVENTORY
    {
        E_PLAYER_Inventory_item,
        E_PLAYER_Inventory_qty
    };

    new PlayerInventory[MAX_PLAYERS][E_PLAYER_INVENTORY];

    enum E_PLAYER_PHONE
    {
        E_PLAYER_Phone_number,
        bool:E_PLAYER_Phone_off
    };

    new PlayerPhone[MAX_PLAYERS][E_PLAYER_PHONE];

    impl -> เอาไปใช้

    Player.Inventory[playerid][@item] = 1;
	Player.Inventory[playerid][@qty] = 1;
	Player.Phone[playerid][@number] = 99999;

*/
