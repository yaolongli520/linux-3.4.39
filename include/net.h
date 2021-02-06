#ifndef _LINUX_NET_H
#define _LINUX_NET_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define  NET_PORT_MAX  5//20

enum connect_status{
	DISCONNECT = 0,/* 断开*/
	WAITCONNECT, /* 等待 */
	INCONNECT, /* 连接成功 */
};

class NET_OPS{
private:	


public: 
	int serv_sock;/* serv socket */
	struct sockaddr_in serv_addr;/*服务器地址*/
	struct sockaddr_in clnt_addr[NET_PORT_MAX];/*客户端地址*/
	socklen_t clnt_addr_size[NET_PORT_MAX]; /*客户端地址结构大小*/
	int clnt_sock[NET_PORT_MAX];/*clnt socket*/
	enum connect_status c_current[NET_PORT_MAX];/*  当前客户端连接状态 */
	enum connect_status s_current;/*  当前服务器连接状态 */
	
	~NET_OPS();
	NET_OPS(int port);

};

int find_first_null_clr_index(NET_OPS &ops);
int set_clr_index_null(NET_OPS *ops, int index);
int get_host_name(char *host_name);
int get_host_ipaddr(char *ipaddr);
int get_wlan_line_name(char *name);






#endif
