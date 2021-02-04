#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

#include "net.h"
#include "disk.h"

/**
 * find_first_null_clr_index  查找可用的客户端下标
 * Return:	-1 失败 否则返回空闲的下标号
 */
int find_first_null_clr_index(NET_OPS &ops)
{
	int i = 0;
	for(i = 0; i < NET_PORT_MAX; i++) {
		if((ops.c_current[i] == DISCONNECT) && (ops.clnt_sock[i] == -1))
			return i;
	}
	
	return -1;
}

/**
 * set_clr_index_null  通过索引设置客户端状态为空闲
 * Return:	
 */
int set_clr_index_null(NET_OPS *ops, int index)
{
	if(index >= NET_PORT_MAX)
		return -1;
	ops->c_current[index] = DISCONNECT;
	ops->clnt_sock[index] = -1;
	return 0;
}


NET_OPS::NET_OPS(int port)
{
	int i = 0;
	uint32_t p0 = port;

	for(i = 0; i < NET_PORT_MAX; i++) {
		this->c_current[i] = DISCONNECT;
		this->clnt_sock[i] = -1;
		this->clnt_addr_size[i] = 0;
		bzero(&this->clnt_addr[i], sizeof(this->clnt_addr[i]));
	}
	this->s_current = DISCONNECT;
	
	/* cteate socket */
	this->serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(this->serv_sock < 0) {
		pr_err("socket cteate fail this->serv_sock =%d \n",this->serv_sock );
		return;
	}

	bzero(&this->serv_addr, sizeof(this->serv_addr));
	this->serv_addr.sin_family = AF_INET; /* IPv4 */
	this->serv_addr.sin_port = htons(p0);  /* set port */
	this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /*ip addr 监听所有地址*/

	/* bind port */
	if (bind(this->serv_sock, (struct sockaddr *)&this->serv_addr, sizeof(this->serv_addr))) {
		pr_err("socket bind fail \n");
		goto net_err;
	}

	listen(this->serv_sock, NET_PORT_MAX); /* 监听端口 */
	
	pr_init("socket init ok\n");
	
	return;
	
net_err:
	close(this->serv_sock);
	pr_err("socket init fail \n");

}



NET_OPS::~NET_OPS(void)
{
	int i = 0;
	for(i = 0; i < NET_PORT_MAX; i++) {
		if(this->c_current[i])
			close(this->clnt_sock[i]);
	}
	memset(this->c_current, 0 , sizeof(this->c_current));
	this->s_current = DISCONNECT;
	close(this->serv_sock);
}








