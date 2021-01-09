#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <time.h>
#include <sys/time.h> 

#include <pthread.h>
#include <iostream>

#include "human.h"
#include "disk.h"
#include "mytime.h"


using namespace std;

struct hunman_data {
	int fd;
	char dev_name[30];
	bool is_init;
	bool has_hunman;			/*人体状态*/
	int  cur_val;	/*电平状态*/
	struct timespec timestamp; /*时间戳*/
	int mod_delay; 			   /*模块时延*/
};

static struct hunman_data cur_hunman;



	


/**
 * get_hunman_status  传感器感应状态
 *
 * Return:	TARGET_NONE /TARGET_EXIST /MACH_SUSPEND
 */
int get_hunman_status(void)
{
	int timeout = 0;
	struct timespec cur_t, off_t;

	if( (cur_hunman.cur_val == -1) || (!cur_hunman.is_init) ) 			/** 暂停 **/
		return MACH_SUSPEND; 
	
	if(cur_hunman.cur_val == 0) {			/** 下降沿过 mod_delay 后认为无人靠近！！ **/
		clock_gettime(CLOCK_REALTIME,&cur_t);
		off_t = get_time_offset(cur_hunman.timestamp, cur_t); 	/**计算时间差**/
		timeout = get_timeout(off_t, cur_hunman.mod_delay); /** 时间差是否超出设想值 **/	
		if(timeout == 1)  
			return  TARGET_NONE;
	}
		
	return  TARGET_EXIST;
}



/**
 * hunman_scanf  人体传感器扫描
 *
 * Return:	0 表示成功
 */
void *hunman_scanf(void *data)
{
	struct input_event vEvent;
	struct hunman_data *cur_data = (struct hunman_data *)data;
	int fd = cur_data->fd;

	if(!cur_data->is_init) {
		pr_err("%s is exit, hunman is not init\n");
		return NULL;
	}
	
	while(1) {
		
		if( read(fd, &vEvent, sizeof(vEvent)) > 0) {
			if(vEvent.code == EV_MSC) { 	/*捕捉 EV_MSC 事件*/
				cur_data->cur_val = vEvent.value; 
				clock_gettime(CLOCK_REALTIME, &cur_data->timestamp);	/*时间戳保存*/	
			}
		}	
	}

	pr_err("hunman_scanf is error exit \n");
	pthread_exit(0);
	close(fd);	
	cur_data->is_init = false;
}



/**
 * hunman_init  初始化配置参数
 *
 * Return:	0 表示成功
 */

int human_init(void)
{
	int ret;
	char file_name[30];
	int fd;
	pthread_t id;
	
	ret = get_par("humdev",file_name,sizeof(file_name));
	if(ret) {
		printf("par humdev is no find \n");
		return -ERR_FILE_NONE;
	}
	strcpy(cur_hunman.dev_name,file_name);
	sprintf(file_name,"/dev/input/%s",cur_hunman.dev_name);

	fd = open(file_name,O_RDONLY); /*只读不加 O_NDELAY 非阻塞*/
	if(fd < 0) {
		pr_err("lcd file %s open fail\n",file_name);
		cur_hunman.is_init = false;
		return -ERR_FILE_NONE;
	} else {
		cur_hunman.fd = fd;
		cur_hunman.is_init = true;
		cur_hunman.mod_delay = HUM_MOUDLE_DELAY;	
		cur_hunman.cur_val = -1;
	}

	ret = pthread_create(&id, NULL,
					   hunman_scanf,&cur_hunman);
	if(ret)
		printf("%s pthread_create fail \n",__func__);

	return ret;

}




