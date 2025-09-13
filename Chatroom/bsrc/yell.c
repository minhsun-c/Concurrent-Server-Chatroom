#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "mysignal.h"

/*
    me: user 0
    target: user 2

    tell 2 Hello World
    <user 0 told you>: Hello World
*/
char userFile[20] = "tmp/usrList.txt";

static void write_buf(int argc, char **argv, int myuid)
{

    char filename[50];
    sprintf(filename, "tmp/yell_%d.txt", getppid());
    FILE *fp = fopen(filename, "w");

    char header[100];
    sprintf(header, "<user(%d) yelled>: ", myuid);
    fwrite(header, sizeof(char), strlen(header), fp);

    for (int i = 1; i < argc; i++)
    {
        if (strlen(argv[i]) >= 256)
            fwrite(argv[i], sizeof(char), 255, fp);
        else
            fwrite(argv[i], sizeof(char), strlen(argv[i]), fp);
        fwrite(" ", sizeof(char), strlen(" "), fp);
    }
}

static int getMyUid(pid_t _pid)
{
    int uid, port, pid;
    char name[100], ip[20];
    FILE *fin = fopen(userFile, "r");
    while (fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid) != EOF)
    {
        if (_pid == pid)
            return uid;
    }
    fclose(fin);
    return -1;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Message Not Given\n");
        return 0;
    }
    int myuid = getMyUid(getppid());
    write_buf(argc, argv, myuid);
}