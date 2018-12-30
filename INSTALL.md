Installing beep
===============

This document describes how to install `beep` from sources and how to
set up the system afterwards.

If you are using `beep` as shipped in a distribution package, the
package should have done most of those steps for you.  The one
exception should be adding users to the `beep` group.


Compile and Install
===================

The easy way is

    $ make
    $ make install

By default, it'll put the executable `beep` in `/usr/bin`.  If you
don't like this, change the common variables as appropriate:

    $ make prefix=$HOME/.local
    $ make prefix=$HOME/.local install

or

    $ make
    $ make DESTDIR=$PWD/__installroot install

For the complete list of those variables, see the top of `GNUmakefile`.


System configuration
====================

  * Add a line

        alias platform:pcspkr pcspkr

    to a file like e.g. `/etc/modprobe.d/pcspkr-beep.conf`.

	After udev rule setup, run `modprobe pcspkr.ko` to load the driver
    without a reboot, creating the
    `/dev/input/by-path/platform-pcspkr-event-spkr` evdev device with
    the desired permissions.


Device permission setup to allow non-root users to beep
=======================================================

If you want to give all users in the `wheel` group write access to the
input event device `/dev/input/by-path/platform-pcspkr-event-spkr`,
add a `udev` rule like this to `/etc/udev/rules.d/90-pcspkr-beep.rules`:

    # Give write access to the PC speaker to "wheel" group so they can run "beep"
    ACTION=="add", SUBSYSTEM=="input", ATTRS{name}=="PC Speaker", ENV{DEVNAME}!="", RUN+="/usr/bin/setfacl -m g:wheel:w '$env{DEVNAME}'"

For non-`wheel` users, set up a new system group `beep`:

    [root@host ~]# groupadd -r beep
    [root@host ~]# 

Add a `udev` rule to `/etc/udev/rules.d/90-pcspkr-beep.rules` which
allows the `beep` group write access to the input event device
`/dev/input/by-path/platform-pcspkr-event-spkr`:

    # Give write access to the PC speaker to "beep" group so they can run "beep"
    ACTION=="add", SUBSYSTEM=="input", ATTRS{name}=="PC Speaker", ENV{DEVNAME}!="", RUN+="/usr/bin/setfacl -m g:beep:w '$env{DEVNAME}'"

You might have to unload and reload the `pcspkr.ko´ module to re-add
the device so that the new rule is invoked:

    [root@host ~]# modprobe -r pcspkr; sleep 2; modprobe pcspkr
    [root@host ~]# 

Check that the device has the desired permissions with

    [root@host ~]# ls -lH /dev/input/by-path/platform-pcspkr-event-spkr
    crw-rw----+ 1 root input 13, 84 29. Dez 07:35 /dev/input/by-path/platform-pcspkr-event-spkr
    [root@host ~]# getfacl /dev/input/by-path/platform-pcspkr-event-spkr
    getfacl: Removing leading '/' from absolute path names
    # file: dev/input/by-path/platform-pcspkr-event-spkr
    # owner: root
    # group: input
    user::rw-
    group::rw-
    group:beep:-w-
    group:wheel:-w-
    mask::rw-
    other::---

    [root@host ~]# 

Now add the users who are allowed to use beep to the `beep` group:

    [root@host ~]# usermod jane -a -G beep
    [root@host ~]# 

After having user `jane` log out (and after killing user `jane`'s
running `tmux` instances, killing `system --user` sessions for user
`jane`, or just plain rebooting), user `jane` can log back in and
check whether she now a `beep` group member:

    [jane@host ~]$ id
    uid=1000(jane) gid=1000(jane) groups=1000(jane),10(wheel),942(beep) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
    [jane@host ~]$ ./beep -f 220 -n -f 275 -n -f 330 -n -f 440 -n -f 550 -n -f 660 -n -f 880
    [jane@host ~]$ 


