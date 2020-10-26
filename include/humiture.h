#ifndef	__HUMITURE__H
#define __HUMITURE__H



#define TEMP_PATH  "/sys/devices/platform/dht11/iio:device1/in_temp_input"
#define HUDTY_PATH  "/sys/devices/platform/dht11/iio:device1/in_humidityrelative_input"

#define HT_SACN_TIME 	2   //扫描间隔秒

int humiture_init(void);
float humiture_get_temp(void);
float humiture_get_hudiy(void);



#endif















