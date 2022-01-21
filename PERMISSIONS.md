Permission setup for beep
=========================

The `beep` command needs write access to the
`/dev/input/by-path/platform-pcspkr-event-spkr` device special file
which will be a symlink to the actual device special file called
something like `/dev/input/event20`.

This device special file is created when the `pcspkr.ko` PC speaker
driver kernel module is loaded. The permissions given to that device
special file can be changed by adding some `udev(7)` rules.

The `beep` package suggests the udev rules mentioned in the sections
[Users currently logged into virtual
console](#users-currently-logged-into-virtual-console) and [All other
users](#all-other-users) below are installed by distro package or
manually by the system administrator.

Then the system administrator can easily give users access to the PC
speaker device as described in the [Add users to `beep`
group](#add-users-to-beep-group) section at the bottom
of this file.


udev rules
==========


udev rule basics
----------------

Whenever a new device is added to or removed from the system, the
`udev(7)` system will apply its rules to the device.

Every rule consists of a part to match a particular type of device
(with comparison operators such as `==`), and a part which does
something about that device, such as modifying or setting one or more
variable (e.g. `+=' or `=`), which can change the device file's mode
or ownership or file ACL.

See the sections below for example rules suggested for `beep`.


Where to install udev rules
---------------------------

There are two locations where `udev(7)` rules can be installed:

  * `/usr/lib/udev/rules.d/*.rules` or `/lib/udev/rules.d/*.rules`  
    This is the distribution dependent directory where distribution
    binary packages will install their static rules which are not to
    be modified by the system administrator.

  * `/etc/udev/rules.d/*.rules`  
    This where the system administrator will install their special
    rules independent of any distribution packages.

    This location is suitable when a system administrator is
    installing `beep` from source tree, or when the system
    administrator wants to install their own rules to override the
    distro package provided ones.

    This is also where a sysadmin can override a file from
    `/usr/lib/udev/rules.d/` or `/lib/udev/rules.d/` by creating a
    file of the same name in `/etc/udev/rules.d/`. This file can
    contain different udev rules, or be empty (or a symlink to
    `/dev/null`) to just disable the rules from that file.


Users currently logged into virtual console
-------------------------------------------

Making use of modern GNU/Linux/freedesktop/etc. systems' session and
udev magic, you can give the user logged into the currently active
virtual console session access to the PC speaker by dropping the
following rule into a udev rule file like
`/usr/lib/udev/rules.d/70-pcspkr-beep.rules` or
`/lib/udev/rules.d/70-pcspkr-beep.rules` (the exact location depends
on your distribution):

```
# Give write access to the PC speaker to the user logged in on the current virtual console
ACTION=="add", SUBSYSTEM=="input", ATTRS{name}=="PC Speaker", ENV{DEVNAME}!="", TAG+="uaccess"
```

This allows `beep` to run for the user  currently logged in locally
without any user specific setup required.

However, it will _not_ allow users logged remotely (e.g. via `ssh`) or
processes running in the background to beep the PC speaker, unless the
same user happens to be logged in on the currently active virtual
console session at the time `beep` opens the device special file.


All other users
----------------

To allow `beep`ing for users logged in remotely (e.g. via ssh) or
processes running as any user other than the one logged into the
currently active virtual console session, the best way is to set up a
system user group `beep` and give that group write access to the
device special file.

The exact command to add the group varies from system to
system. Choose whatever works on your system:

```
[root@host ~]# addgroup --system beep    # Debian, Ubuntu, etc.

[root@host ~]# groupadd --system beep    # Fedora, RHEL, etc.
```

Now to allow writing to the device special file for members of the
`beep` user group, you have the choice between the following two udev
rules for `/usr/lib/udev/rules.d/90-pcspkr-beep.rules` or
`/lib/udev/rules.d/90-pcspkr-beep.rules` (the exact location depends
on your distribution).

  * This rule uses `setfacl(1)` to add an ACL entry to grant write
    access for the `beep` group without changing the standard
    user/group access granted by the default system setup:

        # Add write access to the PC speaker for the "beep" group
        ACTION=="add", SUBSYSTEM=="input", ATTRS{name}=="PC Speaker", ENV{DEVNAME}!="", RUN+="/usr/bin/setfacl -m g:beep:w '$env{DEVNAME}'"

    This ACL based rule requires installing the `acl` package on those
    distributions which do not install it by default yet.

	The nice part about just adding this one ACL is that it does not
    need to possibly interfere with whatever else the system does to
    the device, e.g. to implement the effects of `TAG+="uaccess"`.

  * The following non-ACL rule grants access to the `beep` group by
    changing the owning group, removing access for the default group
    `input`:

        # Give write access to the PC speaker only to the "beep" group
        ACTION=="add", SUBSYSTEM=="input", ATTRS{name}=="PC Speaker", ENV{DEVNAME}!="", GROUP="beep", MODE="0620"


Verifying the udev rules work
-----------------------------

You will have to (re-)load the `pcspkr.ko` module to (re-)add the
device so that the new rule is invoked:

```
[root@host ~]# modprobe -r pcspkr; sleep 2; modprobe pcspkr
[root@host ~]# _
```

Check that the device has the desired permissions with `ls` and/or
`getfacl` (`getfacl` only if you are using ACLs).

A working non-ACL setup might look something like

```
[root@host ~]# ls -lH /dev/input/by-path/platform-pcspkr-event-spkr
crw-rw----. 1 jane beep 13, 84 Apr  8 07:35 /dev/input/by-path/platform-pcspkr-event-spkr
[root@host ~]# _
```

and a working ACL setup might look something like

```
[root@host ~]# ls -lH /dev/input/by-path/platform-pcspkr-event-spkr
crw-rw----+ 1 root input 13, 84 Apr  8 07:35 /dev/input/by-path/platform-pcspkr-event-spkr
[root@host ~]# getfacl /dev/input/by-path/platform-pcspkr-event-spkr
getfacl: Removing leading '/' from absolute path names
# file: dev/input/by-path/platform-pcspkr-event-spkr
# owner: root
# group: input
user::rw-
user:jane:rw-
group::rw-
group:beep:-w-
mask::rw-
other::---

[root@host ~]# _
```


General rules for permissions setup
-----------------------------------

To keep your system secure, try to keep your permissions as
restrictive as possible, while allowing for what you want. Give access
to a single user. Give access to a special group of users.

Here are a few __bad ideas__ you should __avoid__ when you change the
permissions setup:

  * DO NOT add users to the `input` group  
    This would allow the users from the `input` group access to
    __all__ input devices.  This includes keyboards and mice which are
    none of their business (think keyloggers or reprogramming
    programmable keyboards).

  * DO NOT run `beep` setuid root or via sudo-root  
    There have been a few serious security issues for `beep` in 2018
    (CVE-2018-0492 and CVE-2018-1000532). Both had their impact when
    `beep` was run setuid root or via sudo-root.

    Therefore, `beep` now checks whether it is being run setuid root
    or via sudo-root, and if so, `beep` aborts without doing anything
    else, because that something else might turn out to be harmful.
    Also, this reminds people still using setuid-root or sudo-root
    setups to switch to the new more granular permission setup.


Add users to `beep` group
=========================

The suggested way to regulate permissions to the speaker device is to
have the system administrator add all users who should be able to run
beep to the `beep` group.

To add user `jane` to the `beep` group, the system administrator has
to run a command like

```
[root@host ~]# usermod jane -a -G beep
```

After having user `jane` log out (and after killing user `jane`'s
running `tmux` instances, killing `system --user` sessions for user
`jane`, or just plain rebooting), user `jane` can log back in and
check whether she now is a `beep` group member:

```
[jane@host ~]$ id
uid=1000(jane) gid=1000(jane) groups=1000(jane),10(wheel),942(beep) context=unconfined_u:unconfined_r:unconfined_t:s0-s0:c0.c1023
[jane@host ~]$ ./beep -f 220 -n -f 275 -n -f 330 -n -f 440 -n -f 550 -n -f 660 -n -f 880
[jane@host ~]$ _
```
