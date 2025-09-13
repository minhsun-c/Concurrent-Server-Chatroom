#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INSERT_GROUP_TEMPLATE \
    "INSERT INTO groupList (group_name, owner) VALUES ('%s', '%s')"
#define CREATE_GROUP_TABLE               \
    "CREATE TABLE %s_gdetail ("          \
    "id INT AUTO_INCREMENT PRIMARY KEY," \
    "member VARCHAR(50)  NULL);"
#define INSERT_MEMBER_TEMPLATE \
    "INSERT INTO %s_gdetail (member) VALUES ('%s')"

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

    MYSQL_ROW row;
    while (num_rows--)
    {
        row = mysql_fetch_row(result);
        if (strcmp(row[1], argv[1]) == 0)
        {
            printf("Group already exist!\n");
            mysql_free_result(result);
            finish(con);
        }
    }
    char filename[30];
    char sender[30];
    sprintf(filename, "tmp/usr%d.txt", getppid());
    FILE *fp = fopen(filename, "r");
    fscanf(fp, "%s", sender);
    fclose(fp);
    sprintf(sql, INSERT_GROUP_TEMPLATE, argv[1], sender);
    if (mysql_query(con, sql))
        finish(con);

    sprintf(sql, CREATE_GROUP_TABLE, argv[1]);
    if (mysql_query(con, sql))
        finish(con);

    sprintf(sql, INSERT_MEMBER_TEMPLATE, argv[1], sender);
    if (mysql_query(con, sql))
        finish(con);

    printf("Create Group Success!\n");
    /* Close connection & Write back result */
    mysql_free_result(result);
    mysql_close(con);
    exit(EXIT_SUCCESS);
}
