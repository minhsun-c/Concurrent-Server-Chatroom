#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct usr
{
    int uid;
    char name[30];
    char ip[15];
    int port;
    pid_t pid;
} usr_t;

usr_t users[100];
int cnt = 0;

char userFile[20] = "tmp/usrList.txt";

void insert(int uid, char *name, char *ip, int port, pid_t pid)
{
    users[cnt].uid = uid;
    users[cnt].port = port;
    users[cnt].pid = pid;
    strncpy(users[cnt].name, name, strlen(name));
    strncpy(users[cnt].ip, ip, strlen(ip));
    cnt++;
}

void change_name(FILE *fp)
{
    for (int i = 0; i < cnt; i++)
    {
        char temp[100];
        sprintf(temp, "%d %s %s %d %d\n",
                users[i].uid, users[i].name, users[i].ip,
                users[i].port, users[i].pid);
        fwrite(temp, sizeof(char), strlen(temp), fp);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Name Not Given");
        return 0;
    }
    int uid, port, pid;
    char ip[15], name[30];
    pid_t parent = getppid();

    FILE *fin = fopen(userFile, "r");
    while (fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid) != EOF)
    {
        if (strncmp(name, argv[1], strlen(name)) == 0)
        {
            printf("name %s existed\n", argv[1]);
            return 0;
        }
        if (pid == parent)
            insert(uid, argv[1], ip, port, pid);
        else
            insert(uid, name, ip, port, pid);
    }
    fclose(fin);
    FILE *fout = fopen(userFile, "w");
    change_name(fout);
    printf("name %s changed\n", argv[1]);
    fclose(fout);
}