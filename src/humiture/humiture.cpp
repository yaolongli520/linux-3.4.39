#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>


#include "humiture.h"
#include "disk.h"


#include <iostream>
using namespace std;

struct humiture_data {
	int raw_temp;
	int raw_hudiy;
	float temp_mul_cal; /*温度乘法校准*/
	float temp_add_cal; /*温度加法校准*/
	float hudiy_mul_cal; /*温度乘法校准*/
	float hudiy_add_cal; /*温度加法校准*/	
	float temp;	 /*温度计算结果*/
	float hudiy; /*湿度计算结果*/
	
};

static struct humiture_data cur_data = {
	.raw_temp = -1,
	.raw_hudiy = -1,
	.temp_mul_cal = 1.0,
	.temp_add_cal = 0.0,
	.hudiy_mul_cal = 1.0,
	.hudiy_add_cal = 0.0,
};


/**
 * humiture_get_temp  获取当前的温度
 *
 * Return:	0 表示成功
*/

float humiture_get_temp(void)
{
	return cur_data.temp;

}


/**
 * humiture_get_hudiy  获取当前的湿度
 *
 * Return:	0 表示成功
*/

float humiture_get_hudiy(void)
{
	return cur_data.hudiy;
}

/**
 * humiture_child_process  子进程负责获取硬件数据
 *
 * 
*/
void humiture_child_process(int pipe_fd)
{
	int fd;
	int ret;
	struct dht11_data dht11_datas = {0};

	fd = open(DHT11_FILE, O_RDWR);
	if(fd < 0) {
		pr_err("file %s open fail \n", DHT11_FILE);
		return ;
	}	

	while(1)
	{
		ret = read(fd, &dht11_datas, sizeof(dht11_datas));
		if(ret <= 0) {
			pr_warn("%s read fail \n",DHT11_FILE);
			continue;
		}
		ret = write(pipe_fd, &dht11_datas, sizeof(dht11_datas));
		if(ret != sizeof(dht11_datas)) {
			pr_warn("write pipe is fail \n");
		}
		sleep(HT_SACN_TIME);
	}


}


/**
 * humiture_parent_process  父进程负责计算硬件参数
 *
 * 
*/
void humiture_parent_process(int pipe_fd, void *data)
{
	int ret;
	int temp, hudiy;
	struct dht11_data dht11_datas = {0};
	struct humiture_data *dev_data = (struct humiture_data *)data;
	
	while(1) 
	{
		ret = read(pipe_fd, &dht11_datas, sizeof(dht11_datas));
		if(ret < sizeof(dht11_datas)) {
			pr_warn("read pipe is fail \n");
		}
		
		temp = dht11_datas.temp;//读温度
		hudiy = dht11_datas.hum;//读湿度		
		
		/* save data */
		dev_data->raw_temp = temp;
		dev_data->raw_hudiy = hudiy;
		
		dev_data->temp = dev_data->raw_temp/1000.0;
		dev_data->hudiy = dev_data->raw_hudiy/1000.0;

		dev_data->temp = dev_data->temp_mul_cal * dev_data->temp 
			+ dev_data->temp_add_cal;//计算校准温度

		dev_data->hudiy = dev_data->hudiy_mul_cal * dev_data->hudiy 
			+ dev_data->hudiy_add_cal;//计算校准湿度

		sleep(HT_SACN_TIME);
	}


}



/**
 * humiture_scanf  温湿度传感器扫描
 *
 * Return:	0 表示成功
*/
void *humiture_scanf(void *data)
{
	int ret;
	struct dht11_data dht11_datas = {0};
	struct humiture_data *dev_data = (struct humiture_data *)data;
	int pipe_fd[2] = {0}; //0-r  1-w

	/*cteate a pipe*/
	ret = pipe(pipe_fd);
	if(ret < 0) {
		pr_err("pipe creat is fail \n");
		return NULL;
	}

	/*cteate a process*/
	ret = fork();
	if(ret == -1) {
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		pr_err("fork creat is fail \n");
		return NULL;
	}

	/* child */
	if(ret == 0) {	
		close(pipe_fd[0]); /* only reserved write*/
		humiture_child_process(pipe_fd[1]);
		pr_err("humiture_child_process is exit\n");
	}else {
	/* parent */
		close(pipe_fd[1]); /* only reserved read*/
		humiture_parent_process(pipe_fd[0], data);
		pr_err("humiture_parent_process is exit\n");
	}

	return NULL;

}


/**
 * hunman_init  初始化配置参数
 *
 * Return:	0 表示成功
 */
int humiture_init(void)
{
	int ret;
	char val_buf[20] = {0};
	pthread_t id;
	
	ret = get_par("temp_mul_cal",val_buf,sizeof(val_buf));
	if(ret) 
		pr_err("par temp_mul_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.temp_mul_cal);

	ret = get_par("temp_add_cal",val_buf,sizeof(val_buf));
	if(ret) 
		pr_err("par temp_add_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.temp_add_cal);

	
	ret = get_par("hudiy_mul_cal",val_buf,sizeof(val_buf));
	if(ret)
		pr_err("par hudiy_mul_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.hudiy_mul_cal);

	ret = get_par("hudiy_add_cal",val_buf,sizeof(val_buf));
	if(ret)
		pr_err("par hudiy_add_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.hudiy_add_cal);
		

//	printf("cur_data.temp_mul_cal =%f \n",cur_data.temp_mul_cal);
//	printf("cur_data.temp_add_cal =%f \n",cur_data.temp_add_cal);
//	printf("cur_data.hudiy_mul_cal =%f \n",cur_data.hudiy_mul_cal);
//	printf("cur_data.hudiy_add_cal =%f \n",cur_data.hudiy_add_cal);



	ret = pthread_create(&id, NULL,
					   humiture_scanf,&cur_data);
	if(ret)
		pr_err("%s humiture_scanf create fail \n",__func__);

	
	return 0;
}



