# RP Module

## ประกอบไปด้วย
- ``player`` - เกี่ยวกับผู้เล่นทั้งบัญชีและตัวละครผู้เล่นนั้น ๆ

##### hooked callbacks
- ``hook OnPlayerPassedBanCheck(playerid)`` - ถูกเรียกเมื่อผ่านการตรวจสอบผู้ที่ถูกแบนในตารางฐานข้อมูลด้วยชื่อที่เข้าเซิร์ฟเวอร์มา
- ``hook OnPlayerAccountLogin(playerid)`` - ถูกเรียกเมื่อผู้เล่นได้เข้าสู่ระบบบัญชีผู้ใช้และเข้าสู่หน้าเลือกตัวละครต่อไป
- ``hook OnCharacterCreated(playerid, cID)`` - เมื่อตัวละครถูกสร้างขึ้นในเกมและมี parameter ไอดีตัวละครถูกส่งมาด้วย
อาจใช้สำหรับสร้างสถานะตัวละครนั้น ๆ ใน Module อื่น ๆ เช่น สถิติ/ไอเทมเริ่มต้น (อาจไม่ได้ใช้เพราะตัวละครจะถูกสร้างบน UCP)
- ``hook OnPlayerFirstSpawn(playerid)`` - ถูกเรียกเมื่อผู้เล่น Spawn เป็นครั้งแรกที่เข้าเซิร์ฟเวอร์
- ``hook OnPlayerLogin(playerid)`` - ถูกเรียกเมื่อผู้เล่นเลือกตัวละครและไอดีตัวละครได้ถูกตั้งค่าไว้แล้ว (สามารถใช้ Player_GetCharacterID(playerid) ดึงไอดีตัวละครได้)
#### functions
- ``Player_GetAccountID(playerid)`` - ส่งค่ากลับเป็นไอดีบัญชีที่ถูกตั้งไว้ (ค่าเริ่มต้น -1)
- ``Player_GetCharacterID(playerid)`` - ส่งค่ากลับเป็นไอดีตัวละครที่ถูกตั้งไว้ (ค่าเริ่มต้น -1)
- ``Player_GetCharacterName(playerid)`` - ส่งค่ากลับเป็นชื่อตัวละคร
- ``Player_GetCharacterRPName(playerid)`` - ส่งค่ากลับเป็นชื่อตัวละคร (ไม่มี underscore)