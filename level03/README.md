# level03

This level is actually really easy once we have the [source code](./source.c).

The program asks for a `uint32_t` password, but the value is actually subtracted from a constant (`322424845`).
If the result is greater than 21, a random value is generated to replace it.

That result is then used as the rotation amount for a decryption function.
Our goal is to find the value that, when used, decrypts the string ``Q}|u\`sfg~sf{}|a3`` into `Congratulations!`.

If we succeed, we’ll get a shell with **level04** permissions.

Let’s use a one-liner to brute-force all possible values:

```bash
for ((i=0; i <= 21; i++)); do val=$((322424845 - i)); echo -e "$val" | ./level03 | grep -q "Invalid Password" || echo "Valid attempt: i=$i (val=$val)"; done
```

18 seems to be the correct value — so we input **322424827** (since `322424845 - 18 = 322424827`) as the password.

```bash
level03@OverRide:~$ ./level03
***********************************
*               level03         **
***********************************
Password:322424827
$ cat /home/users/level04/.pass
<flag>
```
