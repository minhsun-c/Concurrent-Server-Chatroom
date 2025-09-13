#include "includes.h"

static void child_handler(int param)
{
    (void)param;
    int childpid = 0;
    int saved_errno = errno;
    fprintf(stderr, "%d child exit\n\r", getpid());
    while ((childpid = waitpid((pid_t)(-1), 0, WNOHANG)) > 0)
    {
        errno = saved_errno;
        clinode_t *target = search_clinode_pid(childpid);
        if (target)
        {
            remove_clinode(target);
            fprintf(stderr, "%d child stop\n\r", childpid);
        }
        for (int i = 0; i < 10; i++)
        {
            char file[30];
            sprintf(file, "tmp/buf%d_%d.txt", childpid, i);
            remove(file);
        }
        char yfile[30];
        char tfile[30];
        sprintf(yfile, "tmp/yell_%d.txt", childpid);
        sprintf(tfile, "tmp/tell_%d.txt", childpid);
        remove(yfile);
        remove(tfile);
    }
}

void signal_child_ctrl(void)
{
    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_handler = child_handler;
    sigaction(SIGCHLD, &action, NULL); /* if child terminates or stop */
}

#ifdef HOMEWORK2_FUNCTION

static void com_handler(int sig, siginfo_t *info, void *context) /* pass msg to main server */
{
    (void)context;
    char buffer[100];
    pid_t sender = info->si_pid;
    if (sig == SIG_MYTELL)
        sprintf(buffer, "tmp/tell_%d.txt", sender);
    else
        sprintf(buffer, "tmp/yell_%d.txt", sender);

    FILE *fp = fopen(buffer, "r");
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        printf("%s\n", buffer);
    }
    fclose(fp);
}

void signal_tell_ctrl()
{
    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_sigaction = com_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIG_MYTELL, &action, NULL); /* if "tell" is executed */
}

void signal_yell_ctrl()
{
    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_sigaction = com_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIG_MYYELL, &action, NULL); /* if "yell" is executed */
}

#endif // HOMEWORK2_FUNCTION

#ifdef HOMEWORK3_FUNCTION

static void com_handler(int sig, siginfo_t *info, void *context) /* pass msg to main server */
{
    (void)context;
    char buffer[100];
    pid_t sender = info->si_pid;
    if (sig == SIG_MYYELL)
        sprintf(buffer, "tmp/tell_%d.txt", sender);

    FILE *fp = fopen(buffer, "r");
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        printf("%s\n", buffer);
    }
    fclose(fp);
}

void signal_gyell_ctrl()
{
    struct sigaction action;
    bzero(&action, sizeof(action));
    action.sa_sigaction = com_handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIG_MYYELL, &action, NULL); /* if "yell" is executed */
}

#endif // HOMEWORK3_FUNCTION