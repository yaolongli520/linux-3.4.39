#ifndef	__HUMITURE__H
#define __HUMITURE__H




#define DHT11_FILE	"/dev/dht11"

struct dht11_data {
	int  temp;
	int  hum;
};


#define HT_SACN_TIME 	4   //扫描间隔秒

int humiture_init(void);
float humiture_get_temp(void);
float humiture_get_hudiy(void);



#endif















