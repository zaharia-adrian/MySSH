#include "common/Common.hpp"
#include "common/Protocol.hpp"
#include "client/Session.hpp"
#include "client/Terminal.hpp"

#include <iostream>
#include <vector>
#include <sys/ioctl.h>

int main(int argc, char ** argv){

    if(argc < 2) {
        printf("usage: %s <username>\n", argv[0]);
        return 1;
    }
    AuthReq auth_req;
    strcpy(auth_req.username, argv[1]);

    printf("insert password for %s: ", argv[1]);
    scanf("%s", auth_req.password);
    

    const char *ip = "64.176.71.30";
    // const char *ip = "127.0.0.1";
    Session client(ip, PORT);

    client.send_packet(MsgType::AUTH, (char*)&auth_req, sizeof(AuthReq));
    
    Header auth_h;
    client.read_n_bytes((char*)&auth_h, sizeof(Header));
    
    AuthRes auth_res;
    client.read_n_bytes((char*)&auth_res, auth_h.length);

    if(auth_res.status == 0 || auth_res.status == -1){
        printf("%s\n", auth_res.msg);
        return 1;
    }
    printf("%s\n", auth_res.msg);
    
    TerminalManager term;
    struct winsize ws;

    check(-1 != ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws), "Error at getting the window size!");

    term.enable_raw_mode();

    client.send_packet(MsgType::RESIZE, (char*)&ws, sizeof(ws));
  
    fd_set read_fds;
    char buf[MAX_BUFFER_SIZE];

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);    
        FD_SET(client.get_fd(), &read_fds); 

        
        check(-1 != select(client.get_fd() + 1, &read_fds, NULL, NULL, NULL), "Error at select()");
      
        if (FD_ISSET(client.get_fd(), &read_fds)) {
            Header h;
            client.read_n_bytes((char*)&h, sizeof(h));

            std::vector<char> payload(h.length);
            if (h.length > 0) client.read_n_bytes(payload.data(), h.length);

            if (h.type == MsgType::OUTPUT) {
                write(STDOUT_FILENO, payload.data(), h.length);
            }else if(h.type == MsgType::EXIT){
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {    
            int n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n > 0) {
                client.send_packet(MsgType::INPUT, buf, n);
            }
        }
    }
    
}
