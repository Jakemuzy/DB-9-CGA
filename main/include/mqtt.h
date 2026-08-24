#ifndef MQTT_H_
#define MQTT_H_

/* 
   NOTES:
	- This entire file will run on a separate thread in order to not interrupt the main display thread
	- Its main purpose is to intercept / send out notifications
	- Received notifications (via MQTT) will be interpretted as on screen messages
	- Sent messages are strictly for responses
	- Received messages are ONLY in a blob binary format, meaning the first thread can just display them once they are written to the secondary buffer

*/

void InitializeMQQTTClient();

void ReceiveBlob();
void ReceiveConfig();
void SendBlob();


#endif 
