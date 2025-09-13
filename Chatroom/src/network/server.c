#include "includes.h"

void monitor_client(int pid, struct sockaddr_in *cliaddr)
{
    clinode_t *node = create_clinode(pid, cliaddr);
    insert_clinode(node);
}

void reset_server_socket(int *ssocket, struct sockaddr_in *serv_addr, char *serv_ip, int serv_port)
{
    *ssocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(serv_addr, '0', sizeof(struct sockaddr_in));

    serv_addr->sin_family = AF_INET;                        /* ipv4 */
    serv_addr->sin_addr.s_addr = htonl(inet_addr(serv_ip)); /* listen on ip */
    serv_addr->sin_port = htons(serv_port);                 /* port of server */
}

void set_ip_port(int argc, char **argv, char *ip, int *port)
{
    memset(ip, 0, sizeof(*ip));
    if (argc >= 3)
    {
        printf("ip: [%s]\n", argv[1]);
        strncpy(ip, argv[1], strlen(argv[1]));
        ip[strlen(argv[1])] = 0;
        *port = atoi(argv[2]);
    }
    else if (argc == 2)
    {
        strncpy(ip, "0.0.0.0", strlen("0.0.0.0") + 1);
        ip[7] = 0;
        *port = atoi(argv[1]);
    }
    else
    {
        fprintf(stderr, "ip, port not given\n\r");
    }
}
