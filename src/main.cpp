#include <iostream>
#include <server/database/Database.hpp>


int main(){
    Database::init();
    Database db;
    std::string username = "guest";
    std::string password = "pass";
    std::cout << db.create_user(username, password)<<'\n';

    return 0;
}