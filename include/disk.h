#ifndef __DISK_H
#define __DISK_H

#include <stdint.h>
#include <stdlib.h>

#define ERR_FILE_NONE			2  //文件不存在
#define ERR_FILE_OPS_INVALID	3  //无效操作
#define ERR_PARM_NOT_SET 		4  //配置参数未设置
#define ERR_FILE_NOT_SIZE		5  //文件大小为0
#define ERR_MALLOC			  	6  //分配空间不足


#define  PARM_FILE "/usr/config/param.txt"

#define  PARM_LEN_MAX 	50  //参数名最大长度

//写的方式都会删除内容从新写入
enum disk_dir{
	REONLY_DISK =0, //只读
	REONLY_DISK_BIN, //只读二进制
	WRONLY_DISK, 	//写
	WRONLY_DISK_BIN, //写二进制
};


enum f_start{
	FILE_HEAD_START =0, //起始位是文件头
	FILE_HEAD_CUR, //起始位是当前
	FILE_HEAD_TAIL, //起始位是文件尾
};

struct disk_crtl_date{
	char *name;//名称
	char *path; //路径
	int size; //数据块大小
	int num; //数据块的个数
	FILE * fd; //文件描述符
	uint8_t ops_type; //操作方式
	void *buff;
};



int open_file(struct disk_crtl_date *date);
int close_file(struct disk_crtl_date *date);
int disk_save_data(struct disk_crtl_date *date);
int disk_get_data(struct disk_crtl_date *date);
void file_set_head(struct disk_crtl_date *date);
void file_fseek(struct disk_crtl_date *date,long int off,enum f_start s);
unsigned long get_file_size(const char *pfile_path);
int param_init(void);
int file_save_cfgparam(void);
int set_par(const char *par_name,const char *buf,int len);
int get_par(const char *par_name,char *buf,int len);



#endif






