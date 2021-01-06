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
 * pth_show  显示线程负责刷新显示屏参数
 *  return 0
 */
void *pth_show(void *data)
{
	int ret;
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

		/*无线*/
		ret = get_net_device_status();
		if(ret != DEVICE_LINKED) {
			show_free_type((wchar_t *)L"未连接", 32, 0x1f, 80, 160);
		}else {
			FILE *fpr;
			char *essid;
			char *essid_end;
			memset(usr_buf, 0, sizeof(usr_buf));
			memset(wt_text, 0, sizeof(wt_text));
			fpr = popen("iwconfig wlan0 | grep \"ESSID\"", "r");
			fread(usr_buf, 1, sizeof(usr_buf), fpr);
			essid = strstr(usr_buf,"ESSID");
			essid += 7;
			essid_end = strchr(essid,'\"');
			*essid_end = 0;
			mbstowcs(wt_text, essid, strlen(essid));
			show_free_type(wt_text, 32, 0x1f, 80, 160);
			pclose(fpr);
		}
		
		lcd_combine_write(); //合并显示
		sleep(1);
	}
}





