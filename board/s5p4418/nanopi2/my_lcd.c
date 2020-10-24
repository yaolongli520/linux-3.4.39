#include <common.h> //延时
#include "my_lcd.h"


#define Top_nRST  9 	//TOP IP
#define DualDispla_y_nRST 10  // MLC和DPC IP 

#define CTURE   1
#define CFALSE  0

//IP复位寄存器  
#define IP_RESET_REGISTER  *(unsigned long int*)(0xc0012000)


#define set_bit(val,n)  (val|=(0x1<<n))
#define clr_bit(val,n)  (val&=~(0x1<<n))

/* 
把 date写入 偏移off的位置 
并清空 移off开始的连续n位
*/


#define write_date_to_bit(val,n,date,off) val = (val & ~(((0x1<<n) - 1)<<off))|((date)<<off)




/*32章 TOP */
struct NX_DISPLAYTOP_RegisterSet *TOP =(struct NX_DISPLAYTOP_RegisterSet*)(0xc0101000);
/*33章 MLC */
struct NX_MLC_RegisterSet *MLC =(struct NX_MLC_RegisterSet*)(0xc0102000);
/*34章 DPC */
struct NX_DPC_RegisterSet *DPC =(struct NX_DPC_RegisterSet*)(0xc0102800);




void my_lcd_init(void)
{
	int layer = 0;
	//复位 TOP
	clr_bit(IP_RESET_REGISTER,Top_nRST);//RESET
	mdelay(1);
	set_bit(IP_RESET_REGISTER,Top_nRST);//NO RESET
	
	//复位 DPC 和 MLC
	clr_bit(IP_RESET_REGISTER,DualDispla_y_nRST);//RESET
	mdelay(1);
	set_bit(IP_RESET_REGISTER,DualDispla_y_nRST);//NO RESET

	printf("TOP=%p MLC=%p DPC=%p\n",TOP,MLC,DPC);
	printf("MLC->MLCCLKENB=%p\n",&MLC->MLCCLKENB);

	//MLC 总是使能 PCIK BCLK
	MLC->MLCCLKENB |=(0x1<<3|0x3);
	//DPC 总是启用PCLK
	DPC->DPCCLKENB |=(0x1<<3);
	//VIDEO 优先级 layer0 > layer1 > video layer > layer2
	write_date_to_bit(MLC->MLCCONTROLT,2,0x2,8);
	
	//背景颜色
	MLC->MLCBGCOLOR = 0x000000;
	//用渐进式
	MLC->MLCCONTROLT &=~(0x1<<0);
	//关闭RGB GAMMA 的电源
	MLC->MLCGAMMACONT &=~((0x1<<11)|(0x1)<<9|(0x1)<<3);
	//RGB区域伽玛关闭 禁用抖动操作时  选择RGB伽马
	MLC->MLCGAMMACONT &=~(0x3|(0x1)<<5);
	
	MLC->MLCCONTROLT |=(0x1<<11); //开启MLC电源
	MLC->MLCCONTROLT &=~(0x1<<10);//开启睡眠模式
	
	
	//locksize = 16
	write_date_to_bit(MLC->MLCRGBLAYER[0].MLCCONTROL,
	2,0x2,12);
	
	MLC->MLCRGBLAYER[layer].MLCCONTROL &=~((0x1<<2)|(0x1)<<4);
	MLC->MLCRGBLAYER[layer].MLCTPCOLOR |=(0xf<<28);
	MLC->MLCRGBLAYER[layer].MLCCONTROL &=~((0x1<<0)|(0x1)<<4);
	MLC->MLCRGBLAYER[layer].MLCTPCOLOR &=0xf0000000;
	
	MLC->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT0 = 0x00000000;
	MLC->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM0 = 0x00000000;
	MLC->MLCRGBLAYER[layer].MLCINVALIDLEFTRIGHT1 = 0x00000000;
	MLC->MLCRGBLAYER[layer].MLCINVALIDTOPBOTTOM1 = 0x00000000;
	
	//A8R8G8B8
	write_date_to_bit(MLC->MLCRGBLAYER[layer].MLCCONTROL,
	16,0x0653,16);
	
	//左上角 x
	write_date_to_bit(MLC->MLCRGBLAYER[layer].MLCLEFTRIGHT,
	12,0,16);
	//右下角 x
	write_date_to_bit(MLC->MLCRGBLAYER[layer].MLCLEFTRIGHT,
	12,480-1,0);
	
	//左上角 y
	write_date_to_bit(MLC->MLCRGBLAYER[layer].MLCTOPBOTTOM,
	12,0,16);
	//右下角 y
	write_date_to_bit(MLC->MLCRGBLAYER[layer].MLCTOPBOTTOM,
	12,272-1,0);
	
	MLC->MLCRGBLAYER[layer].MLCHSTRIDE = 4; //一个点的字节
	MLC->MLCRGBLAYER[layer].MLCVSTRIDE = 480*4 ;//一行的字节
	
    MLC->MLCRGBLAYER[layer].MLCADDRESS = 0x46000000;

	MLC->MLCCONTROLT &=~(0x1<<3);//清空脏
	MLC->MLCCONTROLT |= (0x1<<1);//使能MLC
	MLC->MLCCONTROLT |= (0x1<<3);//设置脏
	
	
	
	MLC->MLCRGBLAYER[layer].MLCCONTROL &=~(0x1<<4);//清空脏
	MLC->MLCRGBLAYER[layer].MLCCONTROL |= (0x1<<5);
	MLC->MLCRGBLAYER[layer].MLCCONTROL |= (0x1<<4);//设置脏
	
	DPC->DPCCLKGEN[0][0] &=~(0x7<<2);
	write_date_to_bit(DPC->DPCCLKGEN[0][0],3,0x2,2);//pll2
	write_date_to_bit(DPC->DPCCLKGEN[0][0],8,80-1,5);//分频
	
	write_date_to_bit(DPC->DPCCLKGEN[1][0],3,0x7,2);//CLKGEN0时钟
	write_date_to_bit(DPC->DPCCLKGEN[1][0],8,0x0,5);//不分频
	
	DPC->DPCCLKGEN[0][1] &= ~(0x1f); 
	DPC->DPCCLKGEN[1][1] &= ~(0x1f); 
	
	DPC->DPCCTRL0 |=(0x1<<12); //RGB模式
	DPC->DPCCTRL1 |=(0x1<<6 |0x1<<13 );
	write_date_to_bit(DPC->DPCCTRL1,4,0x2,8); //RGB666
	
	DPC->DPCCTRL2 &=~(0x1<<4); //NO I80
	DPC->DPCCTRL2 &=~(0x3<<7); //TFT或视频编码器
	DPC->DPCCTRL2 &=~(0x3<<0);//VCLK
	
	DPC->DPCCLKGEN[0][0] |=(0x1<<1);//反转(上升沿)
	DPC->DPCCLKGEN[1][0] |=(0x1<<1);//反转(上升沿)
	
	DPC->DPCHTOTAL = 2+40+480+5-1;
	DPC->DPCHSWIDTH = 2-1;
	DPC->DPCHASTART = 2+40-1;
	DPC->DPCHAEND = 2+40+480-1;
	DPC->DPCCTRL0 &=~(0x1<<0|0x1<<10);//行同步极性
	
	DPC->DPCVTOTAL = 2+8+272+8-1;
	DPC->DPCVSWIDTH = 2-1;
	DPC->DPCVASTART = 2+8-1;
	DPC->DPCVAEND = 2+8+272-1;
	DPC->DPCEVTOTAL = 2+8+272+8-1;
	DPC->DPCEVSWIDTH = 2-1; 
	DPC->DPCEVASTART = 2+8-1;
	DPC->DPCEVAEND = 2+8+272-1;
	DPC->DPCCTRL0 &= ~(0x1<<1); //帧同步极性
	
	
	DPC->DPCVSEOFFSET = 1;
	DPC->DPCVSSOFFSET = 1;
	DPC->DPCEVSEOFFSET = 1;
	DPC->DPCEVSSOFFSET = 1;
	
	DPC->DPCCTRL0 &= ~(0xf<<4);
	write_date_to_bit(DPC->DPCDELAY0,6,7,8);//7
	write_date_to_bit(DPC->DPCDELAY0,6,7,0);//7
	write_date_to_bit(DPC->DPCDELAY1,6,7,0);
	
	write_date_to_bit(DPC->DPCCTRL1,2,3,4);//3R
	write_date_to_bit(DPC->DPCCTRL1,2,3,2);//3G
	write_date_to_bit(DPC->DPCCTRL1,2,3,0);//3B
	
	write_date_to_bit(MLC->MLCSCREENSIZE,12,480-1,0);//宽 
	write_date_to_bit(MLC->MLCSCREENSIZE,12,272-1,16);//高 
	
	DPC->DPCCTRL0 |=(0x1<<15);//使能DPC
	DPC->DPCCLKENB |=(0x1<<2); //使能DPC时钟
	
	TOP->TFTMPU_MUX &=~(0x3<<0); //复用 TFT LCD控制0
}

































