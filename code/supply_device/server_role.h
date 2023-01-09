#ifndef SERVER_ROLE__H
#define SERVER_ROLE__H

//SM states in 'server_routine'.
#define SERVER_STATE_INIT 0
#define SERVER_STATE_DUTY 1
#define SERVER_STATE_PUSH 2
#define SERVER_STATE_BUFF 3

#define SERVER_SLEEP_MODE 4
#define SERVER_ERROR_MODE 5

//Test Intervals for now.
#define SERVER_TIMER_INIT uSSEC
#define SERVER_TIMER_BUFF uSSEC * 2

void server_routine(void);

#endif