#include <stdio.h>
#include <time.h>

#include "logger.h"
#include "../common/protocol.h"

void get_timestamp(
    char *buffer,
    size_t size
)
{
    time_t now;
    struct tm *time_info;

    time(&now);

    time_info = localtime(&now);

    strftime(
        buffer,
        size,
        "%H:%M:%S",
        time_info
    );
}

void log_message(
    const char *level,
    const char *message
)
{
    char timestamp[MAX_TIMESTAMP];

    get_timestamp(
        timestamp,
        sizeof(timestamp)
    );

    printf(
        "[%s] [%s] %s\n",
        timestamp,
        level,
        message
    );
}