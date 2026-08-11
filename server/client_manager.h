#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <pthread.h>
#include "../common/protocol.h"

#define MAX_CLIENTS 100

struct Client
{
    int socket;
    char username[MAX_USERNAME];
};

extern struct Client clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t clients_mutex;

int find_client_by_username(const char *username);

int find_client_by_socket(int socket);

int add_client(
    int socket,
    const char *username
);

void remove_client(int index);

#endif