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
#include "net.h"
#include "cmd_ops.h"

using namespace std;

struct pht_server_data{
	NET_OPS *ops;
	int index;
};

#define NET_BUFFER_COUNT	1024

/* 当多个客户端连入启动多个线程执行这个函数,他们的堆栈不一样,比如变量i地址都不一样 */

/*
 * pth_net_server  为每个连接的客户端创建的服务器子线程
 * 
 *
 */
void *pth_net_server(void *data)
{

	int ret;
	int index;
	int clnt_sock = -1;
	struct pht_server_data *server_data = (struct pht_server_data *)data;
	struct sockaddr_in *cli_addr;
	uint8_t cmd_buf[NET_BUFFER_COUNT], rep_buf[NET_BUFFER_COUNT];
	struct cmd_ops_str *cmd_str = net_cmd_ops_str_get();

	index = server_data->index;
	cli_addr = &server_data->ops->clnt_addr[index];
	clnt_sock = server_data->ops->clnt_sock[index];

	pr_init("client ip=%s,port=%d clnt_sock=%d\n", inet_ntoa(cli_addr->sin_addr), 
		ntohs(cli_addr->sin_port),clnt_sock);
	
	while(1)
	{
		struct cmd_data cmd = { 0 }, rep = { 0 };
		
		memset(cmd_buf, 0, NET_BUFFER_COUNT);
		memset(rep_buf, 0, NET_BUFFER_COUNT);
		ret = read(clnt_sock, cmd_buf, sizeof(cmd_buf));
		/* cli is exit or error*/
		if(ret <= 0) {
			pr_warn("index:%d cli is exit ret=%d \n", index, ret);
			break;
		}
		
		cmd.data = cmd_buf;
		cmd.len = sizeof(cmd_buf);
		rep.data = rep_buf;
		rep.len = sizeof(rep_buf);
		cmd_str->cmd_ops(&cmd, &rep);

	/*打印客户端 IP端口*/
	//	printf("[%s:%d]:%s \n",inet_ntoa(cli_addr->sin_addr),ntohs(cli_addr->sin_port),cmd_buf);
		/*如需要响应*/
		if(rep.len > 0)
			write(clnt_sock, rep_buf, rep.len);
		
	}

	close(clnt_sock); /*close socket*/
	set_clr_index_null(server_data->ops, index); /*set index cli to null*/
	delete(server_data);
	return NULL;
}


/**
 * pth_net_server  网络服务器连接监测线程
 *  return 0
 */
void *pth_net_socket_accept(void *data)
{
	int index = -1;
	int ret = 0;
	NET_OPS net_ops(1234); /* define net class */
	pthread_t pth_nrfid[NET_PORT_MAX];
	struct pht_server_data *server_data;
	char host_name[30] = {0};
	char host_ip[30] = {0};
	
	if(net_ops.serv_sock < 0) {
		pr_err("net init is fail \n");
		pthread_exit(0);
	}

	
	ret = get_host_name(host_name);
	if(ret == 0)
		pr_init("host_name = %s \n",host_name);
	
	ret = get_host_ipaddr(host_ip);
	if(ret == 0)
		pr_init("host_ip = %s \n",host_ip);


	while(1)
	{
		index = find_first_null_clr_index(net_ops); /*查找可用的客户端号*/
		if(index == -1) {
			sleep(1);
			pr_warn("Server busy \n");
			continue;
		}

		/*要先设置accept的参数3否则获取ip=0.0.0.0*/
		net_ops.clnt_addr_size[index] = sizeof(net_ops.clnt_addr[index]); 
		net_ops.clnt_sock[index] = accept(net_ops.serv_sock, (struct sockaddr*)&net_ops.clnt_addr[index],
				&net_ops.clnt_addr_size[index]);
		
		if(net_ops.clnt_sock[index] < 0) {
			pr_err("index:%d accept fail \n",index);
			net_ops.clnt_sock[index] = -1;
			net_ops.clnt_addr_size[index] = 0;
			bzero(&net_ops.clnt_addr[index], sizeof(net_ops.clnt_addr[index]));
			continue;
		}
		
		server_data = new(struct pht_server_data);
		server_data->ops = &net_ops;
		server_data->index = index;
		/*虽然每次的线程执行同一个函数,但是他们堆栈等不一样*/
		ret = pthread_create(&pth_nrfid[index], NULL,
					   pth_net_server, (void *)server_data);
		
		if(ret) {
			pr_err("pth_net_server is create fail \n");	
			break;
		}

	}



}








