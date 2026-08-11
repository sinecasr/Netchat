#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../common/protocol.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1200

// ======================================================
// GLOBAL CLIENT SOCKET
// ======================================================

int client_socket;


// ======================================================
// RECEIVE THREAD
// ======================================================

void *receive_messages(void *arg)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
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
            printf("\nServer disconnected.\n");
            break;
        }

        buffer[bytes_received] = '\0';

        printf("\n%s\n", buffer);

        printf("You: ");
        fflush(stdout);
    }

    return NULL;
}


// ======================================================
// MAIN
// ======================================================

int main()
{
    // ==================================================
    // STEP 1: CREATE SOCKET
    // ==================================================

    client_socket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if (client_socket < 0)
    {
        perror(
            "Socket creation failed"
        );

        return 1;
    }

    printf(
        "Client socket created successfully!\n"
    );


    // ==================================================
    // STEP 2: SERVER ADDRESS
    // ==================================================

    struct sockaddr_in server_addr;

    memset(
        &server_addr,
        0,
        sizeof(server_addr)
    );

    server_addr.sin_family =
        AF_INET;

    server_addr.sin_port =
        htons(PORT);


    if (
        inet_pton(
            AF_INET,
            SERVER_IP,
            &server_addr.sin_addr
        ) <= 0
    )
    {
        perror(
            "Invalid server address"
        );

        close(client_socket);

        return 1;
    }


    // ==================================================
    // STEP 3: CONNECT TO SERVER
    // ==================================================

    if (
        connect(
            client_socket,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)
        ) < 0
    )
    {
        perror(
            "Connection failed"
        );

        close(client_socket);

        return 1;
    }

    printf(
        "Connected to NetChat server!\n"
    );


    // ==================================================
    // STEP 4: LOGIN
    // ==================================================

    char username[MAX_USERNAME];

    printf(
        "Enter your username: "
    );

    if (
        fgets(
            username,
            sizeof(username),
            stdin
        ) == NULL
    )
    {
        close(client_socket);

        return 1;
    }


    // Remove newline

    username[
        strcspn(username, "\n")
    ] = '\0';


    if (strlen(username) == 0)
    {
        printf(
            "Username cannot be empty.\n"
        );

        close(client_socket);

        return 1;
    }


    // ==================================================
    // CREATE LOGIN MESSAGE
    // ==================================================

    char buffer[BUFFER_SIZE];

    if (
        create_login_message(
            buffer,
            sizeof(buffer),
            username
        ) != 0
    )
    {
        printf(
            "Failed to create login message.\n"
        );

        close(client_socket);

        return 1;
    }


    // ==================================================
    // SEND LOGIN
    // ==================================================

    if (
        send(
            client_socket,
            buffer,
            strlen(buffer),
            0
        ) < 0
    )
    {
        perror(
            "Login failed"
        );

        close(client_socket);

        return 1;
    }


    printf(
        "\nLogged in as: %s\n",
        username
    );


    // ==================================================
    // STEP 5: CREATE RECEIVE THREAD
    // ==================================================

    pthread_t receive_thread;

    if (
        pthread_create(
            &receive_thread,
            NULL,
            receive_messages,
            NULL
        ) != 0
    )
    {
        perror(
            "Thread creation failed"
        );

        close(client_socket);

        return 1;
    }


    // ==================================================
    // STEP 6: COMMAND / MESSAGE LOOP
    // ==================================================

    char input[BUFFER_SIZE];

    while (1)
    {
        printf(
            "You: "
        );

        fflush(stdout);


        if (
            fgets(
                input,
                sizeof(input),
                stdin
            ) == NULL
        )
        {
            break;
        }


        // Remove newline

        input[
            strcspn(input, "\n")
        ] = '\0';


        // Ignore empty input

        if (strlen(input) == 0)
        {
            continue;
        }


        // ==================================================
        // /quit
        // ==================================================

        if (
            strcmp(
                input,
                "/quit"
            ) == 0
        )
        {
            if (
                create_quit_message(
                    buffer,
                    sizeof(buffer)
                ) == 0
            )
            {
                send(
                    client_socket,
                    buffer,
                    strlen(buffer),
                    0
                );
            }

            break;
        }


        // ==================================================
        // /help
        // ==================================================

        if (
            strcmp(
                input,
                "/help"
            ) == 0
        )
        {
            printf("\n");
            printf("Available commands:\n");
            printf("/help                       Show available commands\n");
            printf("/whoami                     Show your username\n");
            printf("/users                      Show online users\n");
            printf("/msg <username> <message>   Send a private message\n");
            printf("/quit                       Exit NetChat\n");
            printf("\n");

            continue;
        }


        // ==================================================
        // /whoami
        // ==================================================

        if (
            strcmp(
                input,
                "/whoami"
            ) == 0
        )
        {
            printf(
                "You are logged in as: %s\n",
                username
            );

            continue;
        }


        // ==================================================
        // /users
        // ==================================================

        if (
            strcmp(
                input,
                "/users"
            ) == 0
        )
        {
            if (
                create_users_message(
                    buffer,
                    sizeof(buffer)
                ) == 0
            )
            {
                if (
                    send(
                        client_socket,
                        buffer,
                        strlen(buffer),
                        0
                    ) < 0
                )
                {
                    perror(
                        "Send failed"
                    );

                    break;
                }
            }

            continue;
        }


        // ==================================================
        // /msg WITHOUT ARGUMENTS
        // ==================================================

        if (
            strcmp(
                input,
                "/msg"
            ) == 0
        )
        {
            printf(
                "Usage: /msg <username> <message>\n"
            );

            continue;
        }


        // ==================================================
        // /msg USER MESSAGE
        // ==================================================

        if (
            strncmp(
                input,
                "/msg ",
                5
            ) == 0
        )
        {
            char temp[BUFFER_SIZE];

            strncpy(
                temp,
                input + 5,
                sizeof(temp) - 1
            );

            temp[
                sizeof(temp) - 1
            ] = '\0';


            // Find receiver

            char *receiver =
                strtok(
                    temp,
                    " "
                );


            // Find message

            char *message =
                strtok(
                    NULL,
                    ""
                );


            // Validate /msg format

            if (
                receiver == NULL ||
                message == NULL ||
                strlen(receiver) == 0 ||
                strlen(message) == 0
            )
            {
                printf(
                    "Usage: /msg <username> <message>\n"
                );

                continue;
            }


            // ==================================================
            // CREATE PRIVATE MESSAGE
            // ==================================================

            if (
                create_private_message(
                    buffer,
                    sizeof(buffer),
                    username,
                    receiver,
                    message
                ) != 0
            )
            {
                printf(
                    "Failed to create private message.\n"
                );

                continue;
            }


            // ==================================================
            // SEND PRIVATE MESSAGE
            // ==================================================

            if (
                send(
                    client_socket,
                    buffer,
                    strlen(buffer),
                    0
                ) < 0
            )
            {
                perror(
                    "Send failed"
                );

                break;
            }

            continue;
        }


        // ==================================================
        // UNKNOWN COMMAND
        // ==================================================

        if (
            input[0] == '/'
        )
        {
            printf(
                "Unknown command. Type /help to see available commands.\n"
            );

            continue;
        }


        // ==================================================
        // PUBLIC MESSAGE
        // ==================================================

        if (
            create_public_message(
                buffer,
                sizeof(buffer),
                username,
                input
            ) != 0
        )
        {
            printf(
                "Failed to create public message.\n"
            );

            continue;
        }


        // ==================================================
        // SEND PUBLIC MESSAGE
        // ==================================================

        if (
            send(
                client_socket,
                buffer,
                strlen(buffer),
                0
            ) < 0
        )
        {
            perror(
                "Send failed"
            );

            break;
        }
    }


    // ==================================================
    // CLEANUP
    // ==================================================

    close(
        client_socket
    );

    return 0;
}

