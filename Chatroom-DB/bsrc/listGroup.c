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

int main(int argc, char **argv)
{
    /* Initialize mysql connection */
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL)
        finish(con);

    if (mysql_real_connect(con, "localhost", "root", "password", "NPHW3", 0, NULL, 0) == NULL)
        finish(con);

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

    char filename[30];
    char sender[30];
    sprintf(filename, "tmp/usr%d.txt", getppid());
    FILE *fp = fopen(filename, "r");
    fscanf(fp, "%s", sender);
    fclose(fp);

    MYSQL_ROW row;
    printf("<name>\t<owner>\n");
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        printf("%s\t%s\n", row[1], row[2]);
    }

    /* Close connection & Write back result */
    mysql_free_result(result);
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
