#include "SessionManager.hpp"


SessionManager::SessionManager() {
    FD_ZERO(&set);
}

std::map<int, Session>::iterator SessionManager::remove_session(std::map<int, Session>::iterator it) {
    this->remove_fd(it->second.client_fd);
    this->remove_fd(it->second.pty_fd);
    return sessions.erase(it);
}
void SessionManager::add_session(int id, Session& session){
    this->add_fd(session.client_fd);
    this->add_fd(session.pty_fd);
    sessions.emplace(id, std::move(session));
}

void SessionManager::add_fd(int fd){
    FD_SET(fd, &set);
    if (fd > max_fd) max_fd = fd;
}
void SessionManager::remove_fd(int fd){
    FD_CLR(fd, &set);
}

Session& SessionManager::get_session(int id) {
    return sessions.find(id) -> second;
}
