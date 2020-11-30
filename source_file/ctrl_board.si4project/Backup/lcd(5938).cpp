#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>
#include "lcd.h"
#include "disk.h"
using namespace std;


struct lcd_data{
	char dev_name[20];
	int fd;
	bool is_init;
};

static struct lcd_data cur_lcd;








/**
 * lcd_init  初始化显示屏
 *
 * Return:	0 表示成功      ERR_FILE_NONE 打开失败
 */

int lcd_init(void)
{
	int fd;
	int ret;
	char file_name[30] = {0};
	ret = get_par("fbdev",file_name,sizeof(file_name));
	if(ret) {
		printf("par fbdev is no find \n");
		return -ERR_FILE_NONE;
	}
	strcpy(cur_lcd.dev_name,file_name);
	sprintf(file_name,"/dev/%s",cur_lcd.dev_name);

	fd = open(file_name,O_RDWR);
	if(fd < 0) {
		printf("lcd file %s open fail\n",file_name);
		cur_lcd.is_init = false;
		return -ERR_FILE_NONE;
	} else {
		cur_lcd.fd = fd;
		cur_lcd.is_init = true;
	}
	
	return 0;
}


