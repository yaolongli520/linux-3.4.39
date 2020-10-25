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

using namespace std;

int main(int argc,char *argv[])
{
/* 	char date[20] = {0};
	get_cur_time(TIME_GET_TIME,date);
	printf("date =%s \n",date); */
	
	uint32_t size = get_file_size("./green.bmp");
	printf("green.bmp size =%u \n",size);
	cout <<"I am main "<<endl;

	
	return 0;
}

