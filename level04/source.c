#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    char buffer[128];
    memset(buffer, 0, 32);

    int status = 0;

    if (pid == 0) {
        prctl(1, 1);
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        puts("Give me some shellcode, k");
        gets(buffer);
    } else {
        while (ptrace(PTRACE_PEEKUSER, pid, 44, 0) != 11) {
            wait(&status);

            if ((status & 127) == 0 || (((status & 127) + 1) / 2) > 0) {
                puts("child is exiting...");
                return 0;
            }
        }
        puts("no exec() for you");
        kill(pid, 9);
    }
    return 0;
}
