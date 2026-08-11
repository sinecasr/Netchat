# NetChat — Multi-Client TCP Chat Application

NetChat is a **multi-client TCP chat application** developed in **C on Linux** using socket programming and POSIX threads. It follows a client-server architecture and supports real-time communication between multiple connected clients.

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
Netchat/
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
├── screenshots/
├── README.md
└── .gitignore
```

## Architecture

NetChat uses a **client-server architecture**.

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

The server listens for incoming TCP connections and manages communication with multiple clients concurrently.

## Build

The server can be compiled using GCC:

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

Normal text messages are used for public communication between connected clients.

## Protocol

NetChat uses a **custom application-layer message protocol** for communication between the client and server.

The protocol handles:

* Message types
* Username information
* Public messages
* Private messages
* Commands
* Input validation
* Message parsing

The shared protocol implementation is located in:

```text
common/
├── protocol.c
└── protocol.h
```

## Screenshots

Project screenshots are stored in the `screenshots/` directory.

They demonstrate:

* Server startup
* Client login
* Public messaging
* Private messaging
* Multiple-client communication

## Future Improvements

* TLS-based secure communication
* Persistent chat history
* Improved client interface
* Expanded automated testing
* File transfer support

## Author

Developed as a **C/Linux networking project** to demonstrate TCP socket programming, concurrent client handling, client-server communication, synchronization, and application-layer protocol design.
