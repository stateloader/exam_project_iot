#ifndef DRIVERS__H
#define DRIVERS__H

#include "configs.h"

//SM states in 'server_routine'.
#define SENSOR_STATE_INIT 0
#define SENSOR_STATE_READ 1
#define SENSOR_STATE_PUSH 2
#define SENSOR_STATE_BUFF 3

#define SENSOR_SLEEP_MODE 4

void sensor_routine(void);

#endif
