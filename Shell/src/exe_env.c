#include "includes.h"

env_t *env;

static int env_find_name(char *name)
{
    for (int i = 0; i < env->used; i++)
    {
        if (strncmp(name, env->name[i], strlen(env->name[i])) == 0)
        {
            return i;
        }
    }
    return NOT_FOUND;
}

static env_node_t *env_construct_new_node(char *value)
{
    env_node_t *node = (env_node_t *)malloc(sizeof(env_node_t));
    ERR_MSG_MEM(node);
    strncpy(node->data, value, strlen(value));
    node->next = NULL;
    return node;
}

static void env_insert_with_idx(int idx, char *value)
{
    if (env->list[idx] == NULL)
    {
        env->list[idx] = env_construct_new_node(value);
    }
    else
    {
        env_node_t *ptr = env->list[idx];
        while (ptr->next)
            ptr = ptr->next;
        ptr->next = env_construct_new_node(value);
    }
}

static void env_insert(char *name, char *value)
{
    int idx = env_find_name(name);
    if (idx == NOT_FOUND)
    {
        strncpy(env->name[env->used], name, strlen(name));
        env_insert_with_idx(env->used, value);
        env->used++;
    }
    else
    {
        env_insert_with_idx(idx, value);
    }
}

static void my_printenv_exe(int idx)
{
    env_node_t *ptr = env->list[idx];
    if (ptr == NULL)
        printf("Invalid env name\n");
    printf("[%s] = ", env->name[idx]);
    printf("%s", ptr->data);
    ptr = ptr->next;
    while (ptr)
    {
        printf(":%s", ptr->data);
        ptr = ptr->next;
    }
    printf("\n");
}

static void my_printenv_name(char *name)
{
    int idx = env_find_name(name);
    if (idx == NOT_FOUND || idx >= env->used)
        printf("Invalid env name\n");
    else
        my_printenv_exe(idx);
}

static void my_printenv_all()
{
    for (int i = 0; i < env->used; i++)
    {
        my_printenv_exe(i);
    }
}

void myprintenv(void *param)
{
    command_t *cmd = (command_t *)param;
    if (cmd->data.param_count == 0)
    {
        my_printenv_all();
    }
    else if (cmd->data.param_count == 1)
    {
        my_printenv_name(cmd->data.parameter[0]);
    }
    else
    {
        printf("Invalid number of parameter\n");
    }
}

void mysetenv(void *param)
{
    command_t *cmd = (command_t *)param;
    if (cmd->data.param_count != 2)
        printf("Invalid number of parameter\n");
    else
        env_insert(cmd->data.parameter[0], cmd->data.parameter[1]);
}

void env_init()
{
    env = (env_t *)malloc(sizeof(env_t));
    ERR_MSG_MEM(env);
    env->used = 0;
    memset(env->count, 0, sizeof(env->count));
    memset(env->list, 0, sizeof(env->list));
    env_insert("PATH", ".");
    env_insert("PATH", "./bin");
    env_insert("PATH", "/bin");
    env_insert("PATH", "/usr/bin");
}

env_node_t *env_find_val_list(char *name)
{
    int idx = env_find_name(name);
    if (idx == NOT_FOUND)
        return NULL;
    return env->list[idx];
}