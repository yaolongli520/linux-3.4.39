#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#include "mytime.h"
#include "disk.h"
#include "ioctrl.h"
#include "lcd.h"
#include "human.h"
#include "humiture.h"
#include "nrf24l01.h"
#include "wireless.h"
#include "pack.h"
#include "cmd.h"


using namespace std;
#define ONE_PROCESS_NUM  16  /* 单次read 最大读的包数*/

#define BIT_SET_OK  0
#define BIT_IS_SET  1

/* 线性缓冲 pack[h]-[name]-[len]-[data]*/
struct file{
	char name[50];
	uint8_t 	data[0];
};


static struct file *file_buff = NULL; /*文件缓存*/
static uint32_t file_total = 0; /*文件总长度*/
static uint32_t recv_total = 0; /*接收总长度*/
struct nrf_pack *loss_pack = NULL; /*记录丢包*/
struct nrf_pack *loss_pack_off = NULL; /*记录完成偏移*/
struct timespec str_time; /*传输开始*/
struct timespec end_time; /*传输结束*/

/*服务器参数*/
static uint32_t par[20] = {
0,1,2,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19
};


/*文件处理返回*/
enum {
	FILE_PROCESS_OK = 0,
	FILE_ALRE_ALLOC_BUFF, /*文件缓存已分配*/
	FILE_ALRE_ALLOC_LOSS, /*丢包缓存已分配*/
	FILE_BUF_NODEFIDE, /* 文件缓存未定义 */
};

enum{
	FILE_UP_COMP = 0, /*接收完成*/ 
	FILE_UP_NULL, /*未有文件*/
	FILE_UP_FAIL, /*接收出错*/
	FILE_UP_RECVING, /*接收文件中 */
	FILE_UP_SAVE_FAIL, /*保存失败*/
};




class nrf_server {
private:

public:
	int nrf_server_process(void *data, uint32_t len);
	int nrf_handle_pack(void *data); /* 处理包 */
	int recv_comp_process(void);
	int nrf_transfer_file(struct nrf_cmd &cmd);
	int nrf_transfer_cmd(struct nrf_cmd &cmd);
	/*读命令*/
	int nrf_read_cmd(void *cmd,long int timeout);
};



int index_to_data(uint32_t index,uint8_t *data)
{
	if(index > ARRAY_SIZE(par) - 1) {
		memset(data,0xff,4);
		return -1;
	}
	memcpy(data,&par[index],sizeof(par[index]));
	return 0;
}



/**
 * nrf_server_process - 服务器进程 对接收指令和数据进行处理
 * 
 * Return:
 */

int nrf_server::nrf_server_process(void *data, uint32_t len)
{
	int ret = 0;
	int num = DIV_ROUND_UP(len,TRAN_LEN_MAX);
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	for(int i = 0; i < num; i++ ) {
		ret = nrf_handle_pack(&pack[i]);
	}	
	return ret;
}


/**
 * create_up_file_buffer - 创建存储文件的缓存区,和包丢失缓冲区
 * 
 * Return:
 */

int create_up_file_buffer(void *data)
{
	int ret = FILE_PROCESS_OK;
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	struct data_head *file_desc = (struct data_head *)pack->data;

	if(file_buff){
		ret = FILE_ALRE_ALLOC_BUFF;
		free(file_buff);
	}

	if(loss_pack){
		ret = FILE_ALRE_ALLOC_LOSS;
		free(loss_pack);
	}

	if(file_desc->d_len) {
		file_buff  = (struct file *)malloc(file_desc->d_len);
		loss_pack  = make_pack(file_desc->d_len);
		file_total = file_desc->d_len;
		recv_total = 0; /*总长度*/
		clock_gettime(CLOCK_REALTIME, &str_time);	
	}
	
	return ret;
}


/**
 * send_loss_pack_to_client - 从loss_pack 筛选出含丢失包的部分返回给从机
 * 若果没有丢失 返回完整包！！
 * Return:
 */
int  send_loss_pack_to_client(void)
{
	int sum = 0;
	struct nrf_pack *pack;
	if(loss_pack_off) 
		pack = loss_pack_off; /*如果有则从这个位置开始*/
	else
		pack = loss_pack; /*重来开始*/
	
	if(!pack)
		return ERR_SND_DATA;
	while(IS_CHECK_OK(pack) && (sum<16)) {
		for(int i = 0; i < EACH_LEN_REQ; i++) 
			if(pack->req[i]!=0xffffffff) { /*只要有一个未完成*/	
				if(sum == 0)  /*这里开始发现丢包 记录这个开始位置*/
					loss_pack_off = pack; 
				nrf24l01_tx((uint8_t *)pack,TRAN_LEN_MAX);
				sum++; /*回复计数*/
				break;
		   }
			pack++;	
	}
	
	return ERR_NONE;
}



int nrf_transfer_getpar(void *data)
{
	int	ret;
	uint32_t val = 0;
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	struct get_par_str *cur_par = (struct get_par_str *)pack->data;
	int index = cur_par->index;

	ret = index_to_data(index,cur_par->data);
	
	if(ret) 
		cout << "index =" <<index <<"no find "<<endl;
	memcpy(&val,cur_par->data,sizeof(val));
	return 0;
}

int nrf_transfer_setpar(void *data)
{
	int	ret;
	uint32_t val = 0;
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	struct get_par_str *cur_par = (struct get_par_str *)pack->data;
	int index = cur_par->index;
	
	memcpy(&val,cur_par->data,sizeof(val));
	if(index < ARRAY_SIZE(par))
		par[index] = val;
	else
		memset(cur_par->data,0xff,sizeof(val));
	return 0;
}



/**
 * nrf_cmd_handle - 对接收到的命令进行处理和回复
 * 
 * Return:
 */

int nrf_cmd_handle(void *data)
{
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	int cmd = pack->num;
	
	switch(cmd) {
		
	case CMD_SETPAR: /* 设置参数 */
		nrf_transfer_setpar(data);
		nrf24l01_tx((uint8_t *)data,TRAN_LEN_MAX);
		/* 设置并回复 */
		break;	
	case CMD_GETPAR: /* 获取参数 */
		nrf_transfer_getpar(data);
		nrf24l01_tx((uint8_t *)data,TRAN_LEN_MAX);
		/* 获取参数并回复 */
		break;
	case CMD_GET_LOSS: /* 获取丢包 */
		nrf24l01_tx((uint8_t *)data,TRAN_LEN_MAX); /*先回复再返回丢包*/
		send_loss_pack_to_client();
		break;
	case CMD_UP_FILE: /* 上传文件 */
		/* 创建文件缓存区并回复 */
		create_up_file_buffer(data);
		usleep(1000);
		nrf24l01_tx((uint8_t *)data,TRAN_LEN_MAX);
		break;
	case CMD_DOWN_FILE: /* 下载文件 */	
		/*创建文件数据并回复*/
	
		break;
		
	}
	
	return 0;
}



/**
 * set_already_recv_bit - 对接收到的包位进行设置
 * 
 * Return:
 */
bool set_already_recv_bit(uint32_t num)
{
	int pack_num = num/EACH_LEN_LOSS; /*包偏移 0~223 = 0 */
	int pack_bit_offset = num%EACH_LEN_LOSS; /*当前包的位偏移*/
	int pack_word_offset = pack_bit_offset/32; /*第几个字*/
	int pack_word_bit = pack_bit_offset%32; /*该字的第几个位*/
	if(loss_pack[pack_num].req[pack_word_offset] & (0x1)<<pack_word_bit)
		return BIT_IS_SET;
	loss_pack[pack_num].req[pack_word_offset] |=(0x1)<<pack_word_bit;
	return BIT_SET_OK;
}




/**
 * nrf_data_handle - 对接收到的包进行处理
 * 
 * Return:
 */

int nrf_data_handle(void *data)
{
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	uint8_t *dts = (uint8_t *)file_buff + EACH_LEN_MAX * pack->num;

	if(!file_buff) 
		return FILE_BUF_NODEFIDE; /*缓存未分配*/
	
	if(pack->type == TYPE_DATA) { /*次数大于总次数??*/
		bool ret;
		ret = set_already_recv_bit(pack->num); /*设置接收完成位*/
		if(ret == BIT_SET_OK) {
			memcpy(dts, pack->data, sizeof(uint8_t) * pack->len);	
			recv_total += pack->len;
		}
	}
	else if(pack->type == TYPE_REQ_DATA) {
		memcpy(dts, pack->data, sizeof(uint32_t) * pack->len);
		set_already_recv_bit(pack->num); /*设置接收完成位*/
	}
	else 
		cout <<"Unidentified type packet !!"<<endl;
	
	return FILE_PROCESS_OK;
}



/**
 * nrf_handle_pack - 对接收到的包进行处理
 * 
 * Return:
 */

int nrf_server::nrf_handle_pack(void *data)
{
	int ret = 0;
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;

	ret = nrf_check_type(data);
	if(ret >= TYPE_ERR) {
	//	cout <<"nrf_check_type data is fail"<<endl;
	//	print_err(pack);
		return ret;
	}
	
	switch(pack->type) {
		
	case TYPE_CMD:
		ret = nrf_cmd_handle(data);
		break;
	case TYPE_DATA:	
		ret = nrf_data_handle(data);
		break;
	case TYPE_REQ_DATA:
	
		break;
	case TYPE_ERR:	
	
		break;
	case TYPE_NODEFINE:	
	
		break;	
	case TYPE_SHOW_LOSS:
		print_nrf_pack(loss_pack);
		printf("recv_total = %u \n",recv_total);
		break;
	}
	
	return ret;
}


/**
 * recv_comp_process - 文件接收完成处理
 * 
 * Return:
 */
int nrf_server::recv_comp_process(void)
{
	static uint32_t recving = 0;
	if(file_total == 0) 
		return FILE_UP_NULL; /*未有文件*/
	
	if(recv_total < file_total) {
		
		float persen; /*完成百分比*/
		if(recving == 0)
		printf("to_len =%d  persen=       ",file_total);
		recving = 1;
		persen = (100.0*recv_total)/file_total;
		if(persen >= 99.99) persen = 99.99; //防止非常接近100
		printf("\b\b\b\b\b\b%5.2f%%",persen); 	
		fflush(stdout);	
		
		return FILE_UP_RECVING;
	}
	
	if(recv_total > file_total) 
		return FILE_UP_FAIL;

	if(recv_total == file_total) {
		char file_name[50] = {0};
		FILE *fp;
		strcpy(file_name,file_buff->name);
		fp = fopen(file_buff->name,"w+");
		if(fp==NULL) {
			return FILE_UP_SAVE_FAIL;
		}
		fwrite(file_buff->data,file_total - 50,1,fp);
		recv_total = 0;
		file_total = 0;
		free(file_buff);
		free(loss_pack);
		file_buff = NULL;
		loss_pack = NULL;
		loss_pack_off = NULL; /*恢复*/
		fclose(fp);
	}
	printf("\b\b\b\b\b\b\b%6.2f%%......",100.00);
	fflush(stdout);	
	clock_gettime(CLOCK_REALTIME, &end_time);	
	struct timespec time = get_time_offset(str_time,end_time);
	printf(" %u.%u s \n",time.tv_sec,time.tv_nsec/1000000);
	
	recving = 0;
	return FILE_UP_COMP;
	
}



/**
 * pth_nrf_server  无线服务器线程
 *  return 0
 */
void *pth_nrf_server(void *data)
{
	int ret;
	nrf_server ops;
	uint8_t recv[(ONE_PROCESS_NUM + 1)*TRAN_LEN_MAX] = {0}; /*多一个空保留空间*/

	ret = nrf24l01_find_dev();
	if(ret) {
		pr_err("nrf24l01 not find cur pth is exit \n");
		return NULL;
	}

	pack_module_init(); /* init list buffer */

	while(nrf24l01_rx((uint8_t *)&recv,32)>0);/* clear recv buff */
		pr_init("nrf24l01 server start ... \n ");

	/*check cmd size */
	if(sizeof(struct nrf_pack ) != 32)
		pr_warn("warning sizeof cmd = %d",sizeof(struct nrf_pack ));
		

	while(1)
	{
		ret = 0;
		while(ret == 0) {
			memset(recv,0,sizeof(recv));
			ret = nrf24l01_rx((uint8_t *)recv,sizeof(recv) - TRAN_LEN_MAX);
		}
		ops.nrf_server_process(recv,ret);
		ops.recv_comp_process();
		sleep(2);
		

	}


}





