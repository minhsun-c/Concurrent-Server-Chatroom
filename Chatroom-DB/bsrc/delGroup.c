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
void getSender(char *sender)
{
    char filename[30];
    sprintf(filename, "tmp/usr%d.txt", getppid());
    FILE *fp = fopen(filename, "r");
    fscanf(fp, "%s", sender);
    fclose(fp);
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

int main(int argc, char **argv)
{
    /* Initialize mysql connection */
    MYSQL *con = sql_start();

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

    char sender[30];
    getSender(sender);

    MYSQL_ROW row;
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        if (strcmp(row[1], argv[1]) == 0)
        {
            if (strcmp(row[2], sender) == 0)
            {
                sprintf(sql,
                        "DELETE FROM groupList WHERE group_name = '%s'",
                        row[1]);
                if (mysql_query(con, sql))
                    finish(con);
                sprintf(sql,
                        "DROP TABLE %s_gdetail;",
                        row[1]);
                if (mysql_query(con, sql))
                    finish(con);
                printf("Delete Group %s Success\n", row[1]);
                finish(con);
            }
            else
            {
                printf("Only the owner can delete group !!\n");
                mysql_free_result(result);
                finish(con);
            }
        }
    }

    printf("Group not found !\n");
    /* Close connection & Write back result */
    mysql_free_result(result);
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
