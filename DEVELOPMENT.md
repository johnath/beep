beep development notes
======================

This file contains a number of notes, links and remarks which can be
useful if you need to change `beep`.


Testing
=======

If you want to run the tests only for the executables compiled with
clang, run

    $ make check COMPILERS=clang


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


Fallback TTY '\a' API
---------------------

If you print a '\a' (BEL) character to a TTY device stdout, that can
sound some type of beep. Not with the desired frequency or duration or
sequence, but at least there is a beep.


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
PWM pin (with a bunch more electrial components like capacitors,
resistors, transistors where required), one has the hardware to
produce beeps.

This leaves one to sort out the software side of it, which gets us
back to the issue that only root is allowed to touch the GPIO pins,
but `beep` should also run as a non-root user without the potential
for setuid or sudo facilitated security holes.

Options:

  * Leave beeps to the root user only who can access the GPIO pins.

  * Add a priviledge `beep-daemon` to handle the beeping, and have a
    non-priviledge `beep` contact the beep daemon in some manner to do
    the actual beeping.

      * Use `AF_UNIX`/`SOCK_SEQPACKET` socket for some kind of to be
        agreed upon data structure for beeps.

        The `beep-daemon` or (better) someone like systemd who manages
        a `beep-daemon` can set up user and group access to the
        `AF_UNIX` socket.

      * Implementing a userspace input device driver ("uinput")
        compatible with the `EV_SND`/`SND_TONE` interface used by
        `/dev/input/by-path/platform-pcspkr-event-spkr`.

        Then the permission setup happens through `udev` rules, and
        `beep` can just work as normal (except it needs to find out
        the device name).

        As `libevdev` only allows one to create new uinput devices
        mimicking existing devices, we need to go directly to the
        kernel uinput API here to create a specifically pcspkr-like
        uinput device.


TODO list
---------

  * TODO: Make sure all C code uses {} around code blocks everywhere.
  * TODO: Make beep.1 refer to README.md at the installed location.
  * TODO: Document references to Johnathan's Email or Website as historic.
  * TODO: Update CREDITS with the changes since 2010 (beep-1.3) and/or 2013 (last commit to github.com/johnath/beep)
  * TODO: Go over all documentation regarding links.


FIXME: Go over this.
