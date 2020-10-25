#ifndef __TIME__H
#define __TIME__H


#define TIME_GET_TIME  0x01  //时间
#define TIME_GET_DATE 0x02  //日期


int set_time_time(char *t); //设置时间
char * get_time_time(void);//时间
char * get_time_date(void); //日期
/*获取时间偏移*/
struct timespec get_time_offset(struct timespec &prev,struct timespec &cur);
/*确定是否超时*/
int get_timeout(struct timespec &time,long int timeout);
int get_cur_time(int flag,char *buf);
#define	 T_NSEC		1	
#define	 T_USEC		(1000*T_NSEC)
#define	 T_MSEC		(1000*T_USEC)
#define	 T_SEC		(1000*T_MSEC)

#define TIME_OUT	0x1
#define ON_TIME		0x0


#endif