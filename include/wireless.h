#ifndef __WIRELESS_H_
#define __WIRELESS_H_


enum lint_status{
	DEVICE_DOWN = 0, /*断开*/
	DEVICE_UNPLUGGED, /*未插上*/
	DEVICE_LINKED, /*已连接*/
	DEVICE_NONE, /*没有设备*/
};

int get_net_device_status(void);














#endif 


