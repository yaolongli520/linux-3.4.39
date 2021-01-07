#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pack.h"
#include "disk.h"
#include "cmd.h"
#include "list.h"

#include <iostream>



using namespace std;


const char *type_name[] ={
	"TYPE_CMD",
	"TYPE_DATA",
	"TYPE_REQ_DATA",
	"TYPE_ERR",
	"TYPE_NODEFINE",
};



int  if_pace_true(const struct pack_head *head,struct pack_data *data);

uint32_t pack_sum = 0; /*数据号 每次传输自加*/

void pack_module_init(void)
{
	init_data_base(); //初始化链式缓存
}

int get_buff_loss_len(void)
{
	return get_data_len();
}

static struct nrf_pack head_fail; /* 头错误 */
static struct nrf_pack head_com; /* 完成 */

struct nrf_pack* init_fail_pack(void)
{
	struct data_head head = {
		.p_len = 0,
		.d_len = 0,
		.pack_sum = 0,
		.type	= HEAD_FAIL,
	};
	head_fail.type = TYPE_DATA;
	head_fail.len  = 0;
	head_fail.num  = CMD_HEAD_FAIL; /*命令号*/
	head_fail.check = (~head_fail.type & 0x3);
	memcpy(head_fail.data,&head,sizeof(head));
	return &head_fail;
}

/**
 * get_fail_pack - 通过获取 head_fail 结构体的地址并返回
 * 
 *
 * Return: 返回一个头部错误的回复包结构
 */
struct nrf_pack* get_fail_pack(void)
{
	return &head_fail;
}

struct nrf_pack* init_completion_pack(void)
{
	struct data_head head = {
		.p_len = 0,
		.d_len = 0,
		.pack_sum = 0,
		.type	= COMPLETION,
	};
	head_com.type = TYPE_DATA;
	head_com.len  = 0;
	head_com.num  = CMD_COMPLETION;/*命令号*/
	head_com.check = (~head_com.type & 0x3);
	memcpy(head_com.data,&head,sizeof(head));
	return &head_com;	
}



/**
 * get_completion_pack - 通过获取 head_com 结构体的地址并返回
 * 
 *
 * Return: 返回一个头部错误的回复包结构
 */
struct nrf_pack* get_completion_pack(void)
{
	return &head_com;
}


/**
 * data_to_pack_packlen - 有效数据长度转换为包的个数 比如长度285 ->11个
 * 
 *
 * Return: 返回lenght长度的数据需要打包成的包个数
 */

uint32_t data_to_pack_packlen(uint32_t lenght)
{
	return DIV_ROUND_UP(lenght,EACH_LEN_MAX);
}

/**
 * data_to_pack_datalen - 有效数据长度转换为加包头的数据长度
 * 
 *
 * Return: 返回lenght长度的数据打包后的新长度 29->64
 */

uint32_t data_to_pack_datalen(uint32_t lenght)
{
	return DATALEN_TO_PACKLEN(lenght);
}



/**
 * make_pack - 用于释放构造的包空间
 * 
 *
 * Return: NULL
 */

void del_pack(struct nrf_pack* pack)
{
	if(pack->type == TYPE_DATA || pack->type == TYPE_REQ_DATA) 
		free(pack);
}



/**
 * make_pack - 用于构造数据包进行发送
 * 
 *
 * Return: 返回用于发送的数据包结构
 */

struct nrf_pack* make_pack(uint8_t *buf,uint32_t length)
{
	int i;
	struct nrf_pack *pack;
	uint32_t p_len, d_len = length;
	
	/*设置小包的个数*/
	p_len = DIV_ROUND_UP(d_len,EACH_LEN_MAX);
	pack = (struct nrf_pack *)malloc(sizeof(struct nrf_pack) * p_len);
	if(!pack) {
		printf("malloc is fail %d \n",__LINE__);
		return NULL;
	}
	
	/*填充包身*/
	for(i = 0; i < p_len; i++){
		pack[i].type = TYPE_DATA;
		pack[i].len = (d_len>EACH_LEN_MAX)?EACH_LEN_MAX:d_len;
		pack[i].num = i;
		pack[i].check = (~pack[i].type & 0x3);
		memcpy(pack[i].data,buf+i*EACH_LEN_MAX,pack[i].len);	
		d_len -= pack[i].len;
	}
	if(d_len!=0) cout << "warning d_len no == 0"<<endl;
	
	return pack;
} 


/**
 * get_pack_max_num - 用于获取包的最大下标
 *
 *
 * Return: 
 */

uint32_t get_pack_max_num(struct nrf_pack *pack)
{
	uint32_t num;
	while(IS_CHECK_OK(pack)) 	
		pack++;
	pack--;
	num = pack->num;
	return num;
}



/**
 * get_loss_sum - 用于获取丢失包的总数,并把丢失包写入链表
 *
 *
 * Return: 
 */

uint32_t get_loss_sum(uint32_t max,struct nrf_pack *pack)
{
	uint32_t sum = 0;
	
	while(IS_CHECK_OK(pack)) {
		for(int i = 0; i < EACH_LEN_REQ; i++) {
			for(int j = 0; j < EACH_REQ_BIT; j++)	
				if((pack->req[i] & (0x1<<j)) == 0)/*位 = 0*/
				{
					if(pack->num * EACH_LEN_LOSS + i * EACH_REQ_BIT + j <= max) {
						sum ++;
						write_data(pack->num * EACH_LEN_LOSS + i * EACH_REQ_BIT + j);
					//	printf("val =%d \n",pack->num * EACH_LEN_LOSS + i * EACH_REQ_BIT + j);
					//	printf("j = %d pack->req[i]=%d \n",j,pack->req[i]); 
					}
				}
		}
		pack++;
	}
	return sum;
}


/**
 * make_pack - 用于构造数据进行补发
 * @snd_pack: 第一次发送的完整包
 * @lo_pack:  描述丢失的包 
 * Return: 返回用于再次发送的数据包结构
*/
struct nrf_pack* make_pack(struct nrf_pack *snd_pack,struct nrf_pack *lo_pack)
{
	struct nrf_pack *pack;
	uint32_t max_num =  get_pack_max_num(snd_pack); /*最大包号*/
	uint32_t p_len = get_loss_sum(max_num,lo_pack); /*丢失总数*/
	uint32_t loss[p_len];
	int i = 0;

	if(p_len == 0) {
		printf("is not loss \n");
		return NULL;
	}
	read_all_data(loss);

	pack = (struct nrf_pack *)malloc(sizeof(struct nrf_pack) * p_len);
	if(!pack) {
		cout <<"malloc is fail"<<__LINE__<<endl;
		return NULL;
	}

	for(i = 0; i < p_len; i++) {
		memcpy(&pack[i],&snd_pack[loss[i]],TRAN_LEN_MAX);
	}

	return pack;
} 



/**
 * make_pack - 用于构造请求重发包
 *  每个包 224 bit 收到的位=1 未收到=0
 *
 * Return: 返回用于发送的数据包结构
 */

struct nrf_pack* make_pack(uint32_t length)
{
	struct nrf_pack *pack;
	uint32_t p_len, d_len = length;
	
	/*设置小包的个数*/
	p_len = DIV_ROUND_UP(d_len,EACH_LEN_MAX);/*多少个数据包*/
	p_len = DIV_ROUND_UP(p_len,EACH_LEN_LOSS); /*需要多少个请求重发包*/

	pack = (struct nrf_pack *)malloc(sizeof(struct nrf_pack) * p_len);
	if(!pack) {
		cout <<"malloc is fail"<<__LINE__<<endl;
		return NULL;
	}
	memset(pack,0,sizeof(struct nrf_pack) * p_len);
	for(int i = 0; i < p_len; i++){
		pack[i].type = TYPE_REQ_DATA;
		pack[i].len	 = EACH_LEN_REQ;
		pack[i].num  = i;
		pack[i].check = (~pack[i].type & 0x3);
	}
	return pack;
} 


/**
 * nrf_check_type - 检查 nrf_pack 数据的类型是否合法
 * 
 * Return: 校验失败 返回 TYPE_ERR,否则返回类型,或者未定义类型 TYPE_NODEFINE。
 */
int nrf_check_type(void *data)
{
	struct nrf_pack *pack = NULL;
	pack = (typeof (pack)) data;
	if(!IS_CHECK_OK(pack))  /*检验*/
		return TYPE_ERR;
	if(pack->type < TYPE_ERR) 
		return pack->type;
	else 
		return TYPE_NODEFINE; /*校验正确但不存在此类型*/
}


/*打印包*/
void print_nrf_pack(struct nrf_pack* pack)
{
	int i;
	while(IS_CHECK_OK(pack)) {
		printf("type:%s  len:%02d  num:%02d ==>",type_name[pack->type], pack->len, pack->num);
		switch(pack->type) {
		/* */
		case TYPE_CMD:
			printf("unknown...");
			break;
		case TYPE_DATA:	
			for(i = 0; i < pack->len; i++) {
				if(pack->data[i] != 0 )
					printf("%c ",pack->data[i]);
				else
					printf("  ");
			}	
			break;
		case TYPE_REQ_DATA:		
			for(i = 0; i < pack->len; i++) {
				printf("%08x ",pack->req[i]);
			}
			break;
		}
		printf("\n");
		pack++;
	}
	printf("\n");	
}



