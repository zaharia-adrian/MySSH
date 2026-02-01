#include "Threads.hpp"

struct ThreadArgs{
    Threads *pool;
    int cnt;
};

Threads::Threads(size_t num_threads) : 
    notify_pipe(std::vector<int[2]>(num_threads)),
    rr(0)
{

    for (size_t i = 0; i < num_threads; i++){
        pthread_t tid;
        check(-1 != pipe(notify_pipe[i]), "[server] Error at pipe()");

        ThreadArgs *arg = new ThreadArgs{this, i};

        //should delete the arg, but because the server exits, it is not needed
        check(-1 != pthread_create(&tid, nullptr, worker_routine, arg), "[server] Failed to create thread");
        threads.push_back(tid);
    }
}

Threads::~Threads()
{
    //write -1 for thread to stop
    int stop_condition = -1;
    for (size_t i = 0; i < threads.size(); i++){
        check(-1 != write(notify_pipe[i][1], (char*)&stop_condition, sizeof(int)), "[server] Error at write() the stop contidion");
    }

    for (size_t i = 0; i < threads.size(); i++){
        pthread_join(threads[i], nullptr);
    }
    for (size_t i = 0; i < threads.size(); i++){
        check(-1 != close(notify_pipe[i][0]), "[server] Error at close() the pipe");
        check(-1 != close(notify_pipe[i][1]), "[server] Error at close() the pipe");
    }
}

void Threads::add_client(int fd){

    check(-1 != write(notify_pipe[rr][1], &fd, sizeof(int)), "[server] Error at write() client_fd to thread");
    
    // round-robin for next client
    rr = (rr + 1) % threads.size();
}

void *Threads::worker_routine(void *arg){
    ThreadArgs *th_arg = static_cast<ThreadArgs *>(arg);
    th_arg->pool->run_worker(th_arg->cnt);
    return nullptr;
}

void Threads::run_worker(int id){
    Database db;
    SessionManager sm;

    sm.add_fd(notify_pipe[id][0]);

    Header header;

    while (true){

        fd_set read_fds = sm.set;
        check(-1 != select(sm.max_fd + 1, &read_fds, NULL, NULL, NULL), "[thread] error at select()");

        if (FD_ISSET(notify_pipe[id][0], &read_fds)){
            int client_fd;
            
            check(-1!=read(notify_pipe[id][0], (char *)&client_fd, sizeof(int)),"[thread] error at read()");

            //stop condition
            if(client_fd == -1){
                break;
            }

            SSL *ssl = SSL_new(SSLContext::instance());
            SSL_set_fd(ssl, client_fd);

            check(-1 != SSL_accept(ssl), "[thread] error at ssl accept()");
            
            Protocol::read_n_bytes(ssl, (char *)&header, sizeof(header));

            check(header.type == MsgType::AUTH, "[thread] invalid/unexpected message from client | requires AUTH");
                
            AuthReq auth_req;
            Protocol::read_n_bytes(ssl, (char*)&auth_req, header.length);


            AuthRes auth_res;
            auth_res.status = db.authenticate_user(auth_req.username, auth_req.password);
            
            switch (auth_res.status){
                case -1:{
                    sprintf(auth_res.msg, "%s %s %s", "user", auth_req.username, "does not exist!");
                    Protocol::send_packet(ssl, MsgType::AUTH, (char*)&auth_res, sizeof(auth_res));
                    break;
                }
                case 0:{
                    sprintf(auth_res.msg, "%s", "invalid password");
                    Protocol::send_packet(ssl, MsgType::AUTH, (char*)&auth_res, sizeof(auth_res));
                    break;
                }
                default:{
                    sprintf(auth_res.msg, "%s %s", "logged in as", auth_req.username);
                    
                    Session session(client_fd, ssl, auth_req.username);

                    Protocol::send_packet(ssl, MsgType::AUTH, (char*)&auth_res, sizeof(auth_res));
                    Header header;
                    Protocol::read_n_bytes(ssl, (char*)&header, sizeof(header));
                    
                    struct winsize ws;
                    check(header.type == MsgType::RESIZE, "[thread] invalid/unexpected message from client | reaquires RESIZE");

                    Protocol::read_n_bytes(ssl, (char*)&ws, sizeof(ws));
                    check(-1 != ioctl(session.pty_fd, TIOCSWINSZ, &ws), "[thread] error at ioctl() to resize the bash");
                    check(-1 != kill(session.pty_pid, SIGWINCH), "[thread] error at kill()");

                    sm.add_session(auth_res.status, session);
                }
            }        
        }

        for (auto it = sm.sessions.begin(); it != sm.sessions.end();){
            auto &s = it->second;

            if (FD_ISSET(s.client_fd, &read_fds)){
                s.handle_client();
            }

            if (FD_ISSET(s.pty_fd, &read_fds)){
                if (!s.handle_pty()){
                    it = sm.remove_session(it); 
                    continue;
                }
            }
            ++it;
        }
    }
}
