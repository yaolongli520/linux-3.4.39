#ifndef __CMD_H_
#define __CMD_H_
#include "pack.h"


#define NRF_SERVER	0 /*服务器*/	
#define NRF_CLIENT	1 /*客户端*/	

#define CMD_TIMEOUT  500000000  //500MS
#define RETRY_NUM 	 2  //重试次数

#define TRAN_SIZE_MAX	512  //单次最大传输 32*16 字节

#define TRAN_CMD_SIZE	32



#define DATA_LEN		28

#define GET_U16_HIGH(val)  ((val >> 8) & 0xff)
#define GET_U16_LOW(val)   (val & 0xff)


enum {
	ERR_NONE = 0, /*没有出错*/
	ERR_SND_CMD, /*发送命令失败*/
	ERR_SND_DATA, /*发送数据失败*/
	ERR_NO_REPLY, /*没有回复数据*/
};



//cmd list 
enum {
	CMD_SETPAR, /* 设置参数 */
	CMD_GETPAR, /* 获取参数 */
	CMD_GET_LOSS, /*获取丢包*/
	CMD_UP_FILE, /* 上传文件 */
	CMD_DOWN_FILE, /* 下载文件 */
	CMD_HEAD_FAIL, /*头错误*/
	CMD_COMPLETION, /*完成*/
	CMD_NODEFINE, /* 未定义 */
};

struct get_par_str{
	uint32_t index;
	uint8_t  data[24]; 
};

struct nrf_cmd {
	uint32_t cmd_num;
	uint8_t  *cmd_data;
};

enum {
	NET_TYPE_NULL = 0x21, /*空*/
	NET_TYPE_CMD, /* 命令 */
	NET_TYPE_DATA, /*数据包*/
	NET_TYPE_NODEFINE, /* 未定义 */
};



/*远程设置命令回复,对于获取参数直接返回数据*/
enum CMD_REP{
	CMD_REP_NONE = 0x11, /*无此命令*/
	CMD_REP_YES, 		/*设置成功*/
	CMD_REP_NO, 		/*设置或获取失败*/
};

/*远程命令索引*/
enum CMD_INDEX{
	SET_OPEN_TIME,	/*设置开灯时间*/
	GET_OPEN_TIME,  /*获取开灯时间*/
	SET_CLOSE_TIME,	/*设置关灯时间*/
	GET_CLOSE_TIME,  /*获取关灯时间*/	
	SET_BEIJING_TIME,  /*设置北京时间*/
	GET_BEIJING_TIME,  /*获取北京时间*/ 
	GET_LIGHT_STATUS, /*获取当前开关状态*/
	CAL_TEMP,	/*校准温度*/
	GET_TEMP,	/*获取温度*/
	CAL_HUM,	/*校准湿度*/
	GET_HUM,	/*获取湿度*/
	GET_CUR_IP_ADDR, /*获取设备IP地址*/
	GET_CUR_WLAN_NAME, /*获取设备无线名称*/
	CONNET_TO_WLAN, /*连接到无线 仅NRF24L01*/
	GET_HUMMEN_CTRL, /*获取人体红外房灯控制状态*/
	SET_HUMMEN_CTRL, /*设置人体红外房灯控制状态*/
	SET_ROOM_LIGHT_OPEN, /*设置房灯开 一定时间 单位s*/
	SET_ROOM_LIGHT_CLOSE, /*取消房灯开时间 */
	REBOOT_DEVICE, /*重启设备*/
	CMD_INDEX_MAX, /*命令结束*/
};


#define CTRL_TIME_LEN  8



#endif


