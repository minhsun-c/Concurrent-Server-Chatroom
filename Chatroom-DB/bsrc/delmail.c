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

void checkExist(MYSQL *con, char *username, char *id)
{
    char sql[100];
    sprintf(sql, "SELECT * FROM %s_mailbox WHERE mail_id='%s'", username, id);
    if (mysql_query(con, sql))
        finish(con);

    /* Get returned data */
    MYSQL_RES *result;
    if ((result = mysql_store_result(con)) == NULL)
        finish(con);

    int num_rows = mysql_num_rows(result);

    if (num_rows == 0)
    {
        printf("Mailid not found!\n");
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
    char sender[30];
    getSender(sender);

    checkExist(con, sender, argv[1]);
    sprintf(sql,
            "DELETE FROM %s_mailbox WHERE mail_id='%s'",
            sender, argv[1]);
    fprintf(stderr, "%s\n", sql);
    if (mysql_query(con, sql))
        finish(con);

    mysql_close(con);
    exit(EXIT_SUCCESS);
}