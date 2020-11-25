#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>
#include <stdlib.h>



#define max(x, y) (((int)(x) > (int)(y)) ? x : y)
#define min(a, b) (((a) < (b)) ? (a) : (b))

int set_lcd_rotate(int rotate);
int get_lcd_rotate(void);


int lcd_init(void);

enum {
	ROTA_0 = 0,
	ROTA_90, 
	ROTA_180, 
	ROTA_270,
};


const struct  fb_var_screeninfo *get_lcd_info(void);
int lcd_backdrop_write(uint8_t *src);
int lcd_topview_write(uint8_t *src);
int backdrop_show_bmp(void);
int lcd_combine_write(void); //合并显示

void  raw_lcd_write(uint8_t *buf); //测试用



#endif




