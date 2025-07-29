# level06

In this level, we're going to train some of those gdb skills.
The program asks for a username and a serial, which is calculated from the username.
We could try to reverse-engineer the algorithm and brute-force it, but I'm feeling lazy.
Instead, we'll just bypass the `ptrace` check and extract the correct serial.

Let's disassemble the `auth` function and set our breakpoints:

```bash
pwndbg> disas auth
Dump of assembler code for function auth:
...
0x080487b5 <+109>:   call   0x80485f0 <ptrace@plt>
0x080487ba <+114>:   cmp    eax,0xffffffff
...
0x08048866 <+286>:   cmp    eax,DWORD PTR [ebp-0x10]
0x08048869 <+289>:   je     0x8048872 <auth+298>
...

pwndbg> b *0x080487ba
Breakpoint 1 at 0x80487ba

pwndbg> b *0x08048869
Breakpoint 2 at 0x8048869
```

Now that we have our breakpoints, let’s run the program and input a username (e.g., `username`) and a dummy serial (e.g., `1234`):

```bash
pwndbg> r
***********************************
*               level06           *
***********************************
-> Enter Login: username
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 1234
```

When we hit the first breakpoint, we can modify the `EAX` register to bypass the `ptrace` check:

```bash
pwndbg> c
Continuing.

Breakpoint 1, 0x080487ba in auth ()

pwndbg> p/x $eax
$3 = 0xffffffff

pwndbg> set $eax=0

pwndbg> p/x $eax
$4 = 0x0
```

Next, we can check the expected serial for the username:

```bash
pwndbg> c
Continuing.

Breakpoint 2, 0x08048869 in auth ()

pwndbg> p $eax
$5 = 1234

pwndbg> p *(int*)($ebp-0x10)
$6 = 6234463
```

So, the correct serial for `username` is `6234463`.
Let’s quit *gdb* and rerun the program with the correct serial:

```bash
level06@OverRide:~$ ./level06
***********************************
*               level06           *
***********************************
-> Enter Login: username
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6234463
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
<flag>
```
