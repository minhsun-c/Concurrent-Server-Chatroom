#include "includes.h"

builtin_command_t *builtin_list;

static builtin_command_t *construct_builtin_node(char *name, void (*fp)(void *), int type)
{
    builtin_command_t *node = (builtin_command_t *)malloc(sizeof(builtin_command_t));
    ERR_MSG_MEM(node);
    strncpy(node->name, name, strlen(name) + 1);
    node->name[strlen(name)] = 0;
    node->fptr = fp;
    node->type = type;
    node->next = NULL;
    return node;
}

static void insert_builtin(char *name, void (*fp)(void *), int type)
{
    if (builtin_list == NULL)
    {
        builtin_list = construct_builtin_node(name, fp, type);
    }
    else
    {
        builtin_command_t *ptr = builtin_list;
        while (ptr->next)
            ptr = ptr->next;
        ptr->next = construct_builtin_node(name, fp, type);
    }
}

void myhelp(void *param)
{
    (void)param;
    printf("help\n\r");
}

void myquit(void *param)
{
    (void)param;
    printf("Close shell\n\r");
    exit(EXIT_SUCCESS);
}

void init_builtin_list()
{
    builtin_list = NULL;
    insert_builtin("quit", myquit, TYPE_NO_FORK);
    insert_builtin("help", myhelp, TYPE_BUILTIN);
    insert_builtin("setenv", mysetenv, TYPE_NO_FORK);
    insert_builtin("printenv", myprintenv, TYPE_BUILTIN);
}
