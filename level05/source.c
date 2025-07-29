#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    uint32_t i = 0;
    char buffer[100];

    fgets(buffer, 100, stdin);

    while (i < strlen(buffer)) {
        if (buffer[i] > 64 && buffer[i] <= 90) {
            buffer[i] ^= 0x20;
        }
        i++;
    }

    printf(buffer);
    exit(0);
}
