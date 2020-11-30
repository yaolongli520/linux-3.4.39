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






#endif




