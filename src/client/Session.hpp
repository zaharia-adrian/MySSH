#ifndef SESSION_HPP
#define SESSION_HPP

#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include "common/Protocol.hpp"

class Session {
    SSL_CTX* ctx;
    SSL* ssl;
    int sock_fd;

public:
    Session(const char * server_ip, int port) {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        
        ctx = SSL_CTX_new(TLS_client_method());
        
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

        sock_fd = socket(AF_INET, SOCK_STREAM, 0);

        int flag = 1;
        setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(server_ip);

        check(-1 != connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)), "TCP Connection failed");

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock_fd);
        
        check(-1 != SSL_connect(ssl), "SSL Handshake failed");        
    }


    bool read_n_bytes(char *buf, int len){
        return Protocol::read_n_bytes(ssl, buf, len);
    }

    bool send_packet(const MsgType type, char *buf, int len) {
        return Protocol::send_packet(ssl, type, buf, len);
    }

    SSL* get_ssl() { return ssl; }
    int get_fd() { return sock_fd; }
};

#endif
