#ifndef SESSION_HPP
#define SESSION_HPP

#include <unistd.h>
#include <pty.h>
#include <sys/types.h>
#include <openssl/ssl.h>
#include <string>
#include <vector>
#include "common/Protocol.hpp"
#include "server/container/Container.hpp"

class Session{
public:
    int client_fd;
    int pty_fd;
    pid_t pty_pid;
    SSL *ssl;
    std::string username;

    Session(int client_fd, SSL *ssl, const char * username)
        : client_fd(client_fd), ssl(ssl), username(username)
    {
        pty_pid = forkpty(&pty_fd, NULL, NULL, NULL);

        check(-1 != pty_pid, "[thread] forkpty() failled!");
    
        if (pty_pid == 0){
            Container::enter_jail(username);
        }
    }

    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    
    Session(Session &&other) : 
        client_fd(other.client_fd),
        pty_fd(other.pty_fd),
        pty_pid(other.pty_pid),
        ssl(other.ssl),
        username(std::move(other.username))
    {
        other.ssl = nullptr;
        other.pty_fd = -1;
        other.client_fd = -1;
    }

    void handle_client(){
        Header header;
        read_n_bytes((char*)&header, sizeof(header));
        
        if (header.length > 0){
            std::vector<char> payload(header.length);
            read_n_bytes(payload.data(), header.length);
            if (header.type == MsgType::INPUT){       
                check(-1 != write(pty_fd, payload.data(), payload.size()), "[thread] error at write to the pty");
            }
        }
    }
    bool handle_pty(){
        char buf[MAX_BUFFER_SIZE]{};
        int n = read(pty_fd, buf, sizeof(buf));
        if (n <= 0){
            send_packet(MsgType::EXIT, nullptr, 0);
            return false;
        }
        send_packet(MsgType::OUTPUT, buf, n);
        return true;
    }

    bool read_n_bytes(char *buf, int len){
        return Protocol::read_n_bytes(ssl, buf, len);
    }

    bool send_packet(const MsgType type, char *buf, int len){
        return Protocol::send_packet(ssl, type, buf, len);
    }

    ~Session(){
        if (ssl){
            SSL_shutdown(ssl); 
            SSL_free(ssl);
        }
    }
};

#endif