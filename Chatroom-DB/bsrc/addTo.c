#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INSERT_MEMBER_TEMPLATE \
    "INSERT INTO %s_gdetail (member) VALUES ('%s')"

void finish(MYSQL *con)
{
    mysql_close(con);
    exit(EXIT_FAILURE);
}

void getSender(char *sender)
{
    char filename[30];
    sprintf(filename, "tmp/usr%d.txt", getppid());
    FILE *fp = fopen(filename, "r");
    fscanf(fp, "%s", sender);
    fclose(fp);
}

void check_member_exist(MYSQL *con, int argc, char **argv, int *status)
{
    char sql[100];
    MYSQL_ROW row;
    sprintf(sql, "SELECT * FROM user");
    if (mysql_query(con, sql))
        finish(con);
    MYSQL_RES *result;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    int num_rows = mysql_num_rows(result);
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], row[1]) == 0)
            {
                status[i - 2] = -1;
            }
        }
    }
    mysql_free_result(result);
}

int main(int argc, char **argv)
{
    int status[argc - 2];
    memset(status, 0, sizeof(int) * (argc - 2));
    /* Initialize mysql connection */
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL)
        finish(con);

    if (mysql_real_connect(con, "localhost", "root", "password", "NPHW3", 0, NULL, 0) == NULL)
        finish(con);
    MYSQL_ROW row;
    char sender[30];
    getSender(sender);

    check_member_exist(con, argc, argv, status);

    /* Fetch data from database */
    char sql[100];
    sprintf(sql, "SELECT * FROM groupList");
    if (mysql_query(con, sql))
        finish(con);

    /* Get returned data */
    MYSQL_RES *result;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    int num_rows = mysql_num_rows(result);
    int gexist = 0;
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        if (strcmp(row[1], argv[1]) == 0)
        {
            gexist = 1;
            break;
        }
    }
    mysql_free_result(result);
    if (gexist == 0)
    {
        printf("Group not found !\n");
        finish(con);
    }

    sprintf(sql, "SELECT * FROM %s_gdetail", argv[1]);
    if (mysql_query(con, sql))
        finish(con);

    int approved = 0;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    num_rows = mysql_num_rows(result);
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        if (strcmp(row[1], sender) == 0)
        {
            approved = 1;
        }
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(row[1], argv[i]) == 0)
            {
                status[i - 2] = 1;
            }
        }
    }
    if (approved == 0)
    {
        printf("Permission Denied!\n");
    }
    for (int i = 2; i < argc; i++)
    {
        if (status[i - 2] == -1) /* Not exist in Group */
        {
            sprintf(sql, INSERT_MEMBER_TEMPLATE, argv[1], argv[i]);
            if (mysql_query(con, sql))
                finish(con);
            printf("%s added to group\n", argv[i]);
        }
        else if (status[i - 2] == 0) /* Not user */
        {
            printf("%s not a user\n", argv[i]);
        }
        else /* Existed */
        {
            printf("%s already in group\n", argv[i]);
        }
    }

    /* Close connection & Write back result */
    mysql_free_result(result);
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
