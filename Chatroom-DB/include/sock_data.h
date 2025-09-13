#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifndef __SOCKET_DATA
#define __SOCKET_DATA

typedef struct server_socket
{
    pid_t pid;
    in_port_t port;
    struct in_addr ip;
    int read_pipefd;  /* input to main server */
    int write_pipefd; /* output from main server */
    struct server_socket *next;
    struct server_socket *prev;
} clinode_t;

#endif // __SOCKET_DATA