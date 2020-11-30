#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include <iostream>
#include "lcd.h"
#include "disk.h"
using namespace std;


struct lcd_data{
	char dev_name[20];
	int fd;
	bool is_init;
	struct  fb_var_screeninfo fbinfo; /*显示信息*/
	uint8_t *backdrop; /*背景*/
	uint8_t	*topview; /* 上层窗口*/
};

static struct lcd_data cur_lcd;


/**
 * lcd_combine_write  合并两层显示内容
 *  return 0
 */
int lcd_combine_write(void)
{
	int bpp = cur_lcd.fbinfo.bits_per_pixel/8;
	uint16_t	*p16b, *p16t;
	uint32_t 	*p32b, *p32t;
	uint32_t 	 size = cur_lcd.fbinfo.xres * cur_lcd.fbinfo.yres; 
	
	
	if(cur_lcd.is_init == false)
		return -ERR_FILE_NONE;


	if(bpp == 2) {
		uint16_t buf[size];
		p16b = (uint16_t *)cur_lcd.backdrop;
		p16t = (uint16_t *)cur_lcd.topview;	
		for(int i = 0; i < size; i++) {	
			/*顶层不为0 设置为顶层*/
			buf[i] = (p16t[i])?(p16t[i]):(p16b[i]);
		}
		lseek(cur_lcd.fd , 0 , SEEK_SET); //文件头
		write(cur_lcd.fd,buf,sizeof(buf));
	}else if(bpp == 4) {
		uint32_t buf[size];
		p32b = (uint32_t *)cur_lcd.backdrop;
		p32t = (uint32_t *)cur_lcd.topview;	
		for(int i = 0; i < size; i++) {	
			/*顶层不为0 设置为顶层*/
			buf[i] = (p32t[i])?(p32t[i]):(p32b[i]);
		}
		lseek(cur_lcd.fd , 0 , SEEK_SET); //文件头
		write(cur_lcd.fd,buf,sizeof(buf));
	}else {
		cout <<"fail bpp "<<endl;
	}

}


/**
 * get_lcd_rotate  获取LCD旋转角度
 *
 * Return:	ROTA_0	ROTA_90 ROTA_180 ROTA_270
 */
int get_lcd_rotate(void)
{
	int rotate = cur_lcd.fbinfo.rotate;

	if(cur_lcd.is_init == false)
		return -ERR_FILE_NONE;

	if(rotate == 0) 
		return ROTA_0;
	else if(rotate == 90) 
		return ROTA_90;
	else if(rotate == 180) 
		return ROTA_180;
	else if(rotate == 270) 
		return ROTA_270;

	cout <<__func__<<" rotate is error"<<endl;
	return -1;
}


/**
 * set_lcd_rotate  设置LCD旋转角度
 *
 * Return:	0
 */
int set_lcd_rotate(int rotate)
{
	int ret = 0;
	int pixel_max, pixel_min;
	int rotate_raw = cur_lcd.fbinfo.rotate;

	if(cur_lcd.is_init == false)
		return -ERR_FILE_NONE;

	if(rotate_raw == rotate) 
		return ret;

	pixel_max = max(cur_lcd.fbinfo.xres, cur_lcd.fbinfo.yres);
	pixel_min = min(cur_lcd.fbinfo.xres, cur_lcd.fbinfo.yres);

	/*横屏*/
	if((rotate == ROTA_0) ||(rotate == ROTA_180)){
		cur_lcd.fbinfo.xres = pixel_max;
		cur_lcd.fbinfo.yres = pixel_min;
		cur_lcd.fbinfo.rotate = rotate;
	}

	/*竖屏*/
	if((rotate == ROTA_90) ||(rotate == ROTA_270)){
		cur_lcd.fbinfo.xres = pixel_min;
		cur_lcd.fbinfo.yres = pixel_max;
		cur_lcd.fbinfo.rotate = rotate;
	}

	ret = ioctl(cur_lcd.fd,FBIOPUT_VSCREENINFO, &cur_lcd.fbinfo);
	if(ret < 0)
		return -1;

	return 0;
}



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
		int size;
		cur_lcd.fd = fd;
		ret = ioctl(cur_lcd.fd,FBIOGET_VSCREENINFO , &cur_lcd.fbinfo); 
		if(ret < 0) {
			cout <<"FBIOGET_VSCREENINFO is fail "<<endl;
			return -ERR_FILE_OPS_INVALID;
		}

		size = cur_lcd.fbinfo.xres * cur_lcd.fbinfo.yres *
			cur_lcd.fbinfo.bits_per_pixel/8;
		
		cur_lcd.backdrop = (uint8_t *)malloc(size);
		cur_lcd.topview = (uint8_t *)malloc(size);
		/*分配空间*/
		if((cur_lcd.backdrop==NULL) || (cur_lcd.topview == NULL)){
			cout <<"malloc is fail "<<endl;
			return -ERR_FILE_OPS_INVALID;
		}

		cur_lcd.is_init = true;
	}
	
	return 0;
}


