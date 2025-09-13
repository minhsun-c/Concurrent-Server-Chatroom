#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CREATE_TABLE_TEMPLATE                 \
    "CREATE TABLE %s_mailbox ("               \
    "mail_id INT AUTO_INCREMENT PRIMARY KEY," \
    "mail_from VARCHAR(50)  NULL,"            \
    "message VARCHAR(200)  NULL,"             \
    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);"

#define EXIT_REGISTER_SUCCESS 0
#define EXIT_REGISTER_FAILURE 1

void finish(MYSQL *con, int exitcode)
{
    printf("%d\n", exitcode);
    mysql_close(con);
    exit(exitcode);
}

static MYSQL *connect_mysql_server()
{
    MYSQL *con = mysql_init(NULL);
    if (con == NULL)
        finish(con, EXIT_REGISTER_FAILURE);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");
    if (mysql_real_connect(con, "localhost", "root", "password", "NPHW3", 0, NULL, 0) == NULL)
        finish(con, EXIT_REGISTER_FAILURE);
    return con;
}

static MYSQL_RES *get_all_user(MYSQL *con, char **argv)
{
    char sql[100];

    sprintf(sql, "SELECT * FROM user WHERE username='%s'", argv[1]);
    if (mysql_query(con, sql))
        finish(con, EXIT_REGISTER_FAILURE);

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
        finish(con, EXIT_REGISTER_FAILURE);
    return result;
}

static void add_new_user(MYSQL *con, char **argv)
{
    char sql[100];
    sprintf(sql,
            "INSERT INTO user (username, password) VALUES ('%s', '%s')",
            argv[1], argv[2]);
    if (mysql_query(con, sql))
        finish(con, EXIT_REGISTER_FAILURE);
}

static void create_mysql_user_mailbox(MYSQL *con, char *username)
{
    char sql[300];
    sprintf(sql, CREATE_TABLE_TEMPLATE, username);

    if (mysql_query(con, sql))
    {
        fprintf(stderr, "CREATE TABLE failed. Error: %s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
}

static void write_file(int exitcode)
{
    char file[50], data[5];
    memset(file, 0, 50);
    memset(data, 0, 5);
    sprintf(file, "tmp/register%d.txt", getppid());
    sprintf(data, "%d\n", exitcode);
    FILE *fp = fopen(file, "w");
    fwrite(data, sizeof(char), sizeof(data), fp);
    fclose(fp);
}

int main(int argc, char **argv)
{
    int res;
    MYSQL *con = connect_mysql_server();         /* connect to mysql server */
    MYSQL_RES *result = get_all_user(con, argv); /* fetch all user data */

    if (mysql_num_rows(result) > 0) /* user exist */
    {
        printf("Register Failed: user existed\n");
        res = EXIT_REGISTER_FAILURE;
    }
    else
    {
        add_new_user(con, argv);
        create_mysql_user_mailbox(con, argv[1]);

        res = EXIT_REGISTER_SUCCESS;
    }
    mysql_free_result(result);
    write_file(res);
    mysql_close(con);
    exit(res);
}