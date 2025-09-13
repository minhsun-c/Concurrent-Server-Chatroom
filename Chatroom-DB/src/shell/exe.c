#include "includes.h"

int exe_shell(const command_t *cmd)
{
    if (cmd == NULL) /* command not exist */
    {
        return -1;
    }
    if (cmd->next == NULL) /* single command */
    {
        return single_command_handler(cmd);
    }
    else /* multiple command, i.e. piped */
    {
        return multiple_command_handler(cmd);
    }
}