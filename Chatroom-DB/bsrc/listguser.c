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
    if (argc < 2)
    {
        printf("Argument not enough\n");
        exit(EXIT_FAILURE);
    }
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
    sprintf(sql, "SELECT * FROM %s_gdetail", argv[1]);
    if (mysql_query(con, sql))
        finish(con);

    /* Get returned data */
    MYSQL_RES *result;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    int num_rows = mysql_num_rows(result);

    MYSQL_ROW row;
    printf("<id>\t<member>\n");
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        printf("%s\t%s\n", row[0], row[1]);
    }

    /* Close connection & Write back result */
    mysql_free_result(result);
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
