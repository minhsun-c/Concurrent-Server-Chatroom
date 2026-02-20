# Concurrent Server Chatroom

A high-performance, multi-user chat server implemented in C. This project features a custom shell environment for users, a concurrent networking architecture, and a persistent database backend for managing messages, users, and groups.

## Project Overview

The system is designed in three integrated layers:

1. **Custom Shell & Command Execution**: A sophisticated command parser that handles standard UNIX-style execution (`fork` + `exec`), basic pipes (`|`), and specialized "numbered pipes" for advanced redirection.
2. **Concurrent Socket Networking**: A TCP server architecture allowing multiple concurrent clients to connect via `telnet` or similar socket clients.
3. **Data Persistence**: A structured backend (using file-based or SQL-style logic) to manage user registration, login sessions, mail (messages), and group memberships.

---

## Directory Structure

| Directory | Description |
| --- | --- |
| `bin/` | Compiled executable binaries for client-side tools and commands. |
| `bsrc/` | Source code for individual command utilities (e.g., `mailto`, `login`, `createGroup`). |
| `include/` | Header files (`.h`) defining data structures, signals, and networking protocols. |
| `obj/` | Compiled object files (`.o`) used during the linking process. |
| `src/` | The core server logic divided into `db` (database), `network`, and `shell`. |
| `tmp/` | Temporary storage for session buffers and user lists. |

---

## Core Features

### 1. Shell & IPC (Inter-Process Communication)

The server provides each connected user with a shell-like interface.

* **Command Execution**: Uses `fork()` and `execvp()` to run binaries found in the `bin/` directory.
* **Piping**: Supports standard piping (`ls | cat`).
* **Numbered Pipes**: (As indicated by `exe_numpipe.c`) Supports sending command output to a future command in the sequence, allowing complex multi-step workflows.

### 2. Networking & Concurrency

* **TCP/IP Sockets**: Built on the standard Berkeley Sockets API.
* **Multi-User Support**: Handles multiple connections simultaneously using a concurrent execution model (process-per-client or multiplexing).
* **Telnet Compatible**: No custom client is required; users can connect using standard telnet protocols.

### 3. Database & Management

The `src/db` and `bsrc/` modules handle the state of the chatroom:

* **User Management**: `register`, `login`, and `remove`.
* **Messaging**: `mailto` for sending messages and `listMail` / `delMail` for inbox management.
* **Group Chat**: Features to `createGroup`, `listGroup`, `addTo` (add members), and `leaveGroup`.

---

## Getting Started

### Prerequisites

* GCC or Clang compiler
* `make` build utility
* A Unix-like environment (Linux/macOS)
* MySQL Client Library: Required for database connectivity. On macOS (Homebrew), install via:
```bash
brew install mysql
```

### Building the Project

To compile the server and all associated command binaries:

```bash
make
```

### Running the Server

Start the main server process:

```bash
./myserver 127.0.0.1 [port]
```

### Connecting as a Client

Open a new terminal and connect via telnet:

```bash
telnet 127.0.0.1 [port]
```

---

## Available Commands

Once connected, the following commands are available within the shell:

* `login`: Authenticate your session.
* `register`: Create a new user account.
* `mailto [user]`: Send a message to a specific user.
* `createGroup [name]`: Start a new chat group.
* `listGroup`: Show all available groups.
* `number`: A utility to add line numbers to output (useful for piping).