# NetChat — Multi-Client TCP Chat Application

NetChat is a multi-client TCP chat application developed in C on Linux using socket programming and POSIX threads.

## Features

- Multi-client TCP communication
- Client-server architecture
- Username validation
- Public messaging
- Private messaging
- Online user list
- `/help`, `/users`, `/whoami`, `/msg`, and `/quit` commands
- Thread-based client handling
- Client management with mutex synchronization
- Message validation and parsing
- Server-side event logging
- Graceful client disconnect handling
- Graceful server shutdown

## Technologies

- C
- Linux
- TCP/IP
- Socket Programming
- POSIX Threads
- Mutex Synchronization
- Git
- GitHub

## Project Structure

```text
NetChat/
├── client/
│   └── client.c
├── common/
│   ├── protocol.c
│   └── protocol.h
├── server/
│   ├── server.c
│   ├── client_manager.c
│   ├── client_manager.h
│   ├── logger.c
│   └── logger.h
├── tests/
├── docs/
├── README.md
└── .gitignore