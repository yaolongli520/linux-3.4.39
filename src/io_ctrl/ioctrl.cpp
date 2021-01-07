#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <iostream>
#include "ioctrl.h"
#include "disk.h"
using namespace std;




struct io_ctrl {
	const char *file_name;
	bool cur_val;
	int fd;
};

static struct io_ctrl cur_io[] = {
	{
		.file_name = "/dev/LCD_BLK",
		.cur_val	= 1,
	},
	{
		.file_name = "/dev/RGB_BLK",
		.cur_val	= 1,
	},
	{
		.file_name = "/dev/IO_BUZZER",
		.cur_val	= 0,
	},
	{
		.file_name = "/dev/IO_SW1",
		.cur_val	= 1,
	},
	{
		.file_name = "/dev/IO_SW2",
		.cur_val	= 1,
	},	
};


/**
 * ioctrl_init  获取引脚开关状态
 * @name: 获取的引脚名称
 * Return:	0 表示成功  
 */

int get_io_val(const char *name)
{
	char *p = NULL;
	int i;
	if(strlen(name) > IO_NAME_MAX) {
		return value_err;
	}

	for(i = 0; i < ARRAY_SIZE(cur_io); i++) {
		p = strstr((char *)cur_io[i].file_name,(char *)name);
		if(!p) 
			continue;
		else   
			break;
	}

	if(i >= ARRAY_SIZE(cur_io)) {
		printf("name %s io is not find \n",name);
		return value_err;
	}

	if(cur_io[i].cur_val) 
		return value_on;
	else
		return value_off;

}



/**
 * ioctrl_init  获取引脚开关状态
 * @name: 获取的引脚名称
 * Return:	0 表示成功  
 */

int set_io_val(const char *name,int val)
{
	char *p = NULL;
	int i;
	char c_val;
	
	if(strlen(name) > IO_NAME_MAX) {
		return value_err;
	}

	for(i = 0; i < ARRAY_SIZE(cur_io); i++) {
		p = strstr((char *)cur_io[i].file_name,(char *)name);
		if(!p) 
			continue;
		else   
			break;
	}

	if(i >= ARRAY_SIZE(cur_io)) {
		printf("name %s io is not find \n",name);
		return value_err;
	}

	cur_io[i].cur_val = val;
	if(val) 
		c_val = ON_STR;
	else 
		c_val = OFF_STR;
	
	write(cur_io[i].fd,&c_val,sizeof(c_val));
	printf("%s val =%d \n",__func__,val);
	return 0;
}


/**
 * ioctrl_init  初始化IO控制
 *
 * Return:	0 表示成功  
 */
int ioctrl_init(void)
{
	int ret;
	char on =  '1';
	char off = '0';
	for(int i = 0; i < ARRAY_SIZE(cur_io); i++) {
		ret = open(cur_io[i].file_name,O_RDWR);
		if(ret < 0) {
			printf("open %s is fail\n",cur_io[i].file_name);
			cur_io[i].fd = -1;
			continue;
		}
		cur_io[i].fd = ret;	
		if(cur_io[i].cur_val)
			write(cur_io[i].fd,&on,sizeof(on));
		else
			write(cur_io[i].fd,&off,sizeof(off));
	}
	
	return 0;
}


