#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <sched.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pwd.h>
#include <stdexcept>
#include <string>
#include <iostream>

#include "common/Common.hpp"

class Container {
public:
    static void enter_jail(const char * username);
};

#endif