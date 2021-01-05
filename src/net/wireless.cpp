#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <ifaddrs.h>
#include <arpa/inet.h> 

#include "disk.h"
#include "wireless.h"

using namespace std;




/*获取网卡的连接状态*/
int c_ifaddrs_netlink_status(const char *if_name )
{
    struct ifaddrs *ifa = NULL, *ifList;  

    if (getifaddrs(&ifList) < 0) {
        return -1;
    }

    for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            if (strcmp(ifa->ifa_name, if_name) ==0) {
                if (!(ifa->ifa_flags & IFF_UP)) { 
                    freeifaddrs(ifList);
                    return DEVICE_DOWN;
                }

                if (!(ifa->ifa_flags & IFF_RUNNING)) {
                    freeifaddrs(ifList);
                    return DEVICE_UNPLUGGED;
                }

                freeifaddrs(ifList);
                return DEVICE_LINKED;
            }
        }  
    }  

    freeifaddrs(ifList);
    return DEVICE_NONE;
}


/**
 * get_net_device_status  获取网卡的连接状态
 *
 * return:	enum lint_status
 */
int get_net_device_status(void)
{
	int ret;
	char network_card[30] = {0};
	ret = get_par("netdevice",network_card,sizeof(network_card));
		if(ret) {
			printf("par netdevice is no find \n");
			return -ERR_FILE_NONE;
	}

	ret = c_ifaddrs_netlink_status(network_card);

	return ret;
}



