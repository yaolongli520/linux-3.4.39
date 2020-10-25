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

	cout <<"I am main "<<endl;
	param_init();



	
	return 0;
}

