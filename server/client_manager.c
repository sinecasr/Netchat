#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "client_manager.h"

struct Client clients[MAX_CLIENTS];

int client_count = 0;

pthread_mutex_t clients_mutex =
    PTHREAD_MUTEX_INITIALIZER;


// ======================================================
// FIND CLIENT BY USERNAME
// ======================================================

int find_client_by_username(
    const char *username
)
{
    for (int i = 0; i < client_count; i++)
    {
        if (
            strcmp(
                clients[i].username,
                username
            ) == 0
        )
        {
            return i;
        }
    }

    return -1;
}


// ======================================================
// FIND CLIENT BY SOCKET
// ======================================================

int find_client_by_socket(
    int socket
)
{
    for (int i = 0; i < client_count; i++)
    {
        if (
            clients[i].socket == socket
        )
        {
            return i;
        }
    }

    return -1;
}


// ======================================================
// ADD CLIENT
// ======================================================

int add_client(
    int socket,
    const char *username
)
{
    pthread_mutex_lock(
        &clients_mutex
    );

    if (
        client_count >= MAX_CLIENTS
    )
    {
        pthread_mutex_unlock(
            &clients_mutex
        );

        return -1;
    }

    if (
        find_client_by_username(
            username
        ) != -1
    )
    {
        pthread_mutex_unlock(
            &clients_mutex
        );

        return -1;
    }

    clients[client_count].socket =
        socket;

    strncpy(
        clients[client_count].username,
        username,
        MAX_USERNAME - 1
    );

    clients[client_count]
        .username[MAX_USERNAME - 1] = '\0';

    client_count++;

    pthread_mutex_unlock(
        &clients_mutex
    );

    return 0;
}


// ======================================================
// REMOVE CLIENT
// ======================================================

void remove_client(
    int index
)
{
    if (
        index < 0 ||
        index >= client_count
    )
    {
        return;
    }

    char username[MAX_USERNAME];

    strcpy(
        username,
        clients[index].username
    );

    for (
        int i = index;
        i < client_count - 1;
        i++
    )
    {
        clients[i] =
            clients[i + 1];
    }

    client_count--;

    printf(
        "User removed: %s\n",
        username
    );
}