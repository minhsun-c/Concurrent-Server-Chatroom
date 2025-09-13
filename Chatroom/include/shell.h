#include "shell_data.h"

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
int open_shell();
int init_shell();