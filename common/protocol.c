#include <stdio.h>
#include <string.h>

#include "protocol.h"


// ======================================================
// CREATE PUBLIC MESSAGE
// Format:
// PUBLIC|Bob|Hello everyone!
// ======================================================

int create_public_message(
    char *buffer,
    size_t buffer_size,
    const char *sender,
    const char *text
)
{
    if (buffer == NULL ||
        sender == NULL ||
        text == NULL)
    {
        return -1;
    }

    int result = snprintf(
        buffer,
        buffer_size,
        "PUBLIC|%s|%s",
        sender,
        text
    );

    if (result < 0 ||
        (size_t)result >= buffer_size)
    {
        return -1;
    }

    return 0;
}


// ======================================================
// CREATE PRIVATE MESSAGE
// Format:
// PRIVATE|Bob|Alice|Hello Alice!
// ======================================================

int create_private_message(
    char *buffer,
    size_t buffer_size,
    const char *sender,
    const char *receiver,
    const char *text
)
{
    if (buffer == NULL ||
        sender == NULL ||
        receiver == NULL ||
        text == NULL)
    {
        return -1;
    }

    int result = snprintf(
        buffer,
        buffer_size,
        "PRIVATE|%s|%s|%s",
        sender,
        receiver,
        text
    );

    if (result < 0 ||
        (size_t)result >= buffer_size)
    {
        return -1;
    }

    return 0;
}


// ======================================================
// CREATE LOGIN MESSAGE
// Format:
// LOGIN|Bob
// ======================================================

int create_login_message(
    char *buffer,
    size_t buffer_size,
    const char *username
)
{
    if (buffer == NULL ||
        username == NULL)
    {
        return -1;
    }

    int result = snprintf(
        buffer,
        buffer_size,
        "LOGIN|%s",
        username
    );

    if (result < 0 ||
        (size_t)result >= buffer_size)
    {
        return -1;
    }

    return 0;
}


// ======================================================
// CREATE LOGOUT MESSAGE
// Format:
// LOGOUT|Bob
// ======================================================

int create_logout_message(
    char *buffer,
    size_t buffer_size,
    const char *username
)
{
    if (buffer == NULL ||
        username == NULL)
    {
        return -1;
    }

    int result = snprintf(
        buffer,
        buffer_size,
        "LOGOUT|%s",
        username
    );

    if (result < 0 ||
        (size_t)result >= buffer_size)
    {
        return -1;
    }

    return 0;
}


// ======================================================
// CREATE USERS MESSAGE
// Format:
// USERS
// ======================================================

int create_users_message(
    char *buffer,
    size_t buffer_size
)
{
    if (buffer == NULL)
    {
        return -1;
    }

    int result = snprintf(
        buffer,
        buffer_size,
        "USERS"
    );

    if (result < 0 ||
        (size_t)result >= buffer_size)
    {
        return -1;
    }

    return 0;
}


// ======================================================
// CREATE QUIT MESSAGE
// Format:
// QUIT
// ======================================================

int create_quit_message(
    char *buffer,
    size_t buffer_size
)
{
    if (buffer == NULL)
    {
        return -1;
    }

    int result = snprintf(
        buffer,
        buffer_size,
        "QUIT"
    );

    if (result < 0 ||
        (size_t)result >= buffer_size)
    {
        return -1;
    }

    return 0;
}


// ======================================================
// PARSE MESSAGE
// ======================================================

int parse_message(
    const char *buffer,
    struct Message *message
)
{
    if (buffer == NULL ||
        message == NULL)
    {
        return -1;
    }

    // Clear the structure
    memset(
        message,
        0,
        sizeof(struct Message)
    );


    // ==================================================
    // PUBLIC
    // PUBLIC|Bob|Hello everyone!
    // ==================================================

    if (strncmp(buffer, "PUBLIC|", 7) == 0)
    {
        char temp[MAX_PROTOCOL_MESSAGE];

        strncpy(
            temp,
            buffer + 7,
            sizeof(temp) - 1
        );

        temp[sizeof(temp) - 1] = '\0';


        char *sender = strtok(
            temp,
            "|"
        );

        char *text = strtok(
            NULL,
            ""
        );


        if (sender == NULL ||
            text == NULL)
        {
            return -1;
        }


        message->type = MSG_PUBLIC;


        strncpy(
            message->sender,
            sender,
            MAX_USERNAME - 1
        );


        strncpy(
            message->text,
            text,
            MAX_MESSAGE - 1
        );


        return 0;
    }


    // ==================================================
    // PRIVATE
    // PRIVATE|Bob|Alice|Hello Alice!
    // ==================================================

    if (strncmp(buffer, "PRIVATE|", 8) == 0)
    {
        char temp[MAX_PROTOCOL_MESSAGE];

        strncpy(
            temp,
            buffer + 8,
            sizeof(temp) - 1
        );

        temp[sizeof(temp) - 1] = '\0';


        char *sender = strtok(
            temp,
            "|"
        );

        char *receiver = strtok(
            NULL,
            "|"
        );

        char *text = strtok(
            NULL,
            ""
        );


        if (sender == NULL ||
            receiver == NULL ||
            text == NULL)
        {
            return -1;
        }


        message->type = MSG_PRIVATE;


        strncpy(
            message->sender,
            sender,
            MAX_USERNAME - 1
        );


        strncpy(
            message->receiver,
            receiver,
            MAX_USERNAME - 1
        );


        strncpy(
            message->text,
            text,
            MAX_MESSAGE - 1
        );


        return 0;
    }


    // ==================================================
    // LOGIN
    // LOGIN|Bob
    // ==================================================

    if (strncmp(buffer, "LOGIN|", 6) == 0)
    {
        char username[MAX_USERNAME];


        strncpy(
            username,
            buffer + 6,
            MAX_USERNAME - 1
        );

        username[MAX_USERNAME - 1] = '\0';


        message->type = MSG_LOGIN;


        strncpy(
            message->sender,
            username,
            MAX_USERNAME - 1
        );


        return 0;
    }


    // ==================================================
    // LOGOUT
    // LOGOUT|Bob
    // ==================================================

    if (strncmp(buffer, "LOGOUT|", 7) == 0)
    {
        char username[MAX_USERNAME];


        strncpy(
            username,
            buffer + 7,
            MAX_USERNAME - 1
        );

        username[MAX_USERNAME - 1] = '\0';


        message->type = MSG_LOGOUT;


        strncpy(
            message->sender,
            username,
            MAX_USERNAME - 1
        );


        return 0;
    }


    // ==================================================
    // USERS
    // ==================================================

    if (strcmp(buffer, "USERS") == 0)
    {
        message->type = MSG_USERS;

        return 0;
    }


    // ==================================================
    // QUIT
    // ==================================================

    if (strcmp(buffer, "QUIT") == 0)
    {
        message->type = MSG_QUIT;

        return 0;
    }


    // ==================================================
    // UNKNOWN
    // ==================================================

    message->type = MSG_UNKNOWN;

    return -1;
}