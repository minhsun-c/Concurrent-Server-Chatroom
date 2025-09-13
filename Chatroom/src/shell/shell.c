#include "includes.h"

int init_shell()
{
    init_builtin_list();
    env_init();
    init_numpipe();
}

int open_shell()
{
    char input[128] = "\0";
    command_t *cmd;
    fprintf(stdout, "unix> ");
    memset(input, 0, sizeof(input));
    fgets(input, sizeof(input), stdin);
    int len = strlen(input);
    input[len - 2] = 0; /* remove \r\n */
    fprintf(stderr, "command: [%s]\n", input);
    if (strlen(input) == 0)
        return 1;

    if ((cmd = parse(input)) == NULL)
    {
        return 1;
    }
    if (exe_shell(cmd) == -1)
        return -1;
}
