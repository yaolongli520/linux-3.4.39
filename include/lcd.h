#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>
#include <stdlib.h>



#define max(x, y) (((int)(x) > (int)(y)) ? x : y)
#define min(a, b) (((a) < (b)) ? (a) : (b))

int set_lcd_rotate(int rotate);
int get_lcd_rotate(void);


int lcd_init(void);



typedef uint32_t PIXEL_TYPE; /*像素点类型*/

const struct  fb_var_screeninfo *get_lcd_info(void);
int lcd_backdrop_write(uint8_t *src);
int lcd_topview_write(uint8_t *src);
int backdrop_show_bmp(void); //BMP背景
int backdrop_show_jpg(void); //JPG背景
int show_free_type(wchar_t *wtext, int size, PIXEL_TYPE color, int start_x, int start_y);
uint8_t * get_lcd_top_buffer(void);

int lcd_combine_write(void); //合并显示

void  raw_lcd_write(uint8_t *buf); //测试用



#endif




