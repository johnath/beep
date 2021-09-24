Installing beep
===============

This document describes how to install `beep` from sources and how to
set up the system afterwards.

If you are using `beep` as shipped in a binary distribution package,
that package should have done most of those steps for you.  The
notable exception should be the step adding users to the `beep` group.


Build requirements
==================

  * GNU make
  * clang and/or gcc
  * Linux kernel headers


Compile and Install
===================

The easy way is

```
[user@host beep]$ make
[user@host beep]$ make install
```

By default, `make install` will put the executable `beep` in
`/usr/bin`.  If you do not like this, change the common GNU Makefile
standard variables as appropriate, e.g.

```
[user@host beep]$ make prefix=$HOME/.local
[user@host beep]$ make prefix=$HOME/.local install
```

or

```
[user@host beep]$ make
[user@host beep]$ make DESTDIR=$PWD/__installroot install
```

For the complete list of those variables, see the top of
[`GNUmakefile`](GNUmakefile).

Some more compilation related variables mostly useful for distribution
packagers are documented in [`PACKAGING.md`](PACKAGING.md).


System configuration
====================

  * Add a line

        alias platform:pcspkr pcspkr

    to a file like e.g. `/etc/modprobe.d/pcspkr-beep.conf`.

  * Set up the udev rules to set the device special file permissions
    for non-root users according to [`PERMISSIONS.md`](PERMISSIONS.md).
