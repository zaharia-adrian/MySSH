#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <iostream>

#define MAX_BUFFER_SIZE 4096
#define BUFFER_SIZE 256
#define PORT 2025

#define check_failure(bad, expr, msg)              \
    ({                                             \
        if ((expr) == (bad)) {                     \
            std::cerr << (msg) << '\n';            \
            exit(EXIT_FAILURE);                    \
        }                                          \
    })

#define check_ok(ok, expr, msg)                    \
    ({                                             \
        if ((expr) != (ok)) {                      \
            std::cerr << (msg) << '\n';            \
            exit(EXIT_FAILURE);                    \
        }                                          \
    })


#define check(expr, msg)                           \
    ({                                             \
        if (!(expr)) {                             \
            std::cerr << (msg) <<'\n';             \
            exit(EXIT_FAILURE);                    \
        }                                          \
    })
#endif


