#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void finish(MYSQL *con)
{
    mysql_close(con);
    exit(EXIT_FAILURE);
}

void checkExist(MYSQL *con, char *username)
{
    char sql[100];
    sprintf(sql, "SELECT * FROM user WHERE username='%s'", username);
    if (mysql_query(con, sql))
        finish(con);

    /* Get returned data */
    MYSQL_RES *result;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    int num_rows = mysql_num_rows(result);

    if (num_rows == 0)
    {
        printf("User not found!\n");
        mysql_close(con);
        exit(EXIT_SUCCESS);
    }
}

void getSender(char *sender)
{
    char filename[30];
    sprintf(filename, "tmp/usr%d.txt", getppid());
    FILE *fp = fopen(filename, "r");
    fscanf(fp, "%s", sender);
    fclose(fp);
}

int main(int argc, char **argv)
{
    fprintf(stderr, "mailto start\n");

    /* Initialize mysql connection */
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL)
        finish(con);

    if (mysql_real_connect(con, "localhost", "root", "password", "NPHW3", 0, NULL, 0) == NULL)
        finish(con);

    checkExist(con, argv[1]);

    /* Fetch data from database */
    char sql[200];
    char sender[30];
    getSender(sender);

    char msg[150];
    memset(msg, 0, 150);
    for (int i = 2; i < argc; i++)
    {
        strcat(msg, argv[i]);
        if (i < argc - 1)
            strcat(msg, " ");
    }

    sprintf(sql,
            "INSERT INTO %s_mailbox (mail_from, message) VALUES ('%s', '%s')",
            argv[1], sender, msg);
    fprintf(stderr, "%s\n", sql);
    if (mysql_query(con, sql))
        finish(con);

    mysql_close(con);
    printf("Send Accept!\n");
    exit(EXIT_SUCCESS);
}