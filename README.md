# Power Linux Emulated Application Software Environment

PLEASE is a program to run ppc64 Linux binaries on PASE for IBM i, which is an
environment for running AIX binaries on IBM i. (oooh, inception!)

## Building

Some parts are built in PASE and some parts are built in Linux. The make file will build the appropriate
parts automatically:

```sh
git clone --recursive https://github.com/kadler/PLEASE.git
cd PLEASE
make
```

If you do not have a ppc64 (not ppc64le!) Linux system, you can use a
cross-compiler. This will automatically be detected and set, but you may need
to override this value depending on the platform

```sh
# eg. on Ubuntu
sudo apt install gcc-7-powerpc64-linux-gnu
make CC=powerpc64-linux-gnu-gcc-7
```

## Running

Once you've built the Linux parts (args and hello under linux/), you can
transfer them to the system where you built the PASE parts.

```
$ pase/please hello Kevin
Hello from a Linux binary, Kevin!
Returning 42
$ echo $?
42
```

```
$ pase/please args foo bar baz
There are 4 args, argv = ffffffffffffc18, args are:
argv[0] = ./args
argv[1] = foo
argv[2] = bar
argv[3] = baz

envp = ffffffffffffce8, environment variables are:
envp[0] = SHELL=/QOpenSys/pkgs/bin/bash
envp[1] = PWD=/QOpenSys/home/kadler/please
envp[2] = LOGNAME=kadler
envp[3] = TZ=<CST>6<CDT>,M3.2.0,M11.1.0
envp[4] = HOME=/QOpenSys/home/kadler
envp[5] = TERM=xterm-256color
envp[6] = USER=kadler
envp[7] = LOGIN=kadler
envp[8] = SHLVL=1
envp[9] = PATH=/QOpenSys/pkgs/bin:/QOpenSys/usr/bin:/usr/ccs/bin:/QOpenSys/usr/bin/X11:/usr/sbin:.:/usr/bin
envp[10] = MAIL=/var/spool/mail/kadler
envp[11] = _=pase/please

environ = ffffffffffffce8, environment variables are:
environ[0] = SHELL=/QOpenSys/pkgs/bin/bash
environ[1] = PWD=/QOpenSys/home/kadler/please
environ[2] = LOGNAME=kadler
environ[3] = TZ=<CST>6<CDT>,M3.2.0,M11.1.0
environ[4] = HOME=/QOpenSys/home/kadler
environ[5] = TERM=xterm-256color
environ[6] = USER=kadler
environ[7] = LOGIN=kadler
environ[8] = SHLVL=1
environ[9] = PATH=/QOpenSys/pkgs/bin:/QOpenSys/usr/bin:/usr/ccs/bin:/QOpenSys/usr/bin/X11:/usr/sbin:.:/usr/bin
environ[10] = MAIL=/var/spool/mail/kadler
environ[11] = _=pase/please
```

## How it Works

A ppc64 Linux ELF binary is opened and parsed. Any other type of ELF file will
result in an error. Once the ELF file has been validated, the segments of the
ELF file are loaded at their appropriate address and then the entry point is
jumped to, passing the arguments as appropriate.
    
Due to limitations in PASE, the text segment cannot have its permissions
changed by userspace to allow replacing it with the ELF image. Instead, the
image must be loaded in the mmap region. Thus, special Linux programs must be
built and only those will run.

Because PASE does not support syscall mapping, a special libc must be linked
with, which creates Linux-compatible function wrappers for AIX syscalls.
Currently, only `write()` and `exit()` are available. A simple, BSD-licensed
[printf implementation](https://github.com/mpredfearn/simple-printf) is used
for convenience.

## Well, Actually

I mentioned Linux and I see you reaching for the GNU/Linux copypasta, but hold
your horses there! PLEASE *does not* support applications linked to the GNU C
library, so there is no GNU code involved so it's just Linux. Well actually,
now that I think about it, since it doesn't use the Linux syscall interface,
it's not really Linux applications either. More like just PASE applications
wrapped in an ELF container instead of XCOFF...

However, it _is_ a good April Fools joke, don't you think? :wink:
