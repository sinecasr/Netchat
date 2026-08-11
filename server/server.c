#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../common/protocol.h"
#include "client_manager.h"
#include "logger.h"
#define PORT 8080
#define BUFFER_SIZE 1200

int server_fd;
volatile sig_atomic_t server_running = 1;




int is_valid_username(const char *username)
{
    int length = strlen(username);

    // Check length
    if (length < 3 || length > 40)
    {
        return 0;
    }

    // Check every character
    for (int i = 0; i < length; i++)
    {
        char c = username[i];

        if (!(
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_'
        ))
        {
            return 0;
        }
    }

    return 1;
}


// ======================================================
// SYSTEM MESSAGE
// ======================================================

void broadcast_system_message(
    const char *message,
    const char *exclude_username
)
{
    char output[BUFFER_SIZE];

    snprintf(
        output,
        sizeof(output),
        "[System] %s",
        message
    );

    pthread_mutex_lock(
        &clients_mutex
    );

    for (int i = 0; i < client_count; i++)
    {
        if (
            exclude_username == NULL ||
            strcmp(
                clients[i].username,
                exclude_username
            ) != 0
        )
        {
            send(
                clients[i].socket,
                output,
                strlen(output),
                0
            );
        }
    }

    pthread_mutex_unlock(
        &clients_mutex
    );
}

// ======================================================
// PUBLIC MESSAGE
// Send to everyone except sender
// ======================================================

void broadcast_message(
    const char *sender,
    const char *message
)
{
    char output[BUFFER_SIZE];

    snprintf(
    output,
    sizeof(output),
    "%s: %s",
    sender,
    message
);

    // Server terminal
    printf(
        "%s\n",
        output
    );

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        if (
            strcmp(
                clients[i].username,
                sender
            ) != 0
        )
        {
            send(
                clients[i].socket,
                output,
                strlen(output),
                0
            );
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}


// ======================================================
// PRIVATE MESSAGE
// Send ONLY to receiver
// ======================================================

void send_private_message(
    const char *sender,
    const char *receiver,
    const char *message
)
{
    char output[BUFFER_SIZE];

  snprintf(
    output,
    sizeof(output),
    "[Private] %s -> %s: %s",
    sender,
    receiver,
    message
);



    pthread_mutex_lock(&clients_mutex);

    int index =
        find_client_by_username(receiver);

    if (index != -1)
    {
        send(
            clients[index].socket,
            output,
            strlen(output),
            0
        );
    }
    else
{
    char error[BUFFER_SIZE];

    snprintf(
        error,
        sizeof(error),
        "User '%s' is not online.",
        receiver
    );

    printf(
        "%s\n",
        error
    );

    // Send error back to the sender
    int sender_index =
        find_client_by_username(sender);

    if (sender_index != -1)
    {
        send(
            clients[sender_index].socket,
            error,
            strlen(error),
            0
        );
    }
}
    pthread_mutex_unlock(&clients_mutex);
}


// ======================================================
// ONLINE USERS
// ======================================================

void send_users(int socket)
{
    char output[BUFFER_SIZE];

    strcpy(
        output,
        "Online users:\n"
    );

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++)
    {
        strcat(
            output,
            "- "
        );

        strcat(
            output,
            clients[i].username
        );

        strcat(
            output,
            "\n"
        );
    }

    pthread_mutex_unlock(&clients_mutex);

    send(
        socket,
        output,
        strlen(output),
        0
    );
}


// ======================================================
// CLIENT THREAD
// ======================================================

void *handle_client(void *arg)
{
    int client_socket =
        *(int *)arg;

    free(arg);

    char buffer[BUFFER_SIZE];

    // ==================================================
    // RECEIVE LOGIN
    // ==================================================

    memset(
        buffer,
        0,
        sizeof(buffer)
    );

    int bytes_received =
        recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

    if (bytes_received <= 0)
    {
        close(client_socket);
        return NULL;
    }

    buffer[bytes_received] = '\0';


    // ==================================================
    // PARSE LOGIN
    // ==================================================

    struct Message login_message;

    if (
        parse_message(
            buffer,
            &login_message
        ) != 0
    )
    {
        close(client_socket);
        return NULL;
    }

    if (
        login_message.type != MSG_LOGIN
    )
    {
        close(client_socket);
        return NULL;
    }
// ==================================================
// VALIDATE USERNAME
// ==================================================

if (!is_valid_username(login_message.sender))
{
    char error[] =
        "Invalid username. Use 3-40 characters: letters, numbers, and underscore only.";

    send(
        client_socket,
        error,
        strlen(error),
        0
    );

    close(client_socket);

    return NULL;
}


    // ==================================================
    // ADD USER
    // ==================================================

    if (
        add_client(
            client_socket,
            login_message.sender
        ) != 0
    )
    {
        char error[] =
            "Username already exists or server is full.";

        send(
            client_socket,
            error,
            strlen(error),
            0
        );

        close(client_socket);

        return NULL;
    }


    char connect_message[BUFFER_SIZE];

snprintf(
    connect_message,
    sizeof(connect_message),
    "%s connected",
    login_message.sender
);

log_message(
    "CONNECT",
    connect_message
);


    // ==================================================
    // WELCOME
    // ==================================================

    char welcome[] =
        "Welcome to NetChat!";

    send(
        client_socket,
        welcome,
        strlen(welcome),
        0
    );


    // ==================================================
    // NOTIFY USERS
    // ==================================================

    char connected_message[BUFFER_SIZE];

    snprintf(
        connected_message,
        sizeof(connected_message),
        "%s joined the chat.",
        login_message.sender
    );

    broadcast_system_message(
        connected_message,
        login_message.sender
    );


    // ==================================================
    // CHAT LOOP
    // ==================================================

    while (1)
    {
        memset(
            buffer,
            0,
            sizeof(buffer)
        );

        bytes_received =
            recv(
                client_socket,
                buffer,
                sizeof(buffer) - 1,
                0
            );
            
// ==================================================
// MESSAGE LENGTH CHECK
// ==================================================

if (bytes_received >= BUFFER_SIZE - 1)
{
    char error[] =
        "Message too long. Maximum message size exceeded.";

    send(
        client_socket,
        error,
        strlen(error),
        0
    );

    continue;
}
        buffer[bytes_received] = '\0';


        // ==================================================
        // CLIENT DISCONNECTED
        // ==================================================

     if (bytes_received <= 0)
{
    char disconnected_message[BUFFER_SIZE];

    snprintf(
        disconnected_message,
        sizeof(disconnected_message),
        "%s disconnected.",
        login_message.sender
    );

    int index;

    pthread_mutex_lock(
        &clients_mutex
    );

    index =
        find_client_by_socket(
            client_socket
        );

    if (index != -1)
    {
        remove_client(index);
    }

    pthread_mutex_unlock(
        &clients_mutex
    );

    close(client_socket);

    log_message(
        "DISCONNECT",
        disconnected_message
    );

    broadcast_system_message(
        disconnected_message,
        NULL
    );

    return NULL;
}

        buffer[bytes_received] = '\0';


        // ==================================================
        // PARSE MESSAGE
        // ==================================================

        struct Message message;

        if (
            parse_message(
                buffer,
                &message
            ) != 0
        )
        {
            char error[] =
                "Invalid message format.";

            send(
                client_socket,
                error,
                strlen(error),
                0
            );

            continue;
        }


        // ==================================================
        // PUBLIC MESSAGE
        // ==================================================

        if (
            message.type == MSG_PUBLIC
        )
        {
           char public_log[BUFFER_SIZE];

snprintf(
    public_log,
    sizeof(public_log),
    "%s: %s",
    message.sender,
    message.text
);

log_message(
    "PUBLIC",
    public_log
);

            broadcast_message(
                message.sender,
                message.text
            );
        }


        // ==================================================
        // PRIVATE MESSAGE
        // ==================================================

        else if (
            message.type == MSG_PRIVATE
        )
        {
            char private_log[BUFFER_SIZE];

snprintf(
    private_log,
    sizeof(private_log),
    "%s -> %s: %s",
    message.sender,
    message.receiver,
    message.text
);

log_message(
    "PRIVATE",
    private_log
);

            send_private_message(
                message.sender,
                message.receiver,
                message.text
            );
        }


        // ==================================================
        // USERS
        // ==================================================

        else if (
            message.type == MSG_USERS
        )
        {
            send_users(
                client_socket
            );
        }


        // ==================================================
        // QUIT
        // ==================================================

 else if (
    message.type == MSG_QUIT
)
{
    char quit_message[BUFFER_SIZE];

    snprintf(
        quit_message,
        sizeof(quit_message),
        "%s left the chat.",
        login_message.sender
    );

    int index;

    pthread_mutex_lock(
        &clients_mutex
    );

    index =
        find_client_by_socket(
            client_socket
        );

    if (index != -1)
    {
        remove_client(index);
    }

    pthread_mutex_unlock(
        &clients_mutex
    );

    close(client_socket);

    log_message(
        "DISCONNECT",
        quit_message
    );

    broadcast_system_message(
        quit_message,
        NULL
    );

    return NULL;
}

        // ==================================================
        // LOGOUT
        // ==================================================

        else if (
            message.type == MSG_LOGOUT
        )
        {
            char logout_message[BUFFER_SIZE];

            snprintf(
                logout_message,
                sizeof(logout_message),
                "%s logged out.",
                login_message.sender
            );
            log_message(
    "DISCONNECT",
    logout_message
);

            int index;

            pthread_mutex_lock(
                &clients_mutex
            );

            index =
                find_client_by_socket(
                    client_socket
                );

            if (index != -1)
            {
                remove_client(index);
            }

            pthread_mutex_unlock(
                &clients_mutex
            );

            broadcast_system_message(
                logout_message,
                NULL
            );

            close(client_socket);

            return NULL;
        }


        // ==================================================
        // UNKNOWN
        // ==================================================

        else
        {
            char error[] =
                "Unknown command.";

            send(
                client_socket,
                error,
                strlen(error),
                0
            );
        }
    }

    return NULL;
}
void handle_sigint(int signal)
{
    server_running = 0;

    close(server_fd);

    log_message(
        "INFO",
        "Server shutting down"
    );
}

// ======================================================
// MAIN
// ======================================================

int main()
{
    signal(SIGINT, handle_sigint);


    // ==================================================
    // CREATE SOCKET
    // ==================================================

    server_fd =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if (server_fd < 0)
    {
        perror(
            "Socket creation failed"
        );

        return 1;
    }

    log_message(
    "INFO",
    "Socket created successfully"
);


    // ==================================================
    // REUSE PORT
    // ==================================================

    int option = 1;

    setsockopt(
        server_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option)
    );


    // ==================================================
    // BIND
    // ==================================================

    struct sockaddr_in server_addr;

    memset(
        &server_addr,
        0,
        sizeof(server_addr)
    );

    server_addr.sin_family =
        AF_INET;

    server_addr.sin_addr.s_addr =
        INADDR_ANY;

    server_addr.sin_port =
        htons(PORT);


    if (
        bind(
            server_fd,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)
        ) < 0
    )
    {
        perror(
            "Bind failed"
        );

        close(server_fd);

        return 1;
    }

   log_message(
    "INFO",
    "Bind successful"
);


    // ==================================================
    // LISTEN
    // ==================================================

    if (
        listen(
            server_fd,
            10
        ) < 0
    )
    {
        perror(
            "Listen failed"
        );

        close(server_fd);

        return 1;
    }

char server_message[BUFFER_SIZE];

snprintf(
    server_message,
    sizeof(server_message),
    "Server started on port %d",
    PORT
);

log_message(
    "INFO",
    server_message
);


    // ==================================================
    // ACCEPT CLIENTS
    // ==================================================

   while (server_running)
    {
        struct sockaddr_in client_addr;

        socklen_t client_len =
            sizeof(client_addr);


        int *client_socket =
            malloc(sizeof(int));

        if (
            client_socket == NULL
        )
        {
            perror(
                "Memory allocation failed"
            );

            continue;
        }


        *client_socket =
            accept(
                server_fd,
                (struct sockaddr *)&client_addr,
                &client_len
            );


if (*client_socket < 0)
{
    free(client_socket);

    if (!server_running)
    {
        break;
    }

    perror("Accept failed");
    continue;
}


  log_message(
    "CONNECT",
    "TCP client connected"
);

        // ==================================================
        // CREATE THREAD
        // ==================================================

        pthread_t thread;

        if (
            pthread_create(
                &thread,
                NULL,
                handle_client,
                client_socket
            ) != 0
        )
        {
            perror(
                "Thread creation failed"
            );

            close(
                *client_socket
            );

            free(
                client_socket
            );

            continue;
        }


        pthread_detach(
            thread
        );
    }


    close(
        server_fd
    );

    return 0;
}