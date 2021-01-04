#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <freetype.h>
#include <ft2build.h> //字库


#include <iostream>
#include "lcd.h"
#include "disk.h"


using namespace std;

/*控制某行显示的内容 间隔32 * n */
enum display_line{
	CUR_TIME_LINE = 0, 
	OPEN_TIME_LINE, /*开启时间*/
	CLOSE_TIME_LINE, /*关闭时间*/
	CUR_TEMP_LINE, /*温度*/
	CUR_HUM_LINE, /*湿度*/
	CUR_WIRE_LINE, /*无线名称*/
	CUR_TOTAL_LINE, /*总行数*/
};

struct show_line_ctrl{
   const wchar_t *desc;
   int line;
   int start_data_row;
};

static struct show_line_ctrl show_line_ctrls[] = {
  {L"时间:", CUR_TIME_LINE, 3},  /*第3列开始显示时间值*/
  {L"开灯:", OPEN_TIME_LINE, 3},  /*第3列开始显示时间值*/
  {L"关灯:", CLOSE_TIME_LINE, 3},  /*第3列开始显示时间值*/
  {L"温度:", CUR_TEMP_LINE, 3},  /*第3列开始显示温度值*/
  {L"湿度:", CUR_HUM_LINE, 3},  /*第3列开始显示湿度值*/
  {L"无线:", CUR_WIRE_LINE, 3},  /*第3列开始显示网络名值*/
};



typedef uint32_t PIXEL_TYPE; /*像素点类型*/
typedef uint16_t PIXEL_TYPE_16; /*像素点类型*/


struct draw_info {
	int xres;
	int yres;
	int bpp;
	int line_word_length; /*一行的字数*/
};


struct freetype_data{
	FT_Library    library;
	FT_Face       face;
	FT_GlyphSlot  slot;	//用于指向face中的glyph
	FT_Vector     pen;                    
	FT_Error      error;
};

static struct draw_info draw_infos;
static struct freetype_data freetype_datas;



class FreeTypeOps{

public:	
	FreeTypeOps(int size); //类外定义函数必须在内部声明
	~FreeTypeOps()
	{
		FT_Done_Face(freetype_datas.face);
		FT_Done_FreeType(freetype_datas.library);
	}
};




/*
构造函数
*/
FreeTypeOps::FreeTypeOps(int size)
{

	int ret;
	char file_name[50] = "/usr/data/font/simsun.ttc";

	printf("%s %d \n",__func__,__LINE__);
//	ret = get_par("simsun",file_name,sizeof(file_name));
	if(ret) {
	//	printf("par simsun is no find \n");
	//	return -ERR_FILE_NONE;
	}
	
	//2.初始化库
	freetype_datas.error = FT_Init_FreeType( &freetype_datas.library );            
 
	//3.打开一个字体文件，并加载face对象:
	
	freetype_datas.error = FT_New_Face( freetype_datas.library, file_name, 0, &freetype_datas.face ); 
	freetype_datas.slot = freetype_datas.face->glyph;
	
	FT_Select_Charmap(freetype_datas.face,FT_ENCODING_UNICODE);

	
	//4.设置字体大小 32
	freetype_datas.error = FT_Set_Pixel_Sizes(freetype_datas.face, size, 0);//0表示和另一个尺寸相等 

}

static FreeTypeOps cur_ops(32); //定义类初始化



void lcd_draw_point(unsigned int x, unsigned int y, PIXEL_TYPE color, PIXEL_TYPE *lcd_ptr)
{
	int xres = draw_infos.xres;
	int yres = draw_infos.yres;
	int bpp = draw_infos.bpp;
	if(bpp == 4) {
		if( x>=0 && x < xres && y>=0 && y< yres ) {
			*(lcd_ptr + xres  * y + x) = color;
		}
	}else if(bpp == 2) {
		PIXEL_TYPE_16 *lcd_ptr_16 = (PIXEL_TYPE_16 *)lcd_ptr;
		if( x>=0 && x < xres && y>=0 && y< yres ) {
			*(lcd_ptr_16 + xres  * y + x) = color&0xffff;
		//	printf("x =%d y =%d \n",x,y);
		}
	}

	
}

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d)) //向上取整除法


/*清除这个位图所有格点*/
int Show_ClearType_Bitmap(FT_Bitmap*  bitmap,int start_x,int start_y,PIXEL_TYPE color, PIXEL_TYPE *lcd_buf_ptr)
{
	int  buff_x, buff_y;	//遍历bitmap时使用
	int  x,y;	//循环遍历使用
	int  end_x = start_x + bitmap->width;	//计算图片的结束X坐标
	int  end_y = start_y + bitmap->rows;	//计算图片的结束Y坐标
	
	/* (start_x/16)/16  向下取整 16单位 */
	start_x = 16*(start_x/16);
	/* bitmap->width/16 向上取整 16单位 */
	end_x = start_x + 16 * DIV_ROUND_UP(bitmap->width,16) + 2; //+2多擦除2列	
	/* start_y 先减去16 计算完毕补16  向下取整 除32 再乘以32 最后补16*/
	start_y = 32 * ((start_y - 16)/32) + 16;
	/* bitmap->rows/32 向上取整 32单位 */
	end_y = start_y  + 32 * DIV_ROUND_UP(bitmap->width,32);	
		
	int xres = draw_infos.xres;
	int yres = draw_infos.yres;
	int bpp = draw_infos.bpp;
	
	for ( x = start_x, buff_x = 0; x < end_x; buff_x++, x++ )	//y表示起点y，x表示起点x
	{
		
		for ( y = start_y, buff_y = 0; y < end_y; buff_y++, y++ )	//y表示起点y，x表示起点x
		{
			//LCD边界处理
			if ( x < 0 || y < 0 || x >= xres   || y >= yres )
				continue;
 
			lcd_draw_point(start_x+buff_x , start_y+buff_y ,  color, lcd_buf_ptr);
		
		}
	}
} 	


int Show_FreeType_Bitmap(FT_Bitmap*  bitmap,int start_x,int start_y,PIXEL_TYPE color, PIXEL_TYPE *lcd_buf_ptr)
{
	int  buff_x, buff_y;	//遍历bitmap时使用
	int  x,y;	//循环遍历使用
	int  end_x = start_x + bitmap->width;	//计算图片的结束X坐标
	int  end_y = start_y + bitmap->rows;	//计算图片的结束Y坐标
	
	int xres = draw_infos.xres;
	int yres = draw_infos.yres;
	int bpp = draw_infos.bpp;
	
	for ( x = start_x, buff_x = 0; x < end_x; buff_x++, x++ )	//y表示起点y，x表示起点x
	{
		
		for ( y = start_y, buff_y = 0; y < end_y; buff_y++, y++ )	//y表示起点y，x表示起点x
		{
			//LCD边界处理
			if ( x < 0 || y < 0 || x >= xres   || y >= yres )
				continue;
 
			//判断该位上是不是为1，1则表明需要描点
			if(bitmap->buffer[buff_y * bitmap->width + buff_x] ){	
				lcd_draw_point(start_x+buff_x , start_y+buff_y ,  color, lcd_buf_ptr);
			//	printf("(%d,%d)\n",start_x+buff_x,start_y+buff_y);
			}
			//在当前x位置加上p的偏移量（p表示buff中列的移动）
			//在当前y位置加上q的偏移量（q表示buff中行的移动）
			
		}
	}
} 



int Find_Max_Y(FT_Bitmap *bitmap)
{

	int x = 0 , y = bitmap->rows - 1; 
	for( ; y > 0; y--) {
		for( x = bitmap->width -1 ; x > 0; x--) {
			if(bitmap->buffer[y * bitmap->width + x])
				return y;
		}	
	}
	return y;
}


int Find_Min_Y(FT_Bitmap *bitmap)
{

	int x, y; 
	for(y = 0; y < bitmap->rows - 1; y++) {
		for(x = 0; x < bitmap->width -1; x++) {
			if(bitmap->buffer[y * bitmap->width + x])
				return y;
		}
	}
	return y;
}





/*
起始坐标 x是0 ,y 是 15
*/
int Lcd_Show_FreeType(wchar_t *wtext, int size, PIXEL_TYPE color, int start_x, int start_y, PIXEL_TYPE *lcd_buf_ptr)
{
 
	int n, ret;
	int min_y, max_y;

	//x起点位置：start_x。需要*64
	freetype_datas.pen.x = start_x  * 64;	//20*64
	//y起点位置：LCD高度 - start_y。需要*64
	freetype_datas.pen.y = start_y  * 64;
	
	
	//每次取出显示字符串中的一个文字
	for ( n = 0; n < wcslen( wtext ); n++ )	
	{
		//5.设置显示位置和旋转角度，0为不旋转，pen为提前设置好的坐标
		FT_Set_Transform( freetype_datas.face, 0, &freetype_datas.pen );

		//空格键跳过
		if(wtext[n] == ' ') { 
			
		//	printf("freetype_datas.slot->bitmap_left =%d freetype_datas.slot->bitmap_top=%d\n",freetype_datas.slot->bitmap_left,freetype_datas.slot->bitmap_top);
			freetype_datas.pen.x += freetype_datas.slot->advance.x/2;
			continue;
		}

		freetype_datas.error = FT_Load_Char( freetype_datas.face, wtext[n], FT_LOAD_RENDER|FT_LOAD_FORCE_AUTOHINT );
	
		//FT_LOAD_RENDER 表示转换RGB888的位图像素数据 没有这个会段错误
			//FT_LOAD_NO_BITMAP
			//FT_LOAD_VERTICAL_LAYOUT 垂直对齐
			//FT_LOAD_FORCE_AUTOHINT  用这个可以水平对齐
			
		//出错判断	
		if ( freetype_datas.error )
		  continue;                
		
		//向下对齐
		min_y = Find_Min_Y(&freetype_datas.slot->bitmap); 
		max_y = Find_Max_Y(&freetype_datas.slot->bitmap); 
		
	//	printf("min_y=%d max_y =%d slot->bitmap_top=%d \n",min_y,max_y,slot->bitmap_top);
		freetype_datas.slot->bitmap_top = start_y + 15 + size - 1 - max_y; //但是这样只能显示第一行
		//slot->bitmap_top  有点奇怪 a是0 b是9 c是0 d是9
		/*根据位图的地址 左 和 右*/
	//	printf("freetype_datas.slot->bitmap_left =%d freetype_datas.slot->bitmap_top=%d\n",freetype_datas.slot->bitmap_left,freetype_datas.slot->bitmap_top);
		//显示前清空这个格子
		Show_ClearType_Bitmap(&freetype_datas.slot->bitmap, freetype_datas.slot->bitmap_left, freetype_datas.slot->bitmap_top, 0, lcd_buf_ptr);
		Show_FreeType_Bitmap(&freetype_datas.slot->bitmap, freetype_datas.slot->bitmap_left, freetype_datas.slot->bitmap_top, color, lcd_buf_ptr);
		
		//增加笔位
		freetype_datas.pen.x += freetype_datas.slot->advance.x;
		
	}	
	
	return 0;
}





/**
 * show_free_type  显示字体
 * 注意: 显示时x 要是16的倍数 y是32的倍数 ,减少使用空格！！
 * Return:	0 表示成功
 */
int show_free_type(wchar_t *wtext, int size, PIXEL_TYPE color, int start_x, int start_y)
{
	int ret;
	const struct  fb_var_screeninfo *info = get_lcd_info();
	int xres = draw_infos.xres = info->xres;
	int yres = draw_infos.yres = info->yres;
	int bpp = draw_infos.bpp = info->bits_per_pixel/8;
	draw_infos.line_word_length = info->xres/32;
	

	uint8_t *show_buf = get_lcd_top_buffer();

	ret = Lcd_Show_FreeType(wtext, size, color, start_x, start_y, (PIXEL_TYPE *)show_buf);
	lcd_topview_write(show_buf);
	return ret;
}


/**
 * init_show_fix_char  初始化显示固定字体
 *
 * Return:	0 表示成功
 */
int init_show_fix_char(void)
{
	int i = 0;
	for(i = 0; i < CUR_TOTAL_LINE; i++) {
		const wchar_t *str = show_line_ctrls[i].desc;
		int row = 32 * show_line_ctrls[i].line;
		show_free_type((wchar_t *)str, 32, 0xf800, 0, row);
	}
	
	return 0;
}



