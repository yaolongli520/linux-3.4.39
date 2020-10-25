#ifndef __DISK_H
#define __DISK_H

#include <stdint.h>

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
#endif






