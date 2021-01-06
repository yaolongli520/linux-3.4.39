#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>




#include "mytime.h"
#include "disk.h"
#include "ioctrl.h"
#include "lcd.h"
#include "human.h"
#include "humiture.h"
#include "nrf24l01.h"
#include "wireless.h"

using namespace std;


/**
 * pth_nrf_server  无线服务器线程
 *  return 0
 */
void *pth_nrf_server(void *data)
{
	int ret = nrf24l01_find_dev();
	if(ret) {
		printf("nrf24l01 not find cur pth is exit \n");
		return NULL;
	}

	//pack_module_init(); //初始化链式缓存
	
	while(1)
	{
		sleep(2);
		//printf("%s %d \n",__func__,__LINE__);

	}


}





