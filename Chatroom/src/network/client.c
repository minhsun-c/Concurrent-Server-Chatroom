#include "includes.h"

static void start_shell()
{
    init_shell();
    while (1)
    {
        if (open_shell() < 0)
        {
            exit(EXIT_FAILURE);
            return;
        }
    }
}

static int init_client(int connfd)
{
    DO_DUP2(dup2(connfd, STDOUT_FILENO));
    DO_DUP2(dup2(connfd, STDIN_FILENO));
    close(connfd);
    clear_clinode();
    setbuf(stdout, NULL);
    return 0;
}

int connect_client(int connfd)
{
    init_client(connfd);
    start_shell();
    exit(EXIT_SUCCESS);
    return 1;
}
