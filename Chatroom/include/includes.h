#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "shell.h"
#include "sock_data.h"
#include "server.h"
#include "connect.h"
#include "mysignal.h"

#define ERR_MSG_MEM(par)                   \
    if ((par) == NULL)                     \
    {                                      \
        perror("Memory allocation error"); \
        exit(EXIT_FAILURE);                \
    }

#define DO_DUP2(par)          \
    if ((par) == -1)          \
    {                         \
        perror("dup2 Error"); \
        exit(EXIT_FAILURE);   \
        return -1;            \
    }

#define DO_DUP2_MSG(par, msg) \
    if ((par) == -1)          \
    {                         \
        perror(msg);          \
        exit(EXIT_FAILURE);   \
    }

#define PIPE_WRITE_END 1
#define PIPE_READ_END 0