#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "disk.h"


/**
 * get_file_size  获取文件的大小
 * @pfile_path: 文件的路径

 * Return:	成功返回文件的大小
 */
unsigned long get_file_size(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;//文件描述
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}


/* w r a 并不是单纯 写 读 追加
w 带有覆盖性 含有w都会创建新文件把原先内容覆盖 
a 带有保护性 使用a打开文件,
用fseek 移动到的文件头,只能移动到原先打开文件时候的
文件尾巴,比如原先含有100字节 这里文件头是 100字节处

r 方式虽然是只读 但是 r+ 方式可以读写文件不会覆盖原内容
亦不会导致原内容无法修改
 */

/**
 * get_ops_type  获取打开文件的类型
 * @pfile_path: 文件的路径

 * Return:	返回对应打开方式的字符串
 */
const char *get_ops_type(struct disk_crtl_date *d)
{
	const char *dir;
	switch(d->ops_type)
	{
		case REONLY_DISK: //read only
			dir = "r";
			break;
		case REONLY_DISK_BIN: // read only bin
			dir = "rb";
			break;
		case WRONLY_DISK: //read + write
			dir = "r+";
			break;
		case WRONLY_DISK_BIN: //read + write bin
			dir = "rb+";
			break;
		default:
			dir = "rb+";	
	}
	return dir;
}


/**
 * open_file  打开文件的类型
 * @date: 描述打开文件的方式路径和文件名称

 * Return:	成功返回0
 */
int open_file(struct disk_crtl_date *date)
{
	int p_len = strlen(date->path);
	int n_len = strlen(date->name);
	char file[p_len+n_len+1];
	const char *dir = get_ops_type(date);
	unsigned long filesize;
	
	strncpy(file,date->path,p_len);
	strncpy(file+p_len,date->name,n_len+1);
	#if 0 //debug
	printf("file=%s file_len=%d\n",file,p_len+n_len);
	printf("dir =%s\n",dir);
	#endif
	filesize = get_file_size(file);
	printf("file %s size =%lu B\n",date->name,filesize);
	
	date->fd = fopen(file,dir);
	if(date->fd == NULL )
	{
		printf("%s open fail was creat\n",file);
		date->fd = fopen(file,"wb+");//创建
		if(date->fd == NULL )
		{
			printf("%s creat fail \n",file);
			return -1;
		}
	}

	return 0;
}

int close_file(struct disk_crtl_date *date)
{
	fclose(date->fd);
}

/**
 * disk_get_data  从文件中读取内容
 * @date: 文件描述结构

 * Return:	成功返回0
 */
int disk_get_data(struct disk_crtl_date *date)
{
	int size = date->size; 
	int num  = date->num;
	int ret;
	if(date->buff == NULL) 
	{
		printf("date->buff is null\n");
		return -1;
	}
	ret = fread(date->buff,size,num,date->fd);
	if(ret == 0) 
	{
		printf("fread is fail\n");
		return -1;
	}
	else if(ret < date->num)
	{
		printf("fread only read %d black\n",ret);	
	}
	
	return ret;	
}

/**
 * disk_save_data  保存内容到文件中
 * @date: 文件描述结构

 * Return:	成功返回0
 */
int disk_save_data(struct disk_crtl_date *date)
{
	int size = date->size; 
	int num  = date->num;
	int ret;
	if(date->buff == NULL) 
	{
		printf("date->buff is null\n");
		return -1;
	}
	
	//printf("size =%d num=%d \n",size,num);
	ret = fwrite(date->buff,size,num,date->fd);
	
	if(ret == 0) 
	{
		printf("fwrite is fail\n");
		return -1;
	}
	else if(ret < date->num)
	{
		printf("fwrite only write %d black\n",ret);	
	}
	return ret;	
}

//将文件位置设置为头
void file_set_head(struct disk_crtl_date *date)
{
	rewind(date->fd);	
}

/**
 * file_fseek  设置文件文件的位置
 * @date: 包含文件句柄的结构
 * @off: 偏移字节
 * @s: 开始位置
 * Return:	
 */
void file_fseek(struct disk_crtl_date *date,long int off,enum f_start s)
{
	if(s == FILE_HEAD_START)
		fseek(date->fd,off,SEEK_SET);
	else if(s == FILE_HEAD_CUR)
		fseek(date->fd,off,SEEK_CUR);
	else
		fseek(date->fd,off,SEEK_END);	
}