# MySSH - Educational SSH (Secure Remote Shell)
MySSH is a robust, educational implementation of a client-server remote shell application.
Built using C++ and low-level POSIX APIs, this project demonstrates advanced systems programming concepts, including encrypted communication, pseudo-terminal (PTY) manipulation, thread pooling, and user-specific filesystem isolation.

### Features

- **Client/Server Architecture**: distinct binaries for remote connection and server handling.
- **Secure Communication**: End-to-end encryption using **OpenSSL**.
- **Interactive Shell**: Full support for interactive Bash sessions (including apps like `vim`) using Pseudo-Terminals (PTY).
- **Concurrency**: High-performance **Pre-threading** model to handle multiple simultaneous users.
- **User Isolation**: Loads a pseudo-filesystem for every connected user to ensure environment separation.
- **Sandboxing**: All authenticated remote sessions are mapped to a specific system user (`ssh_guest`) to strictly limit privileges and protect the host kernel.
- **Data Persistence**: **SQLite** database integration for user management and logging.
- **System Level**: Built on raw **POSIX system calls** for fine-grained control.

### Technologies Used

- **Language:** C++
- **Purpose:** Educational / Systems Programming
- **Core Libraries:** OpenSSL, SQLite3
- **System API:** POSIX (Sockets, Threads, Fork/Exec, PTY)

### Prerequisites

- GCC (C++17 or later recommended)
- OpenSSL development libraries (`libssl-dev`)
- SQLite3 development libraries (`libsqlite3-dev`)
- Make

### Installation

```bash
  # Clone the repository
  git clone https://github.com/zaharia-adrian/MySSH
  cd MySSH

  # Create the sandboxed system user (Required)
  sudo useradd -m -s /bin/bash ssh_guest
  
  # Build the project (compiles both server and client)
  make
```

### Usage 
1. Start the Server
  ```bash 
  sudo ./server
  ```

2. Connect via Client
  ```bash 
  ./client <USERNAME>
  ```
