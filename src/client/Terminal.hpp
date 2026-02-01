#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

class TerminalManager {
    struct termios original_termios;
    bool is_raw = false;
public:
    TerminalManager() {
        tcgetattr(STDIN_FILENO, &original_termios);
    }

    ~TerminalManager() {
        disable_raw_mode();
    }

    void enable_raw_mode() {
        if (is_raw) return;
        
        struct termios raw = original_termios;
        
        raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
        raw.c_iflag &= ~(IXON | ICRNL); 
        
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        is_raw = true;
    }

    void disable_raw_mode() {
        if (!is_raw) return;
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
        is_raw = false;
    }

    struct winsize get_window_size() {
        struct winsize ws;
        check(-1 != ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws), "Error at getting the window size!");
        return ws;
    }
    
    bool is_in_raw_mode() { return is_raw; }
};


#endif