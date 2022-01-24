beep development notes
======================

This file contains a number of notes, links and remarks which can be
useful if you need to change `beep`.

You can also run `make html` and have `pandoc` and `doxygen` generate
[HTML versions of the `*.md` files at `html/`](html/) and
[HTML documentation from the code comments at `html/dox/html/index.html`](html/dox/html/index.html).


Testing
=======

If you want to run the tests only for the executables compiled with
clang, run

```
[user@host beep]$ make check TOOLCHAINS=clang
```


APIs
====

evdev API
---------

The Linux kernel implements the input device event API in
`drivers/input/misc/pcspkr.c` with
`include/uapi/linux/input-event-codes.h` defining `EV_SND` as `0x12`
and `SND_TONE` as 0x02.

`include/uapi/linux/input.h` defines `EVIOCGSND(len)`, and there is
documentation about `EV_SND` to be found in
`Documentation/input/event-codes.rst` and
`Documentation/input/input-programming.rst`.


Console API
-----------

The Linux kernel implements the console API in
`drivers/tty/vt/vt_ioctl.c` with `include/uapi/linux/kd.h` defining
`KIOCSOUND` to be `0x4B2F`.


Fallback TTY BEL API
--------------------

If you print a ``'\a'`` (`BEL`) character to a TTY device stdout, that
can sound some type of beep. Not with the desired frequency or
duration or sequence, but at least there is a beep.


Raspberry Pi
------------

The Raspberry Pi has no PC speaker hardware, but a few GPIO pins with
PWM capability which root can access from userspace.


Non-PC systems
--------------

Non-PC systems like e.g. the Raspberry Pi and similar systems do not
have PC beeper hardware, they have GPIO pins some of which are
attached to PWM hardware which can be set to produce beeps.

So if one properly attaches a loudspeaker or piezo buzzer to such a
PWM pin (with a bunch more electrical components like capacitors,
resistors, transistors where required), one has the hardware to
produce beeps.

This leaves one to sort out the software side of it, which gets us
back to the issue that only root is allowed to touch the Raspberry
Pi GPIO pins, but `beep` should also run as a non-root user without
the potential for setuid or sudo facilitated security holes.

Options:

  * Leave beeps to the root user only who can access the GPIO pins.

    Not realistic.

  * Add a privileged `beep-daemon` to handle the beeping, and have a
    non-priviledge `beep` contact the beep daemon in some manner to do
    the actual beeping.

      * Use `AF_UNIX`/`SOCK_SEQPACKET` socket for some kind of to be
        agreed upon data structure for beeps.

        The `beep-daemon` or (better) someone like systemd who manages
        a `beep-daemon` can set up user and group access to the
        `AF_UNIX` socket.

      * Implement a userspace input device driver ("uinput")
        compatible with the `EV_SND`/`SND_TONE` interface used by
        `/dev/input/by-path/platform-pcspkr-event-spkr`.

        Then the permission setup happens through `udev` rules, and
        `beep` can just work as normal (except it needs to find out
        the device name).

        As `libevdev` only allows one to create new uinput devices
        mimicking existing devices, we need to go directly to the
        kernel uinput API here to create a specifically pcspkr-like
        uinput device.


Architecture
============

  * We use `__attribute__((constructor))` for some initialization in
    the respective `beep-driver-*.c` and `beep-log.c`.

  * If we can get rid of the old console API, we can also get rid of
    the `beep-driver*.[ch]`.  Unless we go cross-platform and want to
    keep the flexibility of supporting more than one API for one
    executable.


TODO list
=========

Post-1.4.0:

  * TODO: Go through all github.com forks of johnath/beep
  * TODO: Read up on signal(2).
  * TODO: Implement the uinput based beep-daemon for Raspberry Pi.


Release checklist
=================

  * Does `make clean && make check` run?

  * Is [`NEWS.md`](NEWS.md) up to date?

  * Does [`NEWS.md`](NEWS.md) start with the proper version
    number?

  * Does [`CREDITS.md`](CREDITS.md) contain the credits information
    for all new commits since the last release?

  * Are the copyright notices inside code files up to date?

  * Does `beep --version` contain the current year?

  * Does [`GNUmakefile`](GNUmakefile) define `PACKAGE_VERSION` as the
    proper number?

  * Does `make clean && make check` run?

  * Do the `git push` to github.

  * Wait for CI builds and checks to finish building
    successfully. Investigate and fix any problems.

  * Enjoy the [github releases web
    interface](https://github.com/spkr-beep/beep/releases):

      * Tag beep version 1.2.3 as "v1.2.3".

      * Summarize the `NEWS.md` in the release notes while properly
        formatting it using github flavoured Markdown.
