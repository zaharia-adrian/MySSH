#ifndef THREADS_HPP
#define THREADS_HPP

#include <pthread.h>
#include <queue>
#include <functional>
#include <vector>
#include <iostream>
#include <unistd.h> 

#include "server/database/Database.hpp"
#include "server/session/SessionManager.hpp"
#include "server/encryption/SSLContext.hpp"


class Threads {
public:
    
    Threads(size_t);
    
    ~Threads();

    void add_client(int);

private:
    static void* worker_routine(void* arg);

    void run_worker(int);

    std::vector<pthread_t> threads;
    std::vector<int[2]> notify_pipe;
    
    unsigned int rr;
};

#endif