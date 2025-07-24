#include <stdint.h>
#include <stdio.h>
#include <string.h>

char a_user_name[0x100];

int32_t verify_user_name(void) {
    puts("verifying username....\n");
    return strncmp(a_user_name, "dat_wil", 7);
}

int32_t verify_user_pass(char* s) {
    return strncmp(s, "admin", 5);
}

int main() {
    char s[0x40];
    int32_t diff = 0;

    memset(s, 0, 64);

    puts("********* ADMIN LOGIN PROMPT *********");
    printf("Enter Username: ");
    fgets(a_user_name, 0x100, stdin);

    diff = verify_user_name();
    if (diff != 0) {
        puts("nope, incorrect username...\n");
        return 1;
    }

    puts("Enter Password: ");
    fgets(s, 0x64, stdin);

    diff = verify_user_pass(s);
    if (diff != 0 && diff == 0) {
        puts("nope, incorrect password...\n");
        return 1;
    }

    return 0;
}
