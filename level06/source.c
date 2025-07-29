#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>

int auth(char* login, uint32_t serial) {
    login[strcspn(login, "\n")] = 0;

    size_t len = strnlen(login, 32);
    if (len <= 5) {
        return 1;
    }

    if (ptrace(0, 0, 1, 0) == -1) {
        puts("\e[32m.---------------------------.");
        puts("\e[31m| !! TAMPERING DETECTED !!  |");
        puts("\e[32m.---------------------------.");
        return 1;
    }

    int hash = ((int)(login[3])) ^ 0x1337 + 0x5eeded;

    for (int i = 0; i < len; i++) {
        if (login[i] <= 31)
            return 1;

        int tmp1 = login[i] ^ hash;
        int tmp2 = 0x88233b2b * tmp1;
        int tmp3 = (tmp1 - tmp2) / 2;
        int tmp4 = (tmp3 + tmp2) / 1024 * 0x539;
        hash += tmp1 - tmp4;
    }

    if (hash == serial)
        return 0;

    return 1;
}

int main() {
    char login[32];
    uint32_t serial;

    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    printf("-> Enter Login: ");
    fgets(login, 32, stdin);

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");
    printf("-> Enter Serial: ");
    scanf("%u", &serial);

    if (!auth(login, serial)) {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }

    return 1;
}
