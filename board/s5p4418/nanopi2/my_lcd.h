#ifndef __MY_LCD_H
#define __MY_LCD_H

#define CONFIG_FB_ADDR					0x46000000  //显存基地址


#if 0
//32章  基地址:0xc0101000
struct NX_DISPLAYTOP_RegisterSet 
{
	volatile U32 RESCONV_MUX_CTRL;		// 0x00
	volatile U32 INTERCONV_MUX_CTRL;	// 0x04
	volatile U32 MIPI_MUX_CTRL;			// 0x08
	volatile U32 LVDS_MUX_CTRL;			// 0x0C

	volatile U32 HDMIFIXCTRL0;			// 0x10
	volatile U32 HDMISYNCCTRL0;			// 0x14
	volatile U32 HDMISYNCCTRL1;			// 0x18
	volatile U32 HDMISYNCCTRL2;			// 0x1C
	volatile U32 HDMISYNCCTRL3;			// 0x20

	volatile U32 TFTMPU_MUX;            // 0x24
};


//33章  基地址:c0102000 
struct	NX_MLC_RegisterSet
{
	volatile U32 MLCCONTROLT;				///< 0x00 : MLC Top Control Register
	volatile U32 MLCSCREENSIZE;				///< 0x04 : MLC Screen Size Register
	volatile U32 MLCBGCOLOR;				///< 0x08 : MLC Background Color Register

	struct {
	volatile U32 MLCLEFTRIGHT;				///< 0x0C, 0x40 : MLC RGB Layer Left Right Register 0/1
	volatile U32 MLCTOPBOTTOM;				///< 0x10, 0x44 : MLC RGB Layer Top Bottom Register 0/1
	volatile U32 MLCINVALIDLEFTRIGHT0;		///< 0x14, 0x48 : MLC RGB Invalid Area0 Left, Right Register 0/1
	volatile U32 MLCINVALIDTOPBOTTOM0;		///< 0x18, 0x4C : MLC RGB Invalid Area0 Top, Bottom Register 0/1
	volatile U32 MLCINVALIDLEFTRIGHT1;		///< 0x1C, 0x50 : MLC RGB Invalid Area1 Left, Right Register 0/1
	volatile U32 MLCINVALIDTOPBOTTOM1;		///< 0x20, 0x54 : MLC RGB Invalid Area1 Top, Bottom Register 0/1
	volatile U32 MLCCONTROL;				///< 0x24, 0x58 : MLC RGB Layer Control Register 0/1
	volatile S32 MLCHSTRIDE;				///< 0x28, 0x5C : MLC RGB Layer Horizontal Stride Register 0/1
	volatile S32 MLCVSTRIDE;				///< 0x2C, 0x60 : MLC RGB Layer Vertical Stride Register 0/1
	volatile U32 MLCTPCOLOR;				///< 0x30, 0x64 : MLC RGB Layer Transparency Color Register 0/1
	volatile U32 MLCINVCOLOR;				///< 0x34, 0x68 : MLC RGB Layer Inversion Color Register 0/1
	volatile U32 MLCADDRESS;				///< 0x38, 0x6C : MLC RGB Layer Base Address Register 0/1
	volatile U32 __Reserved0;				///< 0x3C, 0x70 : Reserved Region

	} MLCRGBLAYER[2];

	struct {
	volatile U32 MLCLEFTRIGHT;				///< 0x74 : MLC Video Layer Left Right Register
	volatile U32 MLCTOPBOTTOM;				///< 0x78 : MLC Video Layer Top Bottom Register
	volatile U32 MLCCONTROL;				///< 0x7C : MLC Video Layer Control Register
	volatile U32 MLCVSTRIDE;				///< 0x80 : MLC Video Layer Y Vertical Stride Register
	volatile U32 MLCTPCOLOR;				///< 0x84 : MLC Video Layer Transparency Color Register
	//volatile U32 __Reserved1[1];			///< 0x88 : Reserved Region
	volatile U32 MLCINVCOLOR;				///< 0x88 : MLCINVCOLOR3 ( )
	volatile U32 MLCADDRESS;				///< 0x8C : MLC Video Layer Y Base Address Register
	volatile U32 MLCADDRESSCB;				///< 0x90 : MLC Video Layer Cb Base Address Register
	volatile U32 MLCADDRESSCR;				///< 0x94 : MLC Video Layer Cr Base Address Register
	volatile S32 MLCVSTRIDECB;				///< 0x98 : MLC Video Layer Cb Vertical Stride Register
	volatile S32 MLCVSTRIDECR;				///< 0x9C : MLC Video Layer Cr Vertical Stride Register
	volatile U32 MLCHSCALE;					///< 0xA0 : MLC Video Layer Horizontal Scale Register
	volatile U32 MLCVSCALE;					///< 0xA4 : MLC Video Layer Vertical Scale Register
	volatile U32 MLCLUENH;					///< 0xA8 : MLC Video Layer Luminance Enhancement Control Register
	volatile U32 MLCCHENH[4];				///< 0xAC, 0xB0, 0xB4, 0xB8 : MLC Video Layer Chrominance Enhancement Control Register 0/1/2/3

	} MLCVIDEOLAYER;

	struct {
	volatile U32 MLCLEFTRIGHT;				///< 0xBC : MLC RGB Layer Left Right Register 2
	volatile U32 MLCTOPBOTTOM;				///< 0xC0 : MLC RGB Layer Top Bottom Register 2
	volatile U32 MLCINVALIDLEFTRIGHT0;		///< 0xC4 : MLC RGB Invalid Area0 Left, Right Register 2
	volatile U32 MLCINVALIDTOPBOTTOM0;		///< 0xC8 : MLC RGB Invalid Area0 Top, Bottom Register 2
	volatile U32 MLCINVALIDLEFTRIGHT1;		///< 0xCC : MLC RGB Invalid Area1 Left, Right Register 2
	volatile U32 MLCINVALIDTOPBOTTOM1;		///< 0xD0 : MLC RGB Invalid Area1 Top, Bottom Register 2
	volatile U32 MLCCONTROL;				///< 0xD4 : MLC RGB Layer Control Register 2
	volatile S32 MLCHSTRIDE;				///< 0xD8 : MLC RGB Layer Horizontal Stride Register 2
	volatile S32 MLCVSTRIDE;				///< 0xDC : MLC RGB Layer Vertical Stride Register 2
	volatile U32 MLCTPCOLOR;				///< 0xE0 : MLC RGB Layer Transparency Color Register 2
	volatile U32 MLCINVCOLOR;				///< 0xE4 : MLC RGB Layer Inversion Color Register 2
	volatile U32 MLCADDRESS;				///< 0xE8 : MLC RGB Layer Base Address Register 2

	} MLCRGBLAYER2;

	volatile U32 MLCPALETETABLE2;               ///< 0xEC
	volatile U32 MLCGAMMACONT;					///< 0xF0 : MLC Gama Control Register
	volatile U32 MLCRGAMMATABLEWRITE;			///< 0xF4 : MLC Red Gamma Table Write Register
	volatile U32 MLCGGAMMATABLEWRITE;			///< 0xF8 : MLC Green Gamma Table Write Register
	volatile U32 MLCBGAMMATABLEWRITE;			///< 0xFC : MLC Blue Gamma Table Write Register
	volatile U32 YUVLAYERGAMMATABLE_RED;        ///< 0x100:
	volatile U32 YUVLAYERGAMMATABLE_GREEN;      ///< 0x104:
	volatile U32 YUVLAYERGAMMATABLE_BLUE;       ///< 0x108:
	// @modified charles 2010/01/28 Dynamic Dimming 眠啊
	volatile U32 DIMCTRL;                       ///< 0x10C
	volatile U32 DIMLUT0;                       ///< 0x110
	volatile U32 DIMLUT1;                       ///< 0x114
	volatile U32 DIMBUSYFLAG;                   ///< 0x118
	volatile U32 DIMPRDARRR0;                   ///< 0x11C
	volatile U32 DIMPRDARRR1;                   ///< 0x120
	volatile U32 DIMRAM0RDDATA;                 ///< 0x124
	volatile U32 DIMRAM1RDDATA;                 ///< 0x128
	volatile U32 __Reserved2[(0x3C0-0x12C)/4];	///< 0x12C ~ 0x3BC : Reserved Region
	volatile U32 MLCCLKENB;						///< 0x3C0 : MLC Clock Enable Register

};


//34章  基地址:0xc0102800
struct	NX_DPC_RegisterSet
	{
		volatile U32 __Reserved[(0x07C-0x00)/2];///< 0x00-0x078*2 Reserved Regions
		volatile U32 DPCHTOTAL		;			///< 0x07C*2 : DPC Horizontal Total Length Register
		volatile U32 DPCHSWIDTH		;			///< 0x07E*2 : DPC Horizontal Sync Width Register
		volatile U32 DPCHASTART		;			///< 0x080*2 : DPC Horizontal Active Video Start Register
		volatile U32 DPCHAEND		;			///< 0x082*2 : DPC Horizontal Active Video End Register
		volatile U32 DPCVTOTAL		;			///< 0x084*2 : DPC Vertical Total Length Register
		volatile U32 DPCVSWIDTH		;			///< 0x086*2 : DPC Vertical Sync Width Register
		volatile U32 DPCVASTART		;			///< 0x088*2 : DPC Vertical Active Video Start Register
		volatile U32 DPCVAEND		;			///< 0x08A*2 : DPC Vertical Active Video End Register
		volatile U32 DPCCTRL0		;			///< 0x08C*2 : DPC Control 0 Register
		volatile U32 DPCCTRL1		;			///< 0x08E*2 : DPC Control 1 Register
		volatile U32 DPCEVTOTAL		;			///< 0x090*2 : DPC Even Field Vertical Total Length Register
		volatile U32 DPCEVSWIDTH	;			///< 0x092*2 : DPC Even Field Vertical Sync Width Register
		volatile U32 DPCEVASTART	;			///< 0x094*2 : DPC Even Field Vertical Active Video Start Register
		volatile U32 DPCEVAEND		;			///< 0x096*2 : DPC Even Field Vertical Active Video End Register
		volatile U32 DPCCTRL2		;			///< 0x098*2 : DPC Control 2 Register
		volatile U32 DPCVSEOFFSET	;			///< 0x09A*2 : DPC Vertical Sync End Offset Register
		volatile U32 DPCVSSOFFSET	;			///< 0x09C*2 : DPC Vertical Sync Start Offset Register
		volatile U32 DPCEVSEOFFSET	;			///< 0x09E*2 : DPC Even Field Vertical Sync End Offset Register
		volatile U32 DPCEVSSOFFSET	;			///< 0x0A0*2 : DPC Even Field Vertical Sync Start Offset Register
		volatile U32 DPCDELAY0		;			///< 0x0A2*2 : DPC Delay 0 Register
		volatile U32 DPCUPSCALECON0 ;			///< 0x0A4*2 : DPC Sync Upscale Control Register 0
		volatile U32 DPCUPSCALECON1 ;			///< 0x0A6*2 : DPC Sync Upscale Control Register 1
		volatile U32 DPCUPSCALECON2 ;			///< 0x0A8*2 : DPC Sync Upscale Control Register 2
		//@modified choiyk 2012-10-09 坷傈 11:10:34 : ( reference - NXP3200, LF2000, mes_disp03.h )
		volatile U32 DPCRNUMGENCON0 ;           ///< 0x0AA*2 : DPC Sync Random number generator control register 0
		volatile U32 DPCRNUMGENCON1 ;           ///< 0x0AC*2 : DPC Sync Random number generator control register 1
		volatile U32 DPCRNUMGENCON2 ;           ///< 0x0AE*2 : DPC Sync Random number generator control register 2
	    volatile U32 DPCRNDCONFORMULA_L;        ///< 0x0B0*2
	    volatile U32 DPCRNDCONFORMULA_H;        ///< 0x0B2*2
		volatile U32 DPCFDTAddr     ;           ///< 0x0B4*2 : DPC Sync frame dither table address register
		volatile U32 DPCFRDITHERVALUE;          ///< 0x0B6*2 : DPC Sync frame red dither table value register
		volatile U32 DPCFGDITHERVALUE;          ///< 0x0B8*2 : DPC Sync frame green dither table value register
		volatile U32 DPCFBDITHERVALUE;          ///< 0x0BA*2 : DPC Sync frame blue table value register
        volatile U32 DPCDELAY1       ;          ///< 0x0BC*2 : DPC Delay 1 Register
        volatile U32 DPCMPUTIME0     ;          ///< 0x0BE*2   @todo Find Databook of these registers..
        volatile U32 DPCMPUTIME1     ;          ///< 0x0C0*2
        volatile U32 DPCMPUWRDATAL   ;          ///< 0x0C2*2
        volatile U32 DPCMPUINDEX     ;          ///< 0x0C4*2
        volatile U32 DPCMPUSTATUS    ;          ///< 0x0C6*2
        volatile U32 DPCMPUDATAH     ;          ///< 0x0C8*2
        volatile U32 DPCMPURDATAL    ;          ///< 0x0CA*2
        volatile U32 DPCDummy12      ;	        ///< 0x0CC*2
        volatile U32 DPCCMDBUFFERDATAL;         ///< 0x0CE*2
        volatile U32 DPCCMDBUFFERDATAH;         ///< 0x0D0*2
        volatile U32 DPCPOLCTRL      ;          ///< 0x0D2*2
        volatile U32 DPCPADPOSITION[8];         ///< 0x0D4*2~0x0E2*2(4,6,8,A,C,E,0,2)
        volatile U32 DPCRGBMASK[2]   ;          ///< 0x0E4*2~0x0E6*2(4,6)
        volatile U32 DPCRGBSHIFT     ;          ///< 0x0E8*2
        volatile U32 DPCDATAFLUSH    ;		    ///< 0x0EA*2
		volatile U32 __Reserved06[( (0x3C0) - (2*0x0EC) )/4];		///< 0x0EC*2 ~ (0x3C0)
		// CLKGEN
		volatile U32 DPCCLKENB		;			///< 0x3C0 : DPC Clock Generation Enable Register
		volatile U32 DPCCLKGEN[2][2];			///< 0x3C4 : DPC Clock Generation Control 0 Low Register
												///< 0x3C8 : DPC Clock Generation Control 0 High Register
												///< 0x3CC : DPC Clock Generation Control 1 Low Register
												///< 0x3D0 : DPC Clock Generation Control 1 High Register
	};

#endif
void my_lcd_init(void);


#endif




