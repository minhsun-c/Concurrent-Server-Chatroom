void monitor_client(int pid, struct sockaddr_in *cliaddr);

int connect_client(int connfd);

void reset_server_socket(int *ssocket, struct sockaddr_in *serv_addr,
                         char *serv_ip, int serv_port);

void set_ip_port(int argc, char **argv, char *ip, int *port);