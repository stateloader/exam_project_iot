#ifndef DRIVERS__H
#define DRIVERS__H

#include "configs.h"

//SM states in 'server_routine'.
#define SERVER_STATE_INIT 0
#define SERVER_STATE_READ 1
#define SERVER_STATE_PUSH 2
#define SERVER_STATE_BUFF 3

#define SERVER_SLEEP_MODE 4

void server_routine(void);

#endif
