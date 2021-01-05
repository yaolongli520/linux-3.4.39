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
 * pth_show_data  显示线程负责刷新显示屏参数
 *  return 0
 */
void *pth_show_data(void *data)
{
	char usr_buf[200] = { 0 };
	wchar_t wt_text[] = L"23:23:23";
	
	//backdrop_show_bmp(); //背景bmp显示
	backdrop_show_jpg(); //背景jpg显示
	init_show_fix_char();
	lcd_combine_write(); //合并显示
	sleep(1);
	
	while(1)
	{

		/*实时时间*/
		get_cur_time(TIME_GET_TIME,usr_buf); 
		mbstowcs(wt_text, usr_buf, strlen(usr_buf));
		show_free_type(wt_text, 32, 0x1f, 80, 0);

		/*开灯时间*/
		get_par("opening_time",usr_buf,sizeof(usr_buf));
		mbstowcs(wt_text, usr_buf, strlen(usr_buf));
		show_free_type(wt_text, 32, 0x1f, 80, 32);
		
		/*关灯时间*/
		get_par("closeing_time",usr_buf,sizeof(usr_buf));
		mbstowcs(wt_text, usr_buf, strlen(usr_buf));
		show_free_type(wt_text, 32, 0x1f, 80, 64);

		/*温度*/ 
		memset(usr_buf, 0, sizeof(usr_buf));
		memset(wt_text, 0, sizeof(wt_text));
		sprintf(usr_buf, "%.2f C", humiture_get_temp());
		mbstowcs(wt_text, usr_buf, strlen(usr_buf));
		show_free_type(wt_text, 32, 0x1f, 80, 96);

		/*湿度*/
		memset(usr_buf, 0, sizeof(usr_buf));
		memset(wt_text, 0, sizeof(wt_text));
		sprintf(usr_buf, "%.1f R H", humiture_get_hudiy());
		mbstowcs(wt_text, usr_buf, strlen(usr_buf));
		show_free_type(wt_text, 32, 0x1f, 80, 128);

		
		lcd_combine_write(); //合并显示
		sleep(1);
	}
}


int main(int argc,char *argv[])
{
	int ret;
	int hum;
	int val = 0;
	pthread_t pth_showid;

	
	cout <<"I am main "<<endl;
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

	ret = pthread_create(&pth_showid, NULL,
					   pth_show_data,NULL);
	if(ret)
		printf("pth_show_data is create fail \n");

	ret = get_net_device_status();
	printf("get_net_device_status =%d \n",ret);
	/*主线程作为显示*/
	while(1){
		hum = hunman_get_satus();
	//	if(hum == 1) printf("has human \n");
	//	else printf("none human \n");

	//	printf("cur temp =%f \n",humiture_get_temp());
	//	printf("cur hudiy =%f \n",humiture_get_hudiy());
	//	set_io_val("IO_BUZZER",val);
		val = !val;
		sleep(1);

	}

	return 0;
}

