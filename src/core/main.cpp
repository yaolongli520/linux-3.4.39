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
#include "cmd_ops.h"


using namespace std;

void *pth_show(void *data);
void *pth_nrf_server(void *data);
void *pth_net_socket_accept(void *data);




int main(int argc,char *argv[])
{
	int ret;
	int hum;
	int val = 0;
	pthread_t pth_showid;
	pthread_t pth_nrfid;
	pthread_t pth_netid;


	pr_init("project ctrl board start\n");
	
	ret = param_init();
	if(ret) {
		cout <<"param_init fail\n";
	}
	ret = ioctrl_init(); /*IO控制*/
	if(ret) {
		cout <<"ioctrl_init fail\n";
	}

	ret = lcd_init();
	if(ret) {
		cout <<"lcd_init fail\n";
	}

	ret = human_init();
	if(ret) {
		cout <<"human_init fail\n";
	}

	ret = humiture_init();
	if(ret) {
		cout <<"humiture_init fail\n";
	}

	ret = nrf24l01_init();
	if(ret) {
			cout <<"nrf24l01_init fail\n";
	}

	/*命令初始化*/
	ret = net_cmd_ops_init();
	if(ret){
		pr_err("cmd ops  is init fail \n");
	}


	/* 显示线程 */
	ret = pthread_create(&pth_showid, NULL,
					   pth_show,NULL);
	if(ret)
		printf("pth_show is create fail \n");

	/* 启动无线服务器 */
	ret = pthread_create(&pth_nrfid, NULL,
					   pth_nrf_server,NULL);
	if(ret)
		printf("pth_nrf_server is create fail \n");

	/* 网络线程 */
	ret = pthread_create(&pth_netid, NULL,
					   pth_net_socket_accept,NULL);
	if(ret)
		pr_err("pth_net_server is create fail \n");	



	/*主线程*/
	while(1){
		hum = get_hunman_status();
		if(hum == TARGET_EXIST) set_io_val("LCD_BLK",1);
		else set_io_val("LCD_BLK",0);

	//	set_io_val("IO_BUZZER",val);
		val = !val;
		sleep(1);

	}

	return 0;
}

