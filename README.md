### Custom DB-9 CGA video output converter

Written for the Samsung Santron SC 452C, tailored to its horizontal and vertical refresh rate.

Input a video stream to an the ESP32, and it will automaticallly convert into readable signals by the DB-9 connector.


Supports various dynamic ESP32 systems, automatically detecting the hardware clock rates. All that is required is to correctly set your pins outs for DB-9 CGA pins 1-9.

Additionally supports qemu emulator for testing and further development.
