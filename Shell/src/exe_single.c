#include "includes.h"

static int doChild_nonbuiltin(const command_t *cmd)
{
    char *arg[16];
    arg[0] = cmd->data.full_path;
    for (int i = 0; i < cmd->data.param_count; i++)
        arg[i + 1] = cmd->data.parameter[i];
    arg[cmd->data.param_count + 1] = NULL;
    if (execvp(arg[0], arg) < 0)
    {
        perror("execvp error");
        exit(EXIT_FAILURE);
        return -1;
    }
    return 1;
}

static int doChild(const command_t *cmd)
{
    /* if error then do nothing, else dup2 */
    dup2_numpipe(cmd->input_numpipe_id, PIPE_READ_END);
    dup2_numpipe(cmd->output_numpipe_id, PIPE_WRITE_END);

    switch (cmd->type)
    {
    case TYPE_BUILTIN:
        cmd->data.fptr(cmd);
        exit(EXIT_SUCCESS);
        return 1;
    case TYPE_NON_BUILTIN:
        return doChild_nonbuiltin(cmd);
    default:
        printf("doChild error");
        return -1;
    }
}

static int doParent(const command_t *cmd)
{
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
        perror("Fork error");
        exit(EXIT_FAILURE);
        return -1;
    case 0:
        doChild(cmd);
        return 1;
    default:
        waitpid(pid, NULL, 0);
        return 1;
    }
}

int single_command_handler(const command_t *cmd)
{
    switch (cmd->type)
    {
    case TYPE_ERROR:
        printf("Invalid Command: [%s]\n", cmd->data.name);
        return 1;
    case TYPE_NO_FORK:
        cmd->data.fptr(cmd);
        return 1;
    default:
        doParent(cmd);
        return 1;
    }
}