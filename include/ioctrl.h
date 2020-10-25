#ifndef __IOCTRL_H
#define __IOCTRL_H

#include <stdint.h>
#include <stdlib.h>

#define value_err   -1
#define value_off   0
#define value_on   1




#define ON_STR		'1'
#define OFF_STR		'0'

#define IO_NAME_MAX  50



int ioctrl_init(void);
int get_io_val(const char *name);
int set_io_val(const char *name,int val);



#endif






