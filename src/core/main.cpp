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

using namespace std;



int main(int argc,char *argv[])
{
	int ret;
	int hum;
	int val = 0;
	char usr_buf[200] = { 0 };
	
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
			cout <<"humiture_init fail\n";
	}


	wchar_t wb_text[] = L"时间:";
	wchar_t wt_text[] = L"23:16:12";


//	backdrop_show_bmp(); //背景bmp显示
	backdrop_show_jpg(); //背景jpg显示
	
	show_free_type(wb_text, 32, 0xf800, 0, 32); //显示字符时间
	lcd_combine_write(); //合并显示




	
	

	while(1){
		hum = hunman_get_satus();
	//	if(hum == 1) printf("has human \n");
	//	else printf("none human \n");

	//	printf("cur temp =%f \n",humiture_get_temp());
	//	printf("cur hudiy =%f \n",humiture_get_hudiy());
	//	set_io_val("IO_BUZZER",val);
		val = !val;
		get_cur_time(TIME_GET_TIME,usr_buf); //获取时间
		mbstowcs(wt_text, usr_buf, strlen(usr_buf));//转换成宽字符串
		show_free_type(wt_text, 32, 0x1f, 80, 32);//写入顶层缓存
		lcd_combine_write(); //合并显示
		sleep(1);

	}

	return 0;
}

