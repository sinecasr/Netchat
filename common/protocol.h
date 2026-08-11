#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

#define MAX_USERNAME 50
#define MAX_MESSAGE 1024
#define MAX_PROTOCOL_MESSAGE 1200
#define MAX_TIMESTAMP 30

// Message types
#define MSG_UNKNOWN  0
#define MSG_PUBLIC   1
#define MSG_PRIVATE  2
#define MSG_USERS    3
#define MSG_LOGIN    4
#define MSG_LOGOUT   5
#define MSG_QUIT     6

// Message structure
struct Message
{
    int type;

    char sender[MAX_USERNAME];

    char receiver[MAX_USERNAME];

    char timestamp[MAX_TIMESTAMP];

    char text[MAX_MESSAGE];
};


// Create public message
int create_public_message(
    char *buffer,
    size_t buffer_size,
    const char *sender,
    const char *text
);


// Create private message
int create_private_message(
    char *buffer,
    size_t buffer_size,
    const char *sender,
    const char *receiver,
    const char *text
);


// Create login message
int create_login_message(
    char *buffer,
    size_t buffer_size,
    const char *username
);


// Create logout message
int create_logout_message(
    char *buffer,
    size_t buffer_size,
    const char *username
);


// Create users request
int create_users_message(
    char *buffer,
    size_t buffer_size
);


// Create quit message
int create_quit_message(
    char *buffer,
    size_t buffer_size
);


// Parse received message
int parse_message(
    const char *buffer,
    struct Message *message
);

#endif