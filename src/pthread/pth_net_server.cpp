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
 * pth_net_server  网络服务器线程
 *  return 0
 */
void *pth_net_server(void *data)
{
	while(1)
	{
		sleep(1);
	//	printf("%s %d \n",__func__,__LINE__);

	}



}








