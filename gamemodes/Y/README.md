# Y Module

## ประกอบไปด้วย
- ``core`` - กำหนดค่าเริ่มต้น
- ``logger`` - ตั้งค่าล็อกไฟล์จาก samp-logger
- ``mysql`` - เชื่อมต่อฐานข้อมูล
- ``player`` - เกี่ยวกับตัวผู้เล่น ตั้งค่าทั่วไป

### core
- รอปรับปรุงเนื้อหา

### logger
- ``PLAYER_DEBUG_HANDLER``
- ``SERVER_DEBUG_HANDLER``
- ``ADMIN_DEBUG_HANDLER``

มี Log file ให้ทั้งหมด 3 ประเภท

##### ตัวอย่าง
```c
hook OnPlayerConnected(playerid)
{
    Logger_Dbg(PLAYER_DEBUG_HANDLER, "Player has connected",
        Logger_P(playerid),
        Logger_I("id", playerid)
    );
}
```

### mysql
- โมดูลหลักสำหรับการเชื่อมต่อฐานข้อมูล

ตั้งค่าการเชื่อมต่อ MySQL ให้คัดลอกไฟล์ mysql.ini.template และตั้งชื่อเป็น mysql.ini

##### hooked callbacks
- ``hook OnMySQLConnected()`` - เรียกเมื่อ Database เชื่อมต่อสำเร็จแล้ว

#### functions
- ``stock MySQL:MySQL_GetHandle()`` - ส่งค่าคืนเป็น Handle ที่เชื่อมต่อฐานข้อมูลไว้ 

##### ตัวอย่าง
```c
// Module House
hook OnMySQLConnected()
{
    mysql_tquery(MySQL_GetHandle(), "SELECT * FROM houses", "House_Load", "");
}
```


### player
- เกี่ยวกับตัวผู้เล่น ตั้งค่าทั่วไป

##### hooked callbacks
- ``hook OnPlayerChangeSkin(playerid, skinid, bool:save)`` - ถูกเรียกเมื่อมีการเปลี่ยนสกินตัวละครผ่านฟังก์ชั่น Player_SetSkin

#### functions
- ``stock Player_SetSkin(playerid, skinid, bool:save = false)`` - เปลี่ยนสกินตัวละคร parameter save ค่าเริ่มต้นคือ false
- ``stock Player_GetSkin(playerid)`` - ส่งค่ากลับเป็นไอดีสกินผู้เล่นไอดีนั้น ๆ
- ``stock Player_UpdateSkin(playerid, skinid)`` - อัพเดตไอดีสกินผู้เล่นโดยไม่เปลี่ยนสกินผู้เล่น ถูกใช้ตอนโหลดข้อมูลสกินใน Module Y:RP:player:character:skin
