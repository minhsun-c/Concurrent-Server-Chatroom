#include "includes.h"

pid_t serv_pid;

int main(int argc, char *argv[])
{
    char input_ip[20];            /* ip of main server */
    int input_port = 0;           /* port of main server */
    int ssocket;                  /* server socket */
    int csocket;                  /* client socket */
    struct sockaddr_in serv_addr; /* server info: protocol, port, ... */
    struct sockaddr_in cli_addr;  /* client info: protocol, port, ... */
    pid_t pid;                    /* use for fork() */

    set_ip_port(argc, argv, input_ip, &input_port);
    reset_server_socket(&ssocket, &serv_addr, input_ip, input_port);
    bind(ssocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(ssocket, 10);

    serv_pid = getpid();

    while (1)
    {
        printf("Connecting...: [%s]:[%d]\n\r",
               input_ip,
               ntohs(serv_addr.sin_port));
        socklen_t len = sizeof(cli_addr);
        csocket = accept(ssocket, (struct sockaddr *)&cli_addr, &len);
        if (csocket < 0)
        {
            if (errno == EINTR) /* interrupt error */
                continue;
            else /* non-interrupt errors */
            {
                perror("accept");
                close(ssocket);
                exit(EXIT_FAILURE);
            }
        }
        printf("Connect: %d\n\r", csocket);

        switch (pid = fork())
        {
        case -1:
            perror("fork");
            break;
        case 0: /* child */
            close(ssocket);
            signal_tell_ctrl();
            signal_yell_ctrl();
            connect_client(csocket);
            break;
        default: /* server */

            /* handling the following child processes */
            signal_child_ctrl();
            close(csocket);
            monitor_client(pid, &cli_addr);
            waitpid((pid_t)-1, NULL, WNOHANG);
        }
    }
    close(ssocket);
}