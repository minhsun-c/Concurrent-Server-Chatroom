#include "includes.h"
extern builtin_command_t *builtin_list;

/*
    typedef struct command_data
    {
        char name[MAX_COMMAND_NAME_SIZE];
        char parameter[MAX_COMMAND_PARAM_COUNT][MAX_COMMAND_PARAM_SIZE];
        int param_count;
        char full_path[MAX_FULL_PATH_SIZE];
        void (*fptr)(void *);
    } command_data_t;

    typedef struct command
    {
        command_data_t data;
        int output_pipe;       // output to next command by pipe
        int number;            // parse from input, indicate how many cycle after
        int output_numpipe_id; // output to numpipe buffer
        int input_numpipe_id;  // input from numpipe buffer
        int type;              // type: no fork, builtin, non-builtin, error
        struct command *next;
    } command_t;
*/

static int find_next_pipe(char *input)
{
    for (int i = 0; i < strlen(input) - 1; i++)
    {
        if (input[i] == '|' && input[i + 1] == ' ')
        {
            input[i] = 0;
            return i;
        }
    }
    return NOT_FOUND;
}

static int check_builtin_and_set(command_t *cmd)
{
    builtin_command_t *ptr = builtin_list;
    while (ptr)
    {
        if (strncmp(ptr->name, cmd->data.name, strlen(ptr->name)) == 0)
        {
            cmd->data.fptr = ptr->fptr;
            cmd->type = ptr->type;
            return 1;
        }
        ptr = ptr->next;
    }
    return NOT_FOUND;
}

static int check_non_builtin_and_set(command_t *cmd)
{
    env_node_t *list = env_find_val_list("PATH");
    char filename[32];
    char cwd[100];
    while (list)
    {
        sprintf(filename, "%s/%s", list->data, cmd->data.name);
        if (access(filename, X_OK) == 0)
        {
            strncpy(cmd->data.full_path, filename, strlen(filename));
            cmd->type = TYPE_NON_BUILTIN;
            return 1;
        }
        list = list->next;
    }
    return NOT_FOUND;
}

static void set_command_data(command_t *cmd, char *input)
{
    while (*input == ' ')
        input++;
    char *temp = strsep(&input, " ");
    strncpy(cmd->data.name, temp, strlen(temp));
    for (int i = 0; 1; i++)
    {
        temp = strsep(&input, " ");
        if (temp == NULL)
        {
            cmd->data.param_count = i;
            break;
        }
        if (strlen(temp) > 0)
            strncpy(cmd->data.parameter[i], temp, strlen(temp));
        else
            i--;
    }
}

static void set_command_type(command_t *cmd)
{
    int builtin_res = check_builtin_and_set(cmd);
    if (builtin_res != NOT_FOUND)
        return;
    int non_builtin_res = check_non_builtin_and_set(cmd);
    if (non_builtin_res != NOT_FOUND)
        return;
    cmd->type = TYPE_ERROR;
}

static void set_command_number(command_t *cmd, char *input)
{
    for (int i = 0; i < strlen(input) - 1; i++)
    {
        if (input[i] == '|' && input[i + 1] != ' ')
        {
            input[i] = 0;
            cmd->number = atoi(input + i + 1);
            cmd->output_numpipe_id = insert_numpipe_info(cmd->number);
            return;
        }
    }
    cmd->number = NOT_FOUND;
    cmd->output_numpipe_id = NOT_FOUND;
}

static command_t *construct_command(char *input, int pipe)
{
    command_t *cmd = (command_t *)malloc(sizeof(command_t));
    ERR_MSG_MEM(cmd);
    set_command_number(cmd, input); /* set command number, i.e. |'2' and set to numpipe list*/
    set_command_data(cmd, input);   /* set command name, param, param_count */
    set_command_type(cmd);          /* set command type, func_ptr, fullpath */
    cmd->output_pipe = pipe;        /* set command has pipe or not */
    return cmd;
}

command_t *parse(char *input)
{
    command_t *cmd = NULL, *ptr;
    int modify_numpipe = 0;
    while (1)
    {
        int pipe_id = find_next_pipe(input);
        if (pipe_id == NOT_FOUND) /* last command node */
        {
            if (cmd == NULL) /* single command */
            {
                cmd = construct_command(input, 0);
                cmd->input_numpipe_id = do_numpipe(
                    (cmd->type == TYPE_ERROR) ? -2 : cmd->output_numpipe_id);
            }
            else /* last of piped command */
            {
                ptr->next = construct_command(input, 0);
                ptr->next->input_numpipe_id = -1;
            }
            break;
        }
        if (cmd == NULL) /* first command node */
        {
            cmd = construct_command(input, 1);
            ptr = cmd;
            cmd->input_numpipe_id = do_numpipe(
                (cmd->type == TYPE_ERROR) ? -1 : cmd->output_numpipe_id);
        }
        else /* internal command node */
        {
            ptr->next = construct_command(input, 1);
            ptr->next->input_numpipe_id = -1;
            ptr = ptr->next;
        }
        input += pipe_id + 1;
    }
    return cmd;
}