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
	bool has_hunman;
};

static struct hunman_data cur_hunman;


static  int mod_delay = 9000; /*模块时延*/

enum mod_status{
	TARGET_NONE = 0,
	TARGET_EXIST,
	MACH_SUSPEND,
};


void *hunman_scanf(void *data)
{
	struct input_event vEvent;
	int ret = 0;
	int status = TARGET_NONE;
	int val = 0, per_val = 0;
	struct timespec fall_t, cur_t, off_t;
	struct hunman_data *cur_data = (struct hunman_data *)data;
	int fd = cur_data->fd;

	if(!cur_data->is_init) {
		printf("%s is exit, hunman is not init\n");
		return NULL;
	}
	#if 0
	while(1) {
		if( read(fd, &vEvent, sizeof(vEvent)) > 0)
		{
			if(vEvent.code == 0x4)
			val = vEvent.value;  //状态变化
		}
		
		if(per_val != val) {
			if(per_val==0 && val ==1) { 	//上升
				if(status == TARGET_NONE) 
					printf("Someone is approaching.\n");
				status = TARGET_EXIST;
			}
			else {   		//下降
				if(status == TARGET_NONE) 
					printf("this is warning \n");
				if(status == TARGET_EXIST)
					clock_gettime(CLOCK_REALTIME, &fall_t);	//记下下降时间		
			//	printf(" fall_t.tv_sec =%u \n",fall_t.tv_sec);
			//	printf(" fall_t.tv_nsec =%u \n",fall_t.tv_nsec);
			}
			per_val = val;
		}
		
		if(status == TARGET_EXIST && val == 0) {
			clock_gettime(CLOCK_REALTIME,&cur_t);
			off_t = get_time_offset(fall_t,cur_t);	
			if(get_timeout(off_t,mod_delay)) {
			//	printf(" off_t.tv_sec =%u \n",off_t.tv_sec);
			//	printf(" off_t.tv_nsec =%u \n",off_t.tv_nsec);
				printf("The man has left.\n");
				status = TARGET_NONE;
			}
		}
			
	}
	#endif
	printf("%s %d\n",__func__,__LINE__);
	
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

	fd = open(file_name,O_RDONLY | O_NDELAY);
	if(fd < 0) {
		printf("lcd file %s open fail\n",file_name);
		cur_hunman.is_init = false;
		return -ERR_FILE_NONE;
	} else {
		cur_hunman.fd = fd;
		cur_hunman.is_init = true;
	}

	ret = pthread_create(&id, NULL,
					   hunman_scanf,&cur_hunman);

	return 0;

}




