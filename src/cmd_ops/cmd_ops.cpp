#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "cmd_ops.h"
#include "disk.h"
#include "mytime.h"
#include "humiture.h"
#include "net.h"


static struct cmd_ops_str  cur_cmd_ops_str;




/**
 * net_cmd_set_par  设置参数
 * @index 参数索引
 * @data 命令数据
 * @len 数据长度
 * Return:	
 */
int net_cmd_set_par(const char *par_name, uint8_t *data, uint16_t len)
{
	int ret = 0;
	
	pthread_mutex_lock(&cur_cmd_ops_str.cmd_mutex);
	ret =  set_par((const char *)par_name, (const char *)data, len);
	ret = file_save_cfgparam();
	pthread_mutex_unlock(&cur_cmd_ops_str.cmd_mutex);
	return ret;
}

/**
 * net_cmd_set_par_time  设置时间类参数
 * @index 参数索引
 * @data 命令数据
 * @len 数据长度
 * Return:	成功 0
 */
int net_cmd_set_par_time(const char *par_name, uint8_t *data, uint16_t len)
{
	int ret = 0;
	ret = check_par_len(par_name, (char *)data);
	if(ret) 
		return ret; 

	ret = check_par_time((char *)data);
	if(ret) 
		return ret;	

	ret = net_cmd_set_par(par_name, data, len);

	return ret;
}


/**
 * net_cmd_set_beijing_time  设置北京时间参数
 * @index 参数索引
 * @data 命令数据
 * @len 数据长度
 * Return:	成功 0
 */
int net_cmd_set_beijing_time(const char *par_name, uint8_t *data, uint16_t len)
{
	int ret = 0;
	ret = check_par_len(par_name, (char *)data);
	if(ret) 
		return ret; 

	ret = check_par_time((char *)data);
	if(ret) 
		return ret;	

	ret = net_cmd_set_par(par_name, data, len);

	return ret;
}




/**
 * net_cmd_handle  执行命令并返回响应
 * @cmd 命令数据
 * @rep 响应数据 仅保存响应数据和长度 比如时间 19:50:00 长度是8 
 * Return:	
 */
int net_cmd_handle(uint8_t *msg, uint16_t msg_len, uint8_t *reply, uint16_t *reply_len)
{
	uint8_t cmd = msg[0];
	int ret = 0;
	char par_buf[100] = {0};
	int val_len = msg_len - 1; /*有效数据长度 不含命令号*/
	
	/*CMD NO DEFINE*/
	if(cmd > CMD_INDEX_MAX) {
		pr_err("cmd is fail \n");	
		reply[0] = CMD_REP_NONE; /*无此命令*/
		*reply_len = 1;
	}

	switch(cmd)
	{

	case SET_OPEN_TIME:	/*设置开灯时间*/
		{
			memcpy(par_buf, &msg[1], val_len); //除去命令索引的内容
			ret = net_cmd_set_par_time("opening_time", (uint8_t *)par_buf, 
				strlen(par_buf));
			reply[0] = ret ? CMD_REP_NO:CMD_REP_YES;
			*reply_len = 1;
		}
		break;
	case GET_OPEN_TIME:  /*获取开灯时间*/
		{
			ret = get_par("opening_time", par_buf, sizeof(par_buf));
			if(ret)  
				pr_err("get par opening_time fail\n");
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}
		break;
	case SET_CLOSE_TIME:	/*设置关灯时间*/
		{
			memcpy(par_buf, &msg[1], val_len); //除去命令索引的内容
			ret = net_cmd_set_par_time("closeing_time", (uint8_t *)par_buf, 
				strlen(par_buf));
			reply[0] = ret ? CMD_REP_NO:CMD_REP_YES;
			*reply_len = 1;
		}		
		break;
	case GET_CLOSE_TIME:  /*获取关灯时间*/	
		{
			ret = get_par("closeing_time", par_buf, sizeof(par_buf));
			if(ret)  
				pr_err("get par close_time fail\n");
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}		
		break;
	case SET_BEIJING_TIME:  /*设置北京时间*/
		{
			*reply_len = 1;
			memcpy(par_buf, &msg[1], val_len); //除去命令索引的内容
			ret = check_par_len("beijingtime", par_buf);
			if(ret) {
				reply[0] = CMD_REP_NO;
				return ret;
			}
			ret = check_par_time((char *)par_buf);
			if(ret) {
				reply[0] = CMD_REP_NO;
				return ret;
			}
			reply[0] = CMD_REP_YES;
			pthread_mutex_lock(&cur_cmd_ops_str.cmd_mutex);
			set_time_time(par_buf);
			pthread_mutex_unlock(&cur_cmd_ops_str.cmd_mutex);
		}
		break;
	case GET_BEIJING_TIME:  /*获取北京时间*/ 
		{
			ret = get_cur_time(TIME_GET_TIME, par_buf); 
			if(ret)  
				pr_err("get par beijing_time fail\n");
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}			
		break;
	case GET_LIGHT_STATUS: /*获取当前开关状态*/
		{


		}
		break;
	case CAL_TEMP:	/*校准温度*/		
		break;
	case GET_TEMP:	/*获取温度*/
		{
			sprintf(par_buf, "%.2f", humiture_get_temp());
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}			
		break;
	case CAL_HUM:	/*校准湿度*/
		break;
	case GET_HUM:	/*获取湿度*/
		{
			sprintf(par_buf, "%.2f", humiture_get_hudiy());
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}		
		break;
	case GET_CUR_IP_ADDR: /*获取设备IP地址*/
		{
			ret = get_host_ipaddr(par_buf);
			if(ret)
				pr_err("get_host_ipaddr fail\n");
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}
		break;
	case GET_CUR_WLAN_NAME: /*获取设备无线名称*/
		{
			ret = get_wlan_line_name(par_buf);
			if(ret)
				pr_err("get_wlan_line_name fail\n");
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}
		break;
	case CONNET_TO_WLAN: /*连接到无线 :暂时禁止*/
		{
			ret  = -1;
			pr_err("CONNET_TO_WLAN func is disable\n");
		}
		break;
	case GET_HUMMEN_CTRL: /*获取人体红外房灯控制状态*/
		{
			ret =  get_par("hum_ctrl", par_buf, sizeof(par_buf));
			if(ret)  
				pr_err("get par hum_ctrl fail\n");
			memcpy(&reply[0], par_buf, strlen(par_buf));
			*reply_len = strlen(par_buf);
		}
		break;
	case SET_HUMMEN_CTRL: /*设置人体红外房灯控制状态*/
		break;
	case SET_ROOM_LIGHT_OPEN: /*设置房灯开 一定时间 单位s*/
		break;
	case SET_ROOM_LIGHT_CLOSE: /*取消房灯开时间 */
		break;
	case REBOOT_DEVICE: /*重启设备*/
		break;
	default:
		
		break;

	}

}

static void crc16_byte(unsigned int *crc, unsigned char db)
{
	int i;

	for(i=0;i<8;i++) {
		*crc <<= 1;
		if((db^(*crc>>16)) & 1)
			*crc ^= 0x8005;
		db >>= 1;
	}
	*crc &= 0xFFFF;
}

static unsigned int crc16_area(unsigned char *dbs, int size, unsigned int crc)
{
	while(size--)
		crc16_byte(&crc, *(dbs++));
	return crc;
}





/* net pack:
 *   [head] + [type] + [total len] + [data] + [crc16] 
 *	  3byte +  1byte +    2byte	   +  nbyte +   2byte
 *	 
 * @head = "sen" or "rep"
 * @tepe = NET_TYPE_CMD or NET_TYPE_DATA
 * @total len = 3byte +  1byte +    2byte	   +  nbyte +   2byte
 * @crc16 = 2byte
 */

/**
 * get_type_and_len_crc  获取包类型和长度并计算CRC
 * @msg 命令消息
 * @len 返回长度
 * Return:	
 */
uint8_t get_type_and_len_crc(uint8_t *data, uint16_t *len)
{
	char type;
	uint16_t crc16;
	uint16_t t_len = (data[4] << 8) | data[5];
	uint16_t raw_crc16 = (data[t_len - 2]<<8) | data[t_len - 1];

	if(t_len > NET_BUFFER_LEN_MAX){
		pr_err("pack data len is too big \n");
		return NET_TYPE_NODEFINE;
	}
	
	if(strncmp((char *)data, "sen", 3)){
		pr_err("recv head is not \"sen\" \n");
		return NET_TYPE_NODEFINE;
	}

//	pr_warn("data[3] =%02x t_len=%d\n",data[3], t_len);
	if((data[3] != NET_TYPE_CMD) && (data[3] != NET_TYPE_DATA)){
		pr_err("recv type：0x%02x is not nodefile \n",data[3]);
		return NET_TYPE_NODEFINE;
	}

	type = data[3];
	crc16 = crc16_area((unsigned char *)data, t_len - 2, 0);
	if(crc16 != raw_crc16) {
		pr_err("recv data crc is fail crc16:%04x raw_crc16:%04x \n",crc16, raw_crc16);
		return NET_TYPE_NODEFINE;
	}

	*len = t_len;
	return type;

}



/**
 * net_cmd_operation_func  网络命令操作函数
 * @msg 命令消息
 * @reply 回复消息
 * Return:	
 */
int net_cmd_operation_func(struct cmd_data *msg, struct cmd_data *reply)
{
	int ret;
	uint8_t type;
	uint16_t msg_len = 0, reply_len = 0 ,rep_crc = 0;
	
	type = get_type_and_len_crc((uint8_t *)msg->data, &msg_len);
	strncpy((char *)reply->data, "rep", 3); /*回复头*/
	reply->data[3] = type;	/*回复类型也接收类型一致*/
	
	switch(type) {
		
	case NET_TYPE_CMD:
		ret = net_cmd_handle(&msg->data[6], msg_len - 8, &reply->data[6], &reply_len);
		break;
	case NET_TYPE_DATA:	
		//ret = net_data_handle(data);
		break;
	case NET_TYPE_NODEFINE:
		// net_default_rep();
		break;
	}

	reply_len += 8;
	reply->data[4] = GET_U16_HIGH(reply_len);
	reply->data[5] = GET_U16_LOW(reply_len);
	rep_crc = crc16_area( (unsigned char *)reply->data, reply_len - 2, 0);
	reply->data[reply_len - 2] = GET_U16_HIGH(rep_crc);
	reply->data[reply_len - 1] = GET_U16_LOW(rep_crc);
	reply->len = reply_len;
	
	return 0;
	
}


/**
 * net_cmd_ops_str_get  获取命令操作结构
 * Return:	
 */
struct cmd_ops_str *net_cmd_ops_str_get(void)
{
	return &cur_cmd_ops_str;
}


/**
 * net_cmd_ops_init  初始化网络CMD操作函数相关内容
 * Return:	
 */
int net_cmd_ops_init(void)
{
	int ret = 0;
    ret = pthread_mutex_init(&cur_cmd_ops_str.cmd_mutex, NULL);
	cur_cmd_ops_str.cmd_ops = net_cmd_operation_func;

	if(ret == 0) 
		pr_init("init success \n");
	return ret;
}



