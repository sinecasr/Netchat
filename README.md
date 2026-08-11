# NetChat — Multi-Client TCP Chat Application

NetChat is a **multi-client TCP chat application** developed in **C on Linux** using socket programming and POSIX threads. It follows a client-server architecture and supports real-time public and private messaging between multiple connected clients.

## Features

* Multi-client TCP communication
* Client-server architecture
* Username validation
* Public messaging
* Private messaging
* Online user list
* `/help`, `/users`, `/whoami`, `/msg`, and `/quit` commands
* Concurrent client handling
* Thread-safe client management using mutex synchronization
* Message parsing and input validation
* Server-side event logging
* Graceful client disconnection
* Graceful server shutdown

## Technologies

* C
* Linux
* TCP/IP
* Socket Programming
* POSIX Threads
* Mutex Synchronization
* Git
* GitHub

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
├── screenshots/
├── Makefile
├── README.md
└── .gitignore
```

## Architecture

NetChat uses a **client-server architecture**.

```text
             ┌───────────────┐
             │     Server    │
             │               │
             │ TCP Socket    │
             │ Client Manager│
             │ Logger        │
             └───────┬───────┘
                     │
              TCP Connections
          ┌──────────┼──────────┐
          │          │          │
          ▼          ▼          ▼
       Client 1   Client 2   Client 3
        Alice        Bob      Charlie
```

The server listens for incoming TCP connections. When a client connects, the server handles the client independently while continuing to accept other connections.

## Build

Make sure GCC and pthread support are installed on Linux.

Build the project using:

```bash
make
```

Alternatively, the server can be compiled using:

```bash
gcc server/server.c server/client_manager.c server/logger.c common/protocol.c -o server/server -pthread
```

## Run

### Start the server

```bash
./server/server
```

### Start a client

Open another terminal and run:

```bash
./client/client
```

Connect multiple clients to test concurrent communication.

## Available Commands

| Command                 | Description                  |
| ----------------------- | ---------------------------- |
| `/help`                 | Display available commands   |
| `/users`                | Display online users         |
| `/whoami`               | Display the current username |
| `/msg <user> <message>` | Send a private message       |
| `/quit`                 | Disconnect from the server   |

Normal text messages are sent as public messages to connected users.

## Screenshots

### Server

![NetChat Server](screenshots/server.png)

### Client Login

![Client Login](screenshots/client-login.png)

### Public Messaging

![Public Messaging](screenshots/public-chat.png)

### Private Messaging

![Private Messaging](screenshots/private-message.png)

### Multiple Clients

![Multiple Clients](screenshots/multiple-clients.png)

## Protocol

NetChat uses a custom application-layer message protocol to structure communication between the client and server.

The protocol handles:

* Message types
* Username information
* Public messages
* Private messages
* Commands
* Input validation
* Message parsing

Detailed protocol documentation is available in:

```text
docs/protocol.md
```

## Documentation

Additional project documentation:

```text
docs/
├── architecture.md
└── protocol.md
```

## Testing

The `tests/` directory contains project testing resources used to verify application functionality and communication behavior.

## Future Improvements

* Secure communication using TLS
* Persistent chat history
* Improved client-side UI
* More comprehensive automated testing
* File transfer support

## Author

Developed as a C/Linux networking project to demonstrate socket programming, concurrent client handling, TCP/IP communication, and protocol design.
