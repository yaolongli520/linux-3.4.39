#ifndef	__HUMAN__H
#define __HUMAN__H

#include <stdint.h>
#include <stdlib.h>



#define HUM_FAIL  (-1)
#define HUM_MOUDLE_DELAY		9000  /* ms */

enum mod_status{
	TARGET_NONE = 0,
	TARGET_EXIST,
	MACH_SUSPEND,
};

int human_init(void);
int get_hunman_status(void); /*获取红外状态*/







#endif





