#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <stdint.h>
#include <openssl/ssl.h>
#include "common/Common.hpp"

enum class  MsgType{
    AUTH,         
    INPUT,        
    OUTPUT,       
    RESIZE,
    EXIT,       
};

struct Header{
    MsgType type;  
    uint32_t length;
};

struct AuthReq{
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
};

struct AuthRes{
    int32_t status;
    char msg[BUFFER_SIZE];
};


class Protocol {
public:

    static bool send_n_bytes(SSL* ssl, char *buf, int len){
        int sent = 0;
        while (sent < len){
            int n = SSL_write(ssl, buf + sent, len - sent);
            if (n <= 0) return false;
            sent += n;
        }
        return true;
    }

    static bool send_packet(SSL* ssl, const MsgType type, char *buf, int len){
        Header h{type, len};
        bool sent = send_n_bytes(ssl, (char *)&h, sizeof(h));
        if (!sent) return false;
        return send_n_bytes(ssl, buf, len);
    }

    static bool read_n_bytes(SSL *ssl, char* buffer, int n) {
        int received = 0;
        while (received < n) {
            int r = SSL_read(ssl, buffer + received, n - received);
            if (r <= 0) return false;
            received += r;
        }
        return true;
    }

};



#endif