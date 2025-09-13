#include "includes.h"
#define __TEST_SHELL

int init_shell()
{
    init_builtin_list();
    env_init();
    init_numpipe();
}

int main()
{
    init_shell();
#ifdef __TEST_COMMAND
    int cnt = 0;
    int strsize = 4;
    char str[][128] = {
        "ls | number | number |1\n",
        "ppppppp\n",
        "number\n",
        "cat Makefile\n"};
#endif // __TEST_COMMAND
#ifdef __TEST_SHELL
    char input[128];
#endif // __TEST_SHELL
    command_t *cmd;
    while (1)
    {
        printf("unix> ");
#ifdef __TEST_COMMAND
        if (cnt == strsize)
            break;
        char input[128];
        printf("round: %d\n", cnt);
        strncpy(input, str[cnt++], sizeof(input));
#endif // __TEST_COMMAND
#ifdef __TEST_SHELL
        fgets(input, sizeof(input), stdin);
#endif // __TEST_SHELL
        int len = strlen(input);
        input[len - 1] = 0;
        if (strlen(input) == 0)
            continue;

        if ((cmd = parse(input)) == NULL)
        {
            continue;
        }
        if (exe_shell(cmd) == -1)
            break;
    }
}