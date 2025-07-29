#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void clear_stdin() {
    char val = 0;

    while (1) {
        val = getchar();
        if (val == '\n' || val == EOF) {
            return;
        }
    }
}

unsigned int get_unum(void) {
    uint32_t n = 0;

    fflush(stdout);
    scanf("%u", &n);
    clear_stdin();

    return n;
}

int store_number(int32_t* buf) {
    uint32_t n;
    uint32_t i;

    printf(" Number: ");
    n = get_unum();
    printf(" Index: ");
    i = get_unum();

    if (!(i % 3) || n >> 0x18 != 0xb7) {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!");
        puts(" *** ERROR! ***");
        return 1;
    }

    buf[i] = n;
    return 0;
}

int read_number(int32_t* buf) {
    uint32_t i = 0;

    printf(" Index:");
    i = get_unum();
    printf(" Number at data[%u] is %u\n", i, buf[i]);
    return 0;
}

int main(int argc, char* argv[], char* envp[]) {
    int32_t buf[100] = {0};
    int32_t ret = 0;
    char cmd[20] = {0};

    for (size_t i = 0; argv[i]; i++) {
        memset(argv[i], 0, strlen(argv[i]));
    }

    for (size_t i = 0; envp[i]; i++) {
        memset(envp[i], 0, strlen(envp[i]));
    }

    puts("----------------------------------------------------\n  Welcome to "
         "wil's crappy number storage service!   "
         "\n----------------------------------------------------\n Commands:   "
         "                                       \n    store - store a number "
         "into the data storage    \n    read  - read a number from the data "
         "storage     \n    quit  - exit the program                        "
         "\n----------------------------------------------------\n   wil has "
         "reserved some storage :>                 "
         "\n----------------------------------");

    while (1) {
        printf("Input command: ");
        fgets(cmd, 20, stdin);
        cmd[strlen(cmd) - 1] = 0;

        if (!strncmp(cmd, "store", 5)) {
            ret = store_number(buf);
        } else if (!strncmp(cmd, "read", 4)) {
            ret = read_number(buf);
        } else if (!strncmp(cmd, "quit", 4)) {
            return 0;
        }

        if (ret == 0) {
            printf(" Completed %s command successfully\n", cmd);
        } else {
            printf(" Failed to do %s command\n", cmd);
        }

        memset(cmd, 0, 20);
    }

    return 0;
}
