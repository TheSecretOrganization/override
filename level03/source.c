#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void decrypt(uint32_t rot) {
    char buffer[17] = "Q}|u`sfg~sf{}|a3";
    uint32_t len = strlen(buffer);

    for (uint32_t i = 0; i < len; i++) {
        buffer[i] = buffer[i] ^ rot;
    }

    if (!strncmp(buffer, "Congratulations!", 0x11)) {
        system("/bin/sh");
    } else {
        puts("\nInvalid Password");
    }
}

void test(uint32_t arg1, uint32_t arg2) {
    uint32_t diff = arg2 - arg1;

    if (diff <= 21) {
        decrypt(diff);
    } else {
        decrypt(rand());
    }
}

int main() {
    uint32_t input;

    srand(time(0));
    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");
    printf("Password:");
    scanf("%u", &input);
    test(input, 322424845);
    return 0;
}
