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
 * humiture_scanf  温湿度传感器扫描
 *
 * Return:	0 表示成功
*/
void *humiture_scanf(void *data)
{
	int ret;
	FILE *f_tmp, *f_hudiy;
	int temp, hudiy;
	struct humiture_data *dev_data = (struct humiture_data *)data;

	
	while(1) {
		f_tmp = fopen(TEMP_PATH,"r"); //读完要关闭否则读取值永远固定
		if(f_tmp ==NULL) {
			cout <<TEMP_PATH<<" null"<<endl;
			exit(-1);
		}

		f_hudiy = fopen(HUDTY_PATH,"r");
		if(f_hudiy ==NULL) {
			cout <<HUDTY_PATH<<" null"<<endl;
		    exit(-1);
		}

		ret = fscanf(f_tmp, "%d",&temp);//读温度
		if(ret == -1)
			temp = dev_data->raw_temp;	
		
		ret = fscanf(f_hudiy, "%d",&hudiy);//读湿度
		if(ret == -1)
			hudiy = dev_data->raw_hudiy;	
	
		dev_data->raw_temp = temp;
		dev_data->raw_hudiy = hudiy;

		dev_data->temp = dev_data->raw_temp/1000.0;
		dev_data->hudiy = dev_data->raw_hudiy/1000.0;

		dev_data->temp = dev_data->temp_mul_cal * dev_data->temp 
			+ dev_data->temp_add_cal;//计算校准温度

		dev_data->hudiy = dev_data->hudiy_mul_cal * dev_data->hudiy 
			+ dev_data->hudiy_add_cal;//计算校准湿度

		fclose(f_tmp);
		fclose(f_hudiy);
		sleep(HT_SACN_TIME);
	}
	

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
	

	if(access(TEMP_PATH, F_OK) || access(TEMP_PATH, F_OK)){
		printf("humiture file is not exit\n");
		return -ERR_FILE_NONE;
	}

	ret = get_par("temp_mul_cal",val_buf,sizeof(val_buf));
	if(ret) 
		printf("par temp_mul_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.temp_mul_cal);

	ret = get_par("temp_add_cal",val_buf,sizeof(val_buf));
	if(ret) 
		printf("par temp_add_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.temp_add_cal);

	
	ret = get_par("hudiy_mul_cal",val_buf,sizeof(val_buf));
	if(ret)
		printf("par hudiy_mul_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.hudiy_mul_cal);

	ret = get_par("hudiy_add_cal",val_buf,sizeof(val_buf));
	if(ret)
		printf("par hudiy_add_cal is not exit \n");
	else 
		sscanf(val_buf, "%f", &cur_data.hudiy_add_cal);
		

//	printf("cur_data.temp_mul_cal =%f \n",cur_data.temp_mul_cal);
//	printf("cur_data.temp_add_cal =%f \n",cur_data.temp_add_cal);
//	printf("cur_data.hudiy_mul_cal =%f \n",cur_data.hudiy_mul_cal);
//	printf("cur_data.hudiy_add_cal =%f \n",cur_data.hudiy_add_cal);



	ret = pthread_create(&id, NULL,
					   humiture_scanf,&cur_data);
	if(ret)
		printf("%s humiture_scanf fail \n",__func__);

	
	return 0;
}



