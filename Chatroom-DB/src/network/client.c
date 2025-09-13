#include "includes.h"

static void start_shell(char *username)
{
    init_shell();
    int counter = 0;
    while (1)
    {
        if (open_shell(username, counter) < 0)
        {
            exit(EXIT_FAILURE);
            return;
        }
        counter = 1;
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
    char *username;
    username = login2db();
    char filename[30];
    sprintf(filename, "tmp/usr%d.txt", getpid());
    FILE *fp = fopen(filename, "w");
    fwrite(username, sizeof(char), strlen(username), fp);
    fwrite("\n", sizeof(char), 1, fp);
    fclose(fp);
    start_shell(username);
    exit(EXIT_SUCCESS);
    return 1;
}
