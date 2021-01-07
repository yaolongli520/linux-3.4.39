#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include "disk.h"
using namespace std;

struct parm {
	 uint8_t *par;
	 uint32_t par_size;
};


static struct parm cur_parm;

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


/**
 * find_par_addr  从cur_parm文件缓存中查到参数的地址
 * @par_name: 要查找的参数名
 * @addr: 参数的地址

 * Return:	0 表示成功      ERR_FILE_OPS_INVALID 操作无效
 */
int find_par_addr(const char *par_name,char **addr)
{
	char *p;
	int par_len;
	if(par_name == NULL) {
		pr_err("par_name is null\n");
		return -ERR_FILE_OPS_INVALID;
	}
	par_len = strlen(par_name);
	if(par_len > PARM_LEN_MAX || par_len < 1){
		pr_err("par_name is untrue\n");
		return -ERR_FILE_OPS_INVALID;
	}

	p = strstr((char *)cur_parm.par,par_name);
	if(!p) {
		*addr = NULL;
		return -ERR_FILE_OPS_INVALID;
	} else 
		*addr = p;	
	
	return 0;

}


/**
 * get_par  从cur_parm文件缓存中获取参数
 * @buf: 保存返回参数的缓存区
 * @len: 拷贝的最大有效长度

 * Return:	0 表示成功      ERR_FILE_OPS_INVALID 操作无效
 */

int get_par(const char *par_name,char *buf,int len)
{
	int ret;
	int par_len,cpy_len;
	char *par, *p1, *p2;
	ret = find_par_addr(par_name,&par);
	if(ret) {
		return -ERR_FILE_OPS_INVALID;
	}
	p1 = strchr(par,'"');
	if(p1 == NULL) {
		pr_err("file p1 is INVALID\n");
		return -ERR_FILE_OPS_INVALID;
	}
	
	p2 = strchr(p1+1,'"');
	if(p2 == NULL) {
		pr_err("file p2 is INVALID\n");
		return -ERR_FILE_OPS_INVALID;
	}

	if(!buf)
		return -ERR_FILE_OPS_INVALID;
	else 
		memset(buf,0,len);
	
	par_len = p2 - p1 - 1;
	cpy_len = len > par_len?par_len:len;
	memcpy(buf,p1+1,cpy_len);	
	return 0;
}


/**
 * set_par 设置cur_parm缓存中的参数
 * @buf: 修改的参数的缓存区
 * @len: 拷贝的最大有效长度

 * Return:	0 表示成功      ERR_FILE_OPS_INVALID 操作无效
 */

int set_par(const char *par_name,const char *buf,int len)
{
	int ret;
	int par_len,cpy_len;
	char *par, *p1, *p2;
	ret = find_par_addr(par_name,&par);
	if(ret) {
		return -ERR_FILE_OPS_INVALID;
	}
	p1 = strchr(par,'"');
	if(p1 == NULL) {
		pr_err("file p1 is INVALID\n");
		return -ERR_FILE_OPS_INVALID;
	}
	
	p2 = strchr(p1+1,'"');
	if(p2 == NULL) {
		pr_err("file p2 is INVALID\n");
		return -ERR_FILE_OPS_INVALID;
	}

	if(!buf)
		return -ERR_FILE_OPS_INVALID;

	par_len = p2 - p1 - 1;
	cpy_len = len > par_len?par_len:len;
	memcpy(p1+1, buf, cpy_len);	
	return 0;
}




/**
 * file_save_cfgparam  把cur_parm配置参数保存到文件中
 *
 * Return:	0 表示成功      ERR_FILE_NONE 打开失败
 */
int file_save_cfgparam(void)
{
	int ret;
	FILE *fp;
	int size = cur_parm.par_size;
	const uint8_t *data = cur_parm.par; 
			
	fp = fopen(PARM_FILE,"w");
	
	if(fp == NULL) {
		pr_err(" PARM_FILE not find!\n");
		return -ERR_FILE_NONE;
	}
	
	fwrite(data, size, 1, fp);
	fclose(fp);
	return 0;
}


/**
 * file_get_cfgparam  从文件中获取配置参数并初始化 cur_parm
 *
 * Return:	0 表示成功      ERR_FILE_NONE 打开失败
 */
int file_get_cfgparam(void)
{
	int size = get_file_size(PARM_FILE);
	FILE *fp;
	
	if(size < 0) 
		return -ERR_FILE_NONE;
		
	fp = fopen(PARM_FILE,"r");
	
	if(fp == NULL) {
		pr_err("PARM_FILE not find!\n");
		return -ERR_FILE_NONE;
	}

	cur_parm.par_size = size;
	cur_parm.par = (uint8_t *)malloc(cur_parm.par_size);
	if(!cur_parm.par) {
		pr_err(" malloc fail!\n");
		return -1;
	} else
		pr_init("par base address =%p \n",cur_parm.par);
	
	memset(cur_parm.par,0,cur_parm.par_size);

	fread(cur_parm.par, cur_parm.par_size, 1, fp);
	
	fclose(fp);
	return 0;
}



/**
 * param_init  初始化配置参数
 *
 * Return:	0 表示成功  ERR_FILE_OPS_INVALID 无参数    	     ERR_FILE_NONE 打开失败
 */
int param_init(void)
{
	int ret;
	ret = file_get_cfgparam(); /*加载文件数据到缓存区*/
	return ret;
}


