Packaging beep for Linux distributions
======================================

This file contains a number of ideas and suggestions which might come
in helpful in case you are packaging `beep` for a Linux distribution.


Building and installing
-----------------------

The building and installing during a package build can be achieved with

    make
    make DESTDIR=/path/to/package-root install

If you want to replace the compiler flags, run `make` like e.g.

    make CFLAGS_gcc="-O -gstabs,foo" CFLAGS_clang=""

You can also keep `beep`'s default flags and just add your own with
`CFLAGS`. The same principle applies for `CPPFLAGS`, `LDFLAGS`, and
`LIBS`.

If the system you are building on has both `gcc` and `clang`, the
`beep` buildsystem will compile with both by default and choose the
first one from its COMPILERS make variable, which you can override
from the command line.

    make COMPILERS="clang gcc"

Or you can specifically disable one of the compilers to only build one
variant:

    make COMPILER_gcc=no

If you need to set any of the `*dir` variables like `mandir` on the
`make` command line, please set them both for the build step (`make`)
and the install step (`make install`). For example, you might want to

    make pkgdocdir='$(docdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)'
    make pkgdocdir='$(docdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)' DESTDIR=/path/to/package-root install


Files to install for beep
-------------------------

### User facing files

  * `/usr/bin/beep`  
    The `beep` executable.

  * `/usr/share/man/man1/beep1.gz`  
    Man page for the `beep` executable.


### Documentation files

  * `/usr/share/doc/beep/README.md`  

  * `/usr/share/doc/beep/PERMISSIONS.md`  
    Change or replace this file as necessary for your distribution to
    give package users and system administrators the information
    they need to successfully set up device permissions.

  * `/usr/share/doc/beep/CHANGELOG`  

  * `/usr/share/doc/beep/COPYING`  

  * `/usr/share/doc/beep/CREDITS`  

The files `INSTALL.md`, `DEVELOPMENT.md`, `PACKAGING.md` are not
useful for binary packages.  They make most sense for source trees.


### System setup including permissions

  * `/etc/modprobe.d/pcspkr-beep.conf`  
    When the system administrator uncomments the `alias` line, this
    tells the system to load the `pcspkr.ko` kernel module.

    Without `pcspkr.ko`, there will be no sound from the PC speaker
    at all.

    Use, change, replace as is fitting for your distribution.

If you want to use the default permission setup with a `beep` group
and the system administrator adding users to the `beep` group, install
and do the following:

  * `/etc/udev/rules.d/90-pcspkr-beep.rules`  
    Sets up write access to the PC speaker device for group `beep` or
    whatever else the system administrator has set it up to do.

    Requires the `pcspkr.ko` kernel module to be loaded.

    Use, change, replace as is fitting for your distribution.

  * Upon installing the package, a `beep` user group needs to be
    created with something like

        # getent group beep >/dev/null || groupadd -r beep

    inside the package installation scripts.

  * `/usr/share/doc/beep/PERMISSIONS.md`  
    This describes to system administrators how to add
    users to the `beep` group and to users how to check
    they actually are members of the `beep` group.

    Use, change, replace as is fitting for your distribution.

All files installed into `/etc` are config files which the system
administrator may change and expect to not be overwritten with
package updates.


Architectures
=============

PC platforms
------------

`beep` was written for the x86 platform, and works on both 32bit
and 64bit x86 platforms.


Non-PC platforms implementing the `EV_SND`/`SND_TONE` interface
---------------------------------------------------------------

From a cursory glance through the Linux kernel source code, there
appear to be a number of devices implementing the same
`EV_SND`/`SND_TONE` evdev interface `pcspkr.ko` implements, like
e.g. `m68kspkr` or `sparcspkr`.

It would be interesting to know on which non-PC platforms `beep`
actually makes sense.  The S/390 e.g. probably does not belong to
the systems where beeper hardware is to be expected.


Platforms without `pcspkr.ko` or other `EV_SND`/`SND_TONE` devices
------------------------------------------------------------------

For platforms *without* a PC speaker, packaging beep only makes sense
if there is both hardware support for beeping, and software support
for that hardware.

At this time, `beep` provides no software support for any of those
platforms.


Packages
========

This is a list of links to distribution packages:

  * [archlinux](https://www.archlinux.org/)  
    https://www.archlinux.org/packages/extra/x86_64/beep/
    https://git.archlinux.org/svntogit/packages.git/tree/trunk?h=packages/beep

  * [Debian](https://www.debian.org/)  
    https://sources.debian.org/src/beep/

  * [Fedora](https://getfedora.org/)  
    https://src.fedoraproject.org/rpms/beep

  * [Gentoo](https://www.gentoo.org/)  
    https://packages.gentoo.org/packages/app-misc/beep
    https://gitweb.gentoo.org/repo/gentoo.git/tree/app-misc/beep

  * [OpenWRT](https://openwrt.org/)  
    https://github.com/openwrt/packages/tree/master/utils/beep

  * [openSUSE](https://www.opensuse.org/)  
    https://software.opensuse.org/package/beep
    https://build.opensuse.org/package/show/home%3AChristianMauderer%3Aopensuse-installations/beep

  * [Raspbian](https://www.raspbian.org/)  
    Sources: See Debian.

  * [Ubuntu](https://www.ubuntu.com/)  
    http://packages.ubuntu.com/src:beep
    Sources: See Debian.
