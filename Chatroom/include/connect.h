#include "sock_data.h"

#ifndef __CONNECT_SERVER
#define __CONNECT_SERVER

clinode_t *create_clinode(int pid, struct sockaddr_in *cliaddr);

int insert_clinode(clinode_t *node);

int remove_clinode(clinode_t *node);

void clear_clinode();

clinode_t *search_clinode_pid(int pid);

pid_t match_pid(int uid);

#endif // __CONNECT_SERVER