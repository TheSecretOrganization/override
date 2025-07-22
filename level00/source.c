#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    puts("********************************…");
    puts("* \t     -Level00 -\t\t  *");
    puts("********************************…");
    printf("Password:");
    int32_t var_14;
    scanf("%d", &var_14);

    // 0x149c == 0d5276
    if (var_14 != 0x149c) {
        puts("\nInvalid Password!");
        return 1;
    }

    puts("\nAuthenticated!");
    system("/bin/sh");
    return 0;
}
