#include "Container.hpp"

void Container::enter_jail(const char * username) {
    check(0 == unshare(CLONE_NEWNS), "Unshare failed");
    check(0 == mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL), "Make private failed");

    const char* jail_root = "guest";

    
    char user_folder[BUFFER_SIZE], user_subfolder[BUFFER_SIZE];
    sprintf(user_folder, "guest/home/%s", username);
    sprintf(user_subfolder, "/home/%s", username);

    check(-1 != mount(user_folder, "/home/ssh_guest", "none", MS_BIND, NULL), "error at mount");

    check(-1 != mount("/lib",   "guest/lib",   "none", MS_BIND | MS_RDONLY, NULL), "error at mount() for /lib");
    check(-1 != mount("/lib64", "guest/lib64", "none", MS_BIND | MS_RDONLY, NULL), "error at mount() for /lib64");
    check(-1 != mount("/usr",   "guest/usr",   "none", MS_BIND | MS_RDONLY, NULL), "error at mount() for /usr");
    check(-1 != mount("/dev", "guest/dev",   "none", MS_BIND | MS_REC,    NULL), "error at mount() for /dev");

    check(-1 != mount(user_folder, user_folder, "none", MS_BIND | MS_RDONLY, NULL), "error at mount()");

    
    chdir(jail_root);
    
    
    struct passwd* pw = getpwnam("ssh_guest");
    check(pw, "Guest user not found");
    check(-1 != chroot("."), "chroot() failed");

    check(-1 != chdir(user_subfolder), "chdir /home");

    check(-1 != setgid(pw->pw_gid), "setgid failed");
    
    check(-1 != setuid(pw->pw_uid), "setuid failed");
    
    setenv("PATH", "/bin", 1);
    setenv("HOME", "/home", 1);

    execl("/bin/bash", "bash", "--norc", "-i", NULL);
    check(false, "error at exec");
    exit(1);
}

