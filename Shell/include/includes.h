#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "data.h"

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

#define DO_PIPE(plist, i)     \
    if (pipe((plist[i])) < 0) \
    {                         \
        perror("Pipe error"); \
        free_pipe(plist, i);  \
        exit(EXIT_FAILURE);   \
        return -1;            \
    }

#define NOT_FOUND -1

#define TYPE_NO_FORK 0
#define TYPE_BUILTIN 1
#define TYPE_NON_BUILTIN 2
#define TYPE_ERROR 3

#define PIPE_WRITE_END 1
#define PIPE_READ_END 0

void myprintenv(void *param);
void mysetenv(void *param);
env_node_t *env_find_val_list(char *name);
int single_command_handler(const command_t *);
int dup2_numpipe(int idx, int RorW);
int exe_shell(const command_t *cmd);
int multiple_command_handler(const command_t *cmd);
command_t *parse(char *input);
int do_numpipe(int);
int insert_numpipe_info(int cycle_after);
int init_numpipe();
void env_init();
void init_builtin_list();