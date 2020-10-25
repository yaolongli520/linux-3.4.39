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

using namespace std;



int main(int argc,char *argv[])
{
	int ret;
	cout <<"I am main "<<endl;
	ret = param_init();
	if(ret) {
		cout <<"param_init fail\n";
	}
	ret = ioctrl_init();
	if(ret) {
		cout <<"ioctrl_init fail\n";
	}

	ret = lcd_init();
	if(ret) {
		cout <<"lcd_init fail\n";
	}

	ret = human_init();
	if(ret) {
		cout <<"lcd_init fail\n";
	}

	
	return 0;
}

