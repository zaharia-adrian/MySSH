#ifndef SSL_CONTEXT_HPP
#define SSL_CONTEXT_HPP

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>
#include <stdexcept>
#include <vector>

#include "common/Common.hpp"

class SSLContext {
public:
    SSLContext() = delete;

    static SSL_CTX* instance() {
        static SSL_CTX* ctx = [] {
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();

            SSL_CTX* c = SSL_CTX_new(TLS_server_method());

            check(nullptr != c, "[ssl] Error at ssl_ctx_new(), unable to create context!");

            check(0 < SSL_CTX_use_certificate_file(c, "certs/certificate.crt", SSL_FILETYPE_PEM), "[ssl] failed to load certificate");
            
            check(0 < SSL_CTX_use_PrivateKey_file(c, "certs/private.key", SSL_FILETYPE_PEM), "[ssl] failed to load private key");

            return c;
        }();

        return ctx;
    }
};



#endif