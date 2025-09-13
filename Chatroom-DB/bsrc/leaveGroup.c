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
    if (argc < 2)
    {
        printf("Argument not enough\n");
        exit(EXIT_FAILURE);
    }
    /* Initialize mysql connection */
    MYSQL *con = sql_start();

    MYSQL_ROW row;
    char sender[30];
    getSender(sender);

    /* Check group */
    char sql[100];
    sprintf(sql, "SELECT * FROM groupList");
    if (mysql_query(con, sql))
        finish(con);

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
            if (strcmp(row[2], sender) == 0)
                gexist = 2;
            break;
        }
    }
    mysql_free_result(result);
    if (gexist == 0)
    {
        printf("Group not found !\n");
        finish(con);
    }
    else if (gexist == 2)
    {
        printf("Owner of group cannot leave the group !\n");
        finish(con);
    }

    sprintf(sql, "DELETE FROM %s_gdetail WHERE member='%s'", argv[1], sender);
    if (mysql_query(con, sql))
        finish(con);

    /* Close connection & Write back result */
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
