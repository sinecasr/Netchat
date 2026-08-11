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

NetChat follows a **client-server architecture**.

```text
             ┌──────────────────┐
             │   NetChat Server │
             │                  │
             │  TCP Socket      │
             │  Client Manager  │
             │  Logger          │
             └────────┬─────────┘
                      │
                 TCP Connections
             ┌────────┼────────┐
             │        │        │
             ▼        ▼        ▼
          Client 1 Client 2 Client 3
           Alice      Bob    Charlie
```

The server listens for incoming TCP connections. Each connected client is handled independently, allowing multiple clients to communicate concurrently.

## Build

Build the project using the provided Makefile:

```bash
make
```

The server can also be compiled manually using GCC:

```bash
gcc server/server.c server/client_manager.c server/logger.c common/protocol.c -o server/server -pthread
```

## Run

### Start the Server

```bash
./server/server
```

### Start the Client

Open another terminal and run:

```bash
./client/client
```

Multiple clients can be started from separate terminals to test concurrent communication.

## Available Commands

| Command                 | Description                  |
| ----------------------- | ---------------------------- |
| `/help`                 | Display available commands   |
| `/users`                | Display online users         |
| `/whoami`               | Display the current username |
| `/msg <user> <message>` | Send a private message       |
| `/quit`                 | Disconnect from the server   |

Normal text messages are sent as public messages to connected users.

## Protocol

NetChat uses a **custom application-layer message protocol** for communication between clients and the server.

The protocol handles:

* Message types
* Username information
* Public messages
* Private messages
* Commands
* Input validation
* Message parsing

Detailed protocol information is documented in:

```text
docs/protocol.md
```

## Documentation

Additional documentation is available in the `docs/` directory:

```text
docs/
├── architecture.md
└── protocol.md
```

## Testing

The `tests/` directory contains testing resources used to verify application functionality and communication behavior.

## Future Improvements

* TLS-based secure communication
* Persistent chat history
* Improved client interface
* Expanded automated testing
* File transfer support
