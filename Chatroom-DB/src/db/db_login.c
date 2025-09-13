#include "includes.h"

#define TYPE_NOTLOGIN 0
#define TYPE_LOGIN_SUCCESS 1
#define TYPE_LOGIN_FAILURE 2
#define TYPE_REGISTER 3

#define EXIT_LOGIN_SUCCESS 0
#define EXIT_LOGIN_FAILURE 1
#define EXIT_LOGIN_PASSWORD_ERROR 2
#define EXIT_LOGIN_NAME_ERROR 3

#define EXIT_REGISTER_SUCCESS 0
#define EXIT_REGISTER_FAILURE 1

#define MODE_LOGIN 0
#define MODE_REGISTER 1

static int get_login_register_from_file(int mode)
{
    char file[50];
    int status;
    memset(file, 0, 50);
    if (mode == MODE_LOGIN)
        sprintf(file, "tmp/login%d.txt", getpid());
    else
        sprintf(file, "tmp/register%d.txt", getpid());
    FILE *fp = fopen(file, "r");
    fscanf(fp, "%d", &status);
    fclose(fp);
    return status;
}

static void enter_user_passwd(char *username, char *password)
{
    memset(username, 0, 50);
    memset(password, 0, 50);
    printf("[User Name]: ");
    scanf("%s", username);
    printf("[Password]: ");
    scanf("%s", password);
}

static int exe_login_register(char *username, char *password, int mode)
{
    char *arg[4];
    char arglist[3][50];
    memset(arglist, 0, sizeof(arglist));
    if (mode == MODE_LOGIN)
        strncpy(arglist[0], "bin/login", sizeof("bin/login"));
    else
        strncpy(arglist[0], "bin/register", sizeof("bin/register"));
    strncpy(arglist[1], username, 50);
    strncpy(arglist[2], password, 50);
    arg[0] = arglist[0];
    arg[1] = arglist[1];
    arg[2] = arglist[2];
    arg[3] = NULL;
    if (execvp(arg[0], arg) < 0)
    {
        perror("execvp error");
        exit(EXIT_FAILURE);
        return -1;
    }
    return 1;
}

static int fork_register()
{
    char username[50];
    char password[50];
    pid_t pid;
    int status;
    printf("Creating Account ...\n");
    enter_user_passwd(username, password);
    switch (pid = fork())
    {
    case -1:
        perror("fork");
        break;
    case 0: /* child: register process */
        exe_login_register(username, password, MODE_REGISTER);
        break;
    default: /* concurrent server */
        waitpid(pid, &status, 0);
        int status = get_login_register_from_file(MODE_REGISTER);
        if (status == EXIT_REGISTER_FAILURE)
            printf("Register FAILURE\n");
        else
            printf("Register SUCCESS\n");
    }
}

static void remove_login_file()
{
    char file[50];
    memset(file, 0, 50);
    sprintf(file, "tmp/login%d.txt", getpid());
    remove(file);
}

static int get_login_status()
{
    int status = get_login_register_from_file(MODE_LOGIN);
    remove_login_file();
    switch (status)
    {
    case EXIT_LOGIN_SUCCESS:
        return TYPE_LOGIN_SUCCESS;
    case EXIT_LOGIN_NAME_ERROR:
        printf("User not found !\n");
        printf("Create account or login again ? <1/2> : ");
        int option;
        scanf("%d", &option);
        if (option == 1)
            return TYPE_REGISTER;
        else
            return TYPE_LOGIN_FAILURE;
    case EXIT_LOGIN_PASSWORD_ERROR:
        printf("Password error !\n");
        return TYPE_LOGIN_FAILURE;
    default:
        printf("Unknown Error\n");
        break;
    }
}

char *login2db()
{
    pid_t pid;
    char *username;
    char *password;
    ERR_MSG_MEM(username = (char *)malloc(50));
    ERR_MSG_MEM(password = (char *)malloc(50));
    int login_status = TYPE_NOTLOGIN;
    int status;
    while (1)
    {
        enter_user_passwd(username, password);
        switch (pid = fork())
        {
        case -1:
            perror("fork");
            break;
        case 0: /* child: login process */
            exe_login_register(username, password, MODE_LOGIN);
            break;
        default: /* concurrent server */
            waitpid(pid, &status, 0);
            login_status = get_login_status();
            if (login_status == TYPE_REGISTER)
                fork_register();
            else if (login_status == TYPE_LOGIN_SUCCESS)
                return username;
        }
    }
    free(password);
}