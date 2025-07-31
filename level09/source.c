#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct msg {
    char content[140];
    char username[40];
    int len;
} msg_t;

void secret_backdoor() {
    char buf[128];

    fgets(buf, 128, stdin);
    system(buf);
}

void set_msg(msg_t* m) {
    char buf[1024];

    memset(buf, 0, 1024);
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(buf, 1024, stdin);
    strncpy(m->content, buf, m->len);
}

void set_username(msg_t* m) {
    char buf[128];

    memset(buf, 0, 128);

    puts(">: Enter your username");
    printf(">>: ");
    fgets(buf, 128, stdin);

    for (int i = 0; i < 41 && buf[i] != '\0'; i++) {
        m->username[i] = buf[i];
    }

    printf(">: Welcome, %s", m->username);
}

void handle_msg() {
    msg_t m;

    memset(m.username, 0, 40);
    m.len = 140;

    set_username(&m);
    set_msg(&m);

    puts(">: Msg sent!");
}

int main() {
    puts("--------------------------------------------\n|   ~Welcome to "
         "l33t-m$n ~    v1337        "
         "|\n--------------------------------------------");
    handle_msg();

    return 0;
}
