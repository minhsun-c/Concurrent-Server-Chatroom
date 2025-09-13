#include "includes.h"

#define NUMPIPE_BUFFER_COUNT 10
#define EMPTY_BUFFER_INFO -1
#define NUMPIPE_NOW 1

int *numpipe_buffer;

static int create_tmp_file()
{
    FILE *file;
    char filename[32];
    for (int i = 0; i < NUMPIPE_BUFFER_COUNT; i++)
    {
        sprintf(filename, "tmp/buf%d.txt", i);
        file = fopen(filename, "w");
        fclose(file);
    }
    return 1;
}

static int find_empty_buffer()
{
    for (int i = 0; i < NUMPIPE_BUFFER_COUNT; i++)
    {
        if (numpipe_buffer[i] == EMPTY_BUFFER_INFO)
            return i;
    }
    return -1;
}

int init_numpipe()
{
    numpipe_buffer = (int *)malloc(sizeof(int) * NUMPIPE_BUFFER_COUNT);
    ERR_MSG_MEM(numpipe_buffer);
    memset(numpipe_buffer, -1, sizeof(int) * NUMPIPE_BUFFER_COUNT);
    create_tmp_file();
    return 1;
}

int insert_numpipe_info(int cycle_after)
{
    if (cycle_after <= 0)
    {
        return -1;
    }
    int next_empty = find_empty_buffer();
    if (next_empty == -1)
        return -1;
    numpipe_buffer[next_empty] = cycle_after;
    return next_empty;
}

#ifdef __DEBUG_MODE_ON
static void print_buffer()
{
    printf("NUMPIPE: \n");
    for (int i = 0; i < NUMPIPE_BUFFER_COUNT; i++)
    {
        printf("%d ", numpipe_buffer[i]);
    }
    printf("\n");
}
#endif // __DEBUG_MODE_ON

int do_numpipe(int idx)
{
    int ret = -1;
    if (idx == -2)
        return -1;
    for (int i = 0; i < NUMPIPE_BUFFER_COUNT; i++)
    {
        if (i == idx)
            continue;
        if (ret == -1 && numpipe_buffer[i] == NUMPIPE_NOW)
        {
            ret = i;
            numpipe_buffer[i] = EMPTY_BUFFER_INFO;
        }
        else if (numpipe_buffer[i] >= 0)
            numpipe_buffer[i]--;
    }
    return ret;
}

int clear_numpipe(int id)
{
    numpipe_buffer[id] = EMPTY_BUFFER_INFO;
    return 1;
}

int close_numpipe_buffer()
{
    free(numpipe_buffer);
    numpipe_buffer = NULL;
    return 1;
}

static int open_file_return_fd(int idx, int open_flag)
{
    char filename[128];
    sprintf(filename, "tmp/buf%d.txt", idx);
    return open(filename, open_flag);
}

int dup2_numpipe(int idx, int RorW)
{
    if (idx < 0)
        return -1;

    int fd;
    if (RorW == PIPE_READ_END)
        fd = open_file_return_fd(idx, O_RDONLY);
    else
        fd = open_file_return_fd(idx, O_WRONLY | O_TRUNC);

    if (fd == -1)
    {
        perror("Input numpipe error");
        exit(EXIT_FAILURE);
        return -1;
    }

    int dup2_ret;
    if (RorW == PIPE_READ_END)
        dup2_ret = dup2(fd, STDIN_FILENO);
    else
        dup2_ret = dup2(fd, STDOUT_FILENO);

    DO_DUP2(dup2_ret);

    close(fd);

    return 1;
}