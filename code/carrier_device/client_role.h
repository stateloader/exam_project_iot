#ifndef CLIENT_ROLE__H
#define CLIENT_ROLE__H

//SM states in 'client_routine'.

#define CLIENT_STATE_SCAN 0
#define CLIENT_STATE_CONN 1
#define CLIENT_STATE_READ 2

#define CLIENT_SLEEP_MODE 3
#define CLIENT_ERROR_MODE 4

void client_routine(void);

#endif
