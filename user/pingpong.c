#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        printf("Invalid amounts of arguments\n");
        exit(1);
    } 

    int ping[2], pong[2];

    if (-1 == pipe(ping) || -1 == pipe(pong)) {
        printf("Can't initialize pipes\n");
        exit(1);
    }

    int pid = fork();

    if (pid < 0) {
        printf("Can't fork\n");
        exit(1);
    }

    if (pid == 0) {
        close(ping[1]); // Close write end of ping (Parent process)
        char buffer[4];
        read(ping[0], buffer, sizeof(buffer)); // Read from ping (Parent process)
        printf("%d: received ping\n", getpid());
        close(ping[0]); // Close read end of ping (Parent process)
 
        close(pong[0]); // Close read end of pong (Child process)
        write(pong[1], "pong", 4); // Write to pong (Child process)
        close(pong[1]); // Close write end of pong (Child process)
    }
    else {
        close(ping[0]); // Close read end of ping (Child process)
        write(ping[1], "ping", 4); // Write to ping (Child process)
        close(ping[1]); // Close write end of ping (Child process)
        wait(0); // Wait for child process to finish

        close(pong[1]); // Close write end of pong (Parent process)
        char buffer[4]; 
        read(pong[0], buffer, sizeof(buffer)); // Read from pong (Parent process)
        printf("%d: received pong\n", getpid()); 
        close(pong[0]); // Close read end of pong (Parent process)
    }

    exit(0);
}