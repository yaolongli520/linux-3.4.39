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
#include "jpeglib.h" //JPG库

using namespace std;




#define PARM_BMP_LOGO "bmp_logo"
#define PARM_JPG_LOGO "jpg_logo"

#define BMP_HEAD_LEN 54


/**
 * show_bmp  背景写入BMP 并不会立刻起作用!!!
 *
 * 成功 return 0
 */
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
	
	return 0;
}



/**
 * show_jpg  背景写入JPG  并不会立刻起作用!!!
 *
 * 成功 return 0
 */
int show_jpg(uint8_t	*data, unsigned int size)
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;		
	char 	*pcolor_buf; 
	unsigned char 	*pjpg ;
	unsigned int 	i=0,j=0;
	unsigned int 	jpg_size = size; //图像大小
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	int jw = 0, jh = 0, jbpp = 0; //jpg参数
	int pw = 0, ph = 0, pbpp = 0; //屏幕参数
	const struct  fb_var_screeninfo *info = get_lcd_info();
	
	/* 指向JPG图像内存空间 */	
	pjpg = (unsigned char *)data;

	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	

	/*图片参数*/
	jw = cinfo.output_width;
	jh = cinfo.output_height;
	jbpp = 3;
	char g_color_buf[jw * jh * 3]; //JPG解码行缓存
	memset(g_color_buf, 0, sizeof(g_color_buf));


	/*屏幕参数*/
	pw = info->xres;
	ph = info->yres;
	pbpp = info->bits_per_pixel/8;	
	uint8_t show_buf[pw *ph *pbpp];

	//宽	 高	 bpp
	if(jw!=pw || jh!=ph || jbpp!=pbpp) {
		cout <<"jw:"<<jw<<" jh"<<jh<<" jbpp"<<jbpp<<endl;
		cout <<"pw:"<<pw<<" ph"<<ph<<" pbpp"<<pbpp<<endl;
	}	

	/*读解码数据*/
	while(cinfo.output_scanline < cinfo.output_height ) //循环次数是高度
	{		
		pcolor_buf = g_color_buf;//获取行缓冲区地址 
		
		/* 读取jpg一行的rgb值 */
		jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++) //处理这一行
		{

			if((pbpp * 8) == 32) {	
				uint32_t *p32 = (uint32_t *)show_buf;
				p32[i + j * pw] = (*(pcolor_buf+2))| ((*(pcolor_buf+1))<<8)|((*(pcolor_buf))<<16);
			}else if((pbpp * 8) == 16) {
				uint16_t *p16 = (uint16_t *)show_buf;
				p16[i + j * pw] = (((*(pcolor_buf+2)) >> 3)& 0x1f) | ((((*(pcolor_buf+1))>>2)&0x3f) <<5) | ((((*(pcolor_buf))>>3)&0x1f) << 11);
			}		
			/* 显示像素点 */			
			pcolor_buf +=3; //下一个点
		}
		j++;
		/* 换行 */	
	}

	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);	

	lcd_backdrop_write(show_buf);

}



/**
 * backdrop_show  背景写入图片
 * @type 图片类型
 *
 * 成功 return 0
 */
int backdrop_show(const char *type)
{
	int ret = 0, pic_fd;;
	char pic_type[30] = {0}; //LOGO 类型
	char pic_file[30] = {0}; //LOGO 所在文件路径
	uint8_t	*ppic;
	uint32_t 	pic_size;
	struct stat statbuff;//文件描述
	
	if(!strcmp(type,"BMP")) {
		strcpy(pic_type,PARM_BMP_LOGO); //BMP
	}else if(!strcmp(type,"JPG")) {
		strcpy(pic_type,PARM_JPG_LOGO); //JPG
	}else {
		cout << "picture type "<<type <<"err"<<endl;
		return -ERR_FILE_OPS_INVALID;
	}

	/*获取文件路径*/
	ret = get_par(pic_type, pic_file, sizeof(pic_file));
	if(ret == 0) {
		printf("%s =%s \n",pic_type,(char *)pic_file);
	}else {
		return -ERR_PARM_NOT_SET;
	}

	pic_fd = open(pic_file,O_RDWR);
	if(pic_fd == -1) {
		cout <<"file "<<pic_file <<"not find"<<endl;
		return -ERR_FILE_NONE;
	}

	if(stat(pic_file, &statbuff) < 0) {
		cout <<"file "<<pic_file <<"error size"<<endl;
		return -ERR_FILE_NOT_SIZE;
	}else
		pic_size = statbuff.st_size; //获取文件总大小

	ppic = (uint8_t	*)malloc(pic_size);
	if(ppic == NULL) {
		cout <<"malloc pic error"<<endl;
		return -ERR_MALLOC;
	}

	read(pic_fd,ppic,pic_size);//整体读出
	
	if(!strcmp(type,"BMP")) {
		show_bmp(ppic); //BMP
	}else if(!strcmp(type,"JPG")) {
		show_jpg(ppic,pic_size); //JPG 需要提供文件大小
	}else {
		cout << "picture type "<<type <<"err"<<endl;
		return -ERR_FILE_OPS_INVALID;
	}

	free(ppic);
	close(pic_fd);
	
	return 0;
}


/**
 * backdrop_show_bmp  背景写入BMP
 *
 * 成功 return 0
 */

int backdrop_show_bmp(void)
{	
	return backdrop_show("BMP");	
}



/**
 * backdrop_show_jpg  背景写入jpg
 *
 * 成功 return 0
 */

int backdrop_show_jpg(void)
{	
	return backdrop_show("JPG");	
}



