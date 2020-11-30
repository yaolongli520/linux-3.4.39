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




#define PARM_BMP_LOGO "bmp_logo"

#define BMP_HEAD_LEN 54

int show_bmp(uint8_t	*data)
{
	int i,j;
	uint32_t ch1;
	uint8_t *head = data;//头
	uint8_t *p = data + BMP_HEAD_LEN;//图片数据	
	int bw = 0, bh = 0, bbpp = 0; //bmp参数
	int pw = 0, ph = 0, pbpp = 0; //屏幕参数
	const struct  fb_var_screeninfo *info = get_lcd_info();

	bw  = *((int *)&head[18]);				
	bh  = *((int *)&head[22]);
	bbpp  = *((int *)&head[28])/8;

	pw = info->xres;
	ph = info->yres;
	pbpp = info->bits_per_pixel/8;

	uint8_t show_buf[pw *ph *pbpp];
	memset(show_buf,0,sizeof(show_buf));

	uint32_t *p32 = (uint32_t *)show_buf;
	uint16_t *p16 = (uint16_t *)show_buf;


	//宽	 高	 bpp
	if(bw!=pw || bh!=ph || bbpp!=pbpp) {
		cout <<"bw:"<<bw<<" bh"<<bh<<" bbpp"<<bbpp<<endl;
		cout <<"pw:"<<pw<<" ph"<<ph<<" pbpp"<<pbpp<<endl;
	}

	
	if((pbpp * 8) == 32) {	
		for(i=0; i < pw * ph; i++)
			p32[i] = p[3*i]<<0|(p[3*i+1]<<8)|(p[3*i+2]<<16); 
	}else if((pbpp * 8) == 16) {
		for(i=0; i < pw * ph; i++)	//24->>16图 BGR --> RGB
		p16[i] = (((p[3*i]>>3)&0x1f)<<0) | (((p[3*i+1]>>2)&0x3f)<<5) | (((p[3*i+2]>>3)&0x1f)<<11);
	}
	
	#if 1  //上下翻转
	if(pbpp == 32) {	
		for(i=0; i< ph/2; i++)
			for(j=0;j<pw;j++) {
			    ch1 = p32[j + pw * i]; 
			    p32[j + pw * i] = p32[j + (ph-i) * pw];		
			    p32[j + (ph-i) * pw ]	= ch1;
			}
	}else if(pbpp == 16) {
		for(i=0;i<ph/2;i++)
			for(j=0;j<pw;j++) {
				ch1 = p16[j + pw * i ]; 
				p16[j + pw * i] = p16[j + (ph-i) * pw];		
				p16[j +(ph-i) * pw] = ch1;
			}
	}
	#endif 	

	lcd_backdrop_write(show_buf);
	
}


/**
 * backdrop_show_bmp  背景写入BMP
 * 成功 return 0
 */
int backdrop_show_bmp(void)
{
	int ret = 0, bmp_fd;;
	char bmp_file[30] = {0};
	uint8_t	*pbmp;
	uint32_t 	bmp_size;
	struct stat statbuff;//文件描述

	
	ret = get_par(PARM_BMP_LOGO, bmp_file, sizeof(bmp_file));
	if(ret == 0) {
		printf("%s =%s \n",PARM_BMP_LOGO,(char *)bmp_file);
	}else {
		return -ERR_PARM_NOT_SET;
	}

	bmp_fd = open(bmp_file,O_RDWR);
	if(bmp_fd == -1) {
		cout <<"file "<<bmp_file <<"not find"<<endl;
		return -ERR_FILE_NONE;
	}

	if(stat(bmp_file, &statbuff) < 0) {
		cout <<"file "<<bmp_file <<"error size"<<endl;
		return -ERR_FILE_NOT_SIZE;
	}else
		bmp_size = statbuff.st_size; //获取文件总大小

	pbmp = (uint8_t	*)malloc(bmp_size);
	if(pbmp == NULL) {
		cout <<"malloc bmp error"<<endl;
		return -ERR_MALLOC;
	}

	read(bmp_fd,pbmp,bmp_size);//整体读出
	
	show_bmp(pbmp);

	free(pbmp);
	close(bmp_fd);
	
	return 0;
}





