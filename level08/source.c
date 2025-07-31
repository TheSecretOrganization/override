#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void log_wrapper(FILE* file, char* s, char* filename) {
    char buf[0xff];

    strcpy(buf, s);
    snprintf(buf + strlen(buf), 0xfe - strlen(buf), filename);
    buf[strcspn(buf, "\n")] = '\0';
    fprintf(file, "LOG: %s\n", buf);
}

int32_t main(int32_t argc, char** argv) {
    char buf[0x64];
    int8_t c = -1;

    if (argc != 2)
        printf("Usage: %s filename\n", *argv);

    FILE* log_file = fopen("./backups/.log", "w");
    if (!log_file) {
        printf("ERROR: Failed to open %s\n", "./backups/.log");
        exit(1);
    }

    log_wrapper(log_file, "Starting back up: ", argv[1]);

    FILE* src_file = fopen(argv[1], "r");
    if (!src_file) {
        printf("ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }

    strcpy(buf, "./backups/");
    strncat(buf, argv[1], 0x63 - strlen(buf));

    int32_t fd = open(buf,  O_WRONLY | O_CREAT | O_TRUNC, 0660);
    if (fd < 0) {
        printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
        exit(1);
    }

    do {
        c = fgetc(src_file);
        write(fd, &c, 1);
    } while (c != -1);

    log_wrapper(log_file, "Finished back up ", argv[1]);
    fclose(src_file);
    close(fd);

    return 0;
}
