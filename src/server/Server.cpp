
#include <pty.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mount.h>
#include <string>
#include <netinet/tcp.h>

#include "server/container/Container.hpp"
#include "server/encryption/SSLContext.hpp"
#include "common/Common.hpp"
#include "common/Protocol.hpp"
#include "server/session/Session.hpp"
#include "server/threads/Threads.hpp"
#include "server/database/Database.hpp"


static int sd;
int main(int arc, char **argv){

    Database::init();

    struct sockaddr_in server;
    struct sockaddr_in from;
    
    check(-1 != (sd = socket(AF_INET, SOCK_STREAM, 0)), "[server] Error at socket()");
    
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    
    check(-1 != bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)), "[server] Error at bind()");
    
    check(-1 != listen(sd, 5), "[server] Error at listen()");
    
    Threads threads(2);

    printf("Server running...\n");
    
    while (true) {
        int client_fd = accept(sd, NULL, NULL);
        check(-1 != client_fd, "[server] Error at accept()");
        int flag = 1;
        check(-1 != setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag)), "[server] error at setsockopt()");
        threads.add_client(client_fd);
    }
    return 0;
}
