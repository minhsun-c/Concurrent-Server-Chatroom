#include "includes.h"

clinode_t *clientList = NULL;
extern pid_t serv_pid;

static void write_client_info(FILE *fp, int usrid, clinode_t *client)
{
    /* user id */
    char uid[4];
    sprintf(uid, "%d ", usrid);
    fwrite(uid, sizeof(char), strlen(uid), fp);

    /* process name */
    fwrite("my_name ", sizeof(char), strlen("my_name "), fp);

    /* ip & port */
    char ip_port[50];
    sprintf(ip_port, "%s %d ", inet_ntoa(client->ip), client->port);
    fwrite(ip_port, sizeof(char), strlen(ip_port), fp);

    /* process id */
    char pid[32];
    sprintf(pid, "%d ", client->pid);
    fwrite(pid, sizeof(char), strlen(pid), fp);

    fwrite("\n", sizeof(char), 1, fp);
}

static void update_client_list()
{
    if (getpid() != serv_pid)
        return;
    int usrid = 0;
    clinode_t *ptr = clientList;
    FILE *fp = fopen("tmp/usrList.txt", "w");
    if (fp == NULL)
        return;
    while (ptr)
    {
        write_client_info(fp, usrid++, ptr);
        ptr = ptr->next;
    }
    fclose(fp);
}

clinode_t *create_clinode(int pid, struct sockaddr_in *cliaddr)
{
    clinode_t *node = (clinode_t *)malloc(sizeof(clinode_t));
    ERR_MSG_MEM(node);
    node->pid = pid;
    node->port = cliaddr->sin_port;
    node->ip = cliaddr->sin_addr;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

int remove_clinode(clinode_t *node)
{
    if (clientList == NULL)
    {
        fprintf(stderr, "clientList is empty\n\r");
        return -1;
    }
    else if (node->prev == NULL && node->next == NULL) /* the only node of list */
    {
        clientList = NULL;
        free(node);
    }
    else if (node == clientList) /* first node of list */
    {
        clientList = clientList->next;
        clientList->prev = NULL;
        free(node);
    }
    else
    {
        node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
        free(node);
    }
    update_client_list();
    return 1;
}

int insert_clinode(clinode_t *node)
{
    if (clientList == NULL)
    {
        clientList = node;
    }
    else
    {
        node->next = clientList;
        clientList->prev = node;
        clientList = node;
    }
    update_client_list();
    return 1;
}

clinode_t *search_clinode_pid(int pid)
{
    clinode_t *ptr = clientList;
    while (ptr != NULL)
    {
        if (ptr->pid == pid)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

void clear_clinode()
{
    while (clientList)
        remove_clinode(clientList);
}

pid_t match_pid(int uid)
{
    clinode_t *ptr = clientList;
    for (int i = 0; i < uid - 1; i++)
    {
        if (ptr == NULL)
            return -1;
        ptr = ptr->next;
    }
    return ptr->pid;
}