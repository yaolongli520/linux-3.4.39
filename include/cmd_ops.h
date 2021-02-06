#ifndef __CMD_OPS_H
#define __CMD_OPS_H

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cmd.h"

struct cmd_data{
	uint8_t *data;
	uint32_t len;
};

struct cmd_ops_str {
	pthread_mutex_t  cmd_mutex; /*命令操作锁*/
	int (*cmd_ops)(struct cmd_data *msg, struct cmd_data *reply); /*网络命令操作函数*/
	

};

#define NET_BUFFER_LEN_MAX	1024




int net_cmd_ops_init(void);
struct cmd_ops_str *net_cmd_ops_str_get(void);







#endif 

