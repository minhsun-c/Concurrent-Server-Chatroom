#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXIT_LOGIN_SUCCESS 0
#define EXIT_LOGIN_FAILURE 1
#define EXIT_LOGIN_PASSWORD_ERROR 2
#define EXIT_LOGIN_NAME_ERROR 3

void finish(MYSQL *con, int exitcode)
{
    printf("%d\n", exitcode);
    mysql_close(con);
    exit(exitcode);
}

static void write_file(int exitcode)
{
    char file[50], data[5];
    memset(file, 0, 50);
    memset(data, 0, 5);
    sprintf(file, "tmp/login%d.txt", getppid());
    sprintf(data, "%d\n", exitcode);
    FILE *fp = fopen(file, "w");
    fwrite(data, sizeof(char), sizeof(data), fp);
    fclose(fp);
}

int main(int argc, char **argv)
{
    int res;

    /* Initialize mysql connection */
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL)
        finish(con, EXIT_LOGIN_FAILURE);

    if (mysql_real_connect(con, "localhost", "root", "password", "NPHW3", 0, NULL, 0) == NULL)
        finish(con, EXIT_LOGIN_FAILURE);

    /* Fetch data from database */
    char sql[100];
    sprintf(sql, "SELECT * FROM user WHERE username='%s'", argv[1]);
    if (mysql_query(con, sql))
        finish(con, EXIT_LOGIN_FAILURE);

    /* Get returned data */
    MYSQL_RES *result;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con, EXIT_LOGIN_FAILURE);

    int num_rows = mysql_num_rows(result);

    MYSQL_ROW row;
    if (num_rows > 0)
    {
        row = mysql_fetch_row(result);
        if (strcmp(row[2], argv[2]) == 0)
            res = EXIT_LOGIN_SUCCESS;
        else
            res = EXIT_LOGIN_PASSWORD_ERROR;
    }
    else
        res = EXIT_LOGIN_NAME_ERROR;

    /* Close connection & Write back result */
    mysql_free_result(result);
    write_file(res);
    mysql_close(con);
    exit(res);
}