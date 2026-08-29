#ifndef DB9_SECRETS_H_
#define DB9_SECRETS_H_

/*
       Simulated .Envs
	-------------
      Added to gitignore
       contains secrets

        Shown here for
          examples.

*/

char* secret_get_wifi_ssid(void);
char* secret_get_wifi_password(void);

void  secret_set_wifi_ssid(char* ssid);
void  secret_set_wifi_password(char* pass);

#endif 
