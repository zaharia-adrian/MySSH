#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include <map>
#include <pthread.h>
#include "Session.hpp"

class SessionManager {
public:
    SessionManager();
    void add_session(int, Session&);
    std::map<int, Session>::iterator remove_session(std::map<int, Session>::iterator);
    Session& get_session(int);
    void add_fd(int);
    void remove_fd(int);
    int max_fd;
    fd_set set;
    std::map<int, Session> sessions;
private:
};

#endif