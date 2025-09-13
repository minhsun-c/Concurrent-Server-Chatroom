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

MYSQL *sql_start()
{
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL)
        finish(con);

    if (mysql_real_connect(con, "localhost", "root", "password", "NPHW3", 0, NULL, 0) == NULL)
        finish(con);
    return con;
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
    if (argc < 2)
    {
        printf("Argument not enough\n");
        exit(EXIT_FAILURE);
    }
    /* Initialize mysql connection */
    MYSQL *con = sql_start();

    char sender[50];
    getSender(sender);

    /* Fetch data from database */
    char sql[100];

    sprintf(sql, "SELECT * FROM groupList");
    if (mysql_query(con, sql))
        finish(con);

    MYSQL_RES *result, *result2;
    int approved = 0;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    int num_rows = mysql_num_rows(result);

    MYSQL_ROW row;
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        if (strcmp(row[1], argv[1]) == 0)    /* group name */
            if (strcmp(row[2], sender) == 0) /* owner */
                approved = 1;
    }
    if (approved == 0)
    {
        printf("Only the group owner can remove a user !!\n");
    }
    else
    {
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], sender) == 0)
            {
                printf("Cannot remove group owner !\n");
                continue;
            }
            else
            {
                sprintf(sql, "SELECT * FROM %s_gdetail WHERE member='%s'",
                        argv[1], argv[i]);
                if (mysql_query(con, sql))
                    finish(con);
                if ((result2 = mysql_store_result(con)) == NULL)
                    finish(con);
                if (mysql_num_rows(result2) == 0)
                {
                    printf("%s not in the group\n", argv[i]);
                    mysql_free_result(result2);
                }
                else
                {
                    sprintf(sql,
                            "DELETE FROM %s_gdetail WHERE member='%s'",
                            argv[1], argv[i]);
                    if (mysql_query(con, sql))
                        finish(con);
                    printf("%s removed\n", argv[i]);
                }
            }
        }
    }

    /* Close connection & Write back result */
    mysql_free_result(result);
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
