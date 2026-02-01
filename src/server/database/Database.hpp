#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <sqlite3.h>
#include <openssl/evp.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "common/Common.hpp"

struct User {
    int id;
    std::string username;
    std::string password_hash;
};

class Database {
public:
    Database();
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool initialize();
    bool create_user(const std::string& username, const std::string& password);
    int authenticate_user(const std::string& username, const std::string& password);
    
    bool user_exists(const std::string& username);

    static void init();

private:
    sqlite3* db = nullptr;
    
    std::string hash_password(const std::string& password);
};

#endif