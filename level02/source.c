#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int size;

    char password[112];
    memset(password, 0, 112);

    char file_content[48];
    memset(file_content, 0, 48);

    char username[100];
    memset(username, 0, 100);

    FILE* file = fopen("/home/users/level03/.pass", "r");

    if (file == (FILE*)0x0) {
        fwrite("ERROR: failed to open password file\n", 1, 0x24, stderr);
        exit(1);
    }

    size = fread(file_content, 1, 0x29, file);
    size = strcspn(file_content, "\n");
    file_content[size] = '\0';

    if (size != 0x29) {
        fwrite("ERROR: failed to read password file\n", 1, 0x24, stderr);
        exit(1);
    }

    fclose(file);

    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    printf("--[ Username: ");
    fgets(username, 100, stdin);
    size = strcspn(username, "\n");
    username[size] = '\0';

    printf("--[ Password: ");
    fgets(password, 100, stdin);
    size = strcspn(password, "\n");
    password[size] = '\0';
    puts("*****************************************");

    size = strncmp(file_content, password, 0x29);

    if (size == 0) {
        printf("Greetings, %s!\n", username);
        system("/bin/sh");
        return 0;
    }

    printf(username);
    puts(" does not have access!");
    exit(1);
}
