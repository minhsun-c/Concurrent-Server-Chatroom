#include "includes.h"

static void free_pipe(int **plist, int size)
{
    for (int i = 0; i < size; i++)
        free(plist[i]);
    free(plist);
}

static int **init_pipe(int size)
{
    int **plist = (int **)malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++)
    {
        plist[i] = (int *)malloc(sizeof(int) * 2);
    }
    return plist;
}

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

static int dup2_pipe(int **fd, int mode, int idx)
{
    if (mode == PIPE_READ_END)
    {
        close(fd[idx][PIPE_WRITE_END]);
        DO_DUP2(dup2(fd[idx][PIPE_READ_END], STDIN_FILENO));
        close(fd[idx][PIPE_READ_END]);
    }
    else /* PIPE_WRITE_END */
    {
        close(fd[idx][PIPE_READ_END]);
        DO_DUP2(dup2(fd[idx][PIPE_WRITE_END], STDOUT_FILENO));
        close(fd[idx][PIPE_WRITE_END]);
    }
}

static int doChild(const command_t *cmd, int **fd, int idx)
{
    // printf("CC command name: %s\n", cmd->data.name);
    dup2_numpipe(cmd->input_numpipe_id, PIPE_READ_END);
    dup2_numpipe(cmd->output_numpipe_id, PIPE_WRITE_END);
    if (cmd->next != NULL) /* not the last command: dup2 write */
    {
        // printf("CC close write %d [%s]\n", idx, cmd->data.name);
        dup2_pipe(fd, PIPE_WRITE_END, idx);
    }
    if (idx != 0) /* not the first command: dup2 read */
    {
        // printf("CC close read %d [%s]\n", idx - 1, cmd->data.name);
        dup2_pipe(fd, PIPE_READ_END, idx - 1);
    }
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

static void close_parent_pipe(const command_t *cmd, int **fd, int idx)
{
    // printf("PP command name %s\n", cmd->data.name);
    if (idx != 0) /* not first command */
    {
        // printf("PP close %d\n", idx - 1);
        close(fd[idx - 1][PIPE_READ_END]);
        close(fd[idx - 1][PIPE_WRITE_END]);
    }
}

static int doParent_with_idx(const command_t *cmd, int **fd, int idx)
{
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
        perror("Fork error");
        exit(EXIT_FAILURE);
        return -1;
    case 0:
        doChild(cmd, fd, idx);
        return 1;
    default:
        close_parent_pipe(cmd, fd, idx);
        waitpid(pid, NULL, 0);
        return 1;
    }
}

static int doParent(const command_t *cmd, int **fd)
{
    for (int i = 0; cmd; i++)
    {
        if (cmd->type == TYPE_ERROR)
        {
            printf("Invalid command: [%s]\n", cmd->data.name);
        }
        else if (cmd->type == TYPE_NO_FORK)
        {
            cmd->data.fptr(cmd);
        }
        if (cmd->next)
            DO_PIPE(fd, i);
        doParent_with_idx(cmd, fd, i);
        cmd = cmd->next;
    }
    return 1;
}

static int get_command_size(const command_t *cmd)
{
    int cnt = 0;
    while (cmd)
    {
        cnt++;
        cmd = cmd->next;
    }
    return cnt;
}

int multiple_command_handler(const command_t *cmd)
{
    int command_size = get_command_size(cmd) - 1;
    int **fd = init_pipe(command_size);
    return doParent(cmd, fd);
}