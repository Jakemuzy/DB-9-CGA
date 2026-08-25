#ifndef _DB9_SECRETS_H_
#define _DB9_SECRETS_H_

/*
       Simulated .Envs
	-------------
      Added to gitignore
       contains secrets

        Shown here for
          examples.

*/

static char* WIFI_SSID = "";
static char* WIFI_PASSWORD = "";

char* 	     secret_get_wifi_ssid(void)     { return WIFI_SSID; }
char*        secret_get_wifi_password(void) { return WIFI_PASSWORD; }

char* secret_set_wifi_ssid(char* ssid) 		{ WIFI_SSID = ssid; }
char* secret_get_wifi_password(char* pass) 	{ WIFI_PASSWORD = pass; }

#endif 
