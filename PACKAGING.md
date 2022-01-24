Packaging beep for Linux distributions
======================================

This file contains a number of ideas and suggestions and references to
other documentation which might come in helpful in case you are
packaging `beep` for a Linux distribution.

In addition, to a packager of `beep`, [`INSTALL.md`](INSTALL.md) might
also be interesting.


Building and installing
-----------------------

The building and installing during a package build can be achieved with

```sh
make
make install DESTDIR=/path/to/package-root CC=false
```

While not strictly necessary, the `CC=false` makes sure that no
compilation happens during the `make install` phase.

You can also add to the default compiler flags by setting or adding to
`CFLAGS`, `CPPFLAGS`, `LDFLAGS`, and `LIBS`, by running `make` like
e.g.

```sh
make CFLAGS="-O -g -flto=auto -ffat-lto-objects" CPPFLAGS="-I/opt/include" LDFLAGS="-f" LIBS="-L/opt/lib"
```

The buildsystem will use whatever compiler `CC` is set to. `clang` and
`gcc` are known to work, other toolchains might.

```sh
make CC=clang
```

If you need to set any of the `*dir` variables like `prefix` or
`docdir` on the `make` command line, you need to set them for both the
build step (`make`) and for the install step (`make install`).  You
might either give the very same command line for every invocation of
`make`, or you can write those definitions into a make include file
called `local.mk` once. For example, you might want to achieve the
effect of

```sh
make prefix='/usr' docdir='$(docdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)'
make prefix='/usr' docdir='$(docdir)/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)' DESTDIR=/path/to/package-root install
```

by creating a `local.mk` which might look as follows in e.g. an RPM
spec file:

```sh
cat>local.mk<<EOF
prefix = %{_prefix}
docdir = %{_pkgdocdir}
EOF
make
make install DESTDIR='%{buildroot}'
```

or in a Debian `debian/rules.mk` file (something along these lines,
untested):

```
override_dh_auto_configure:
	echo 'CC = $(CC)' > local.mk
	echo 'prefix = /usr' >> local.mk
	echo 'CFLAGS = $(shell dpkg-buildflags --get CFLAGS)' >> local.mk
```

Files to install for beep
-------------------------

### User facing files

  * `/usr/bin/beep`  
    The `beep` executable.

  * `/usr/share/man/man1/beep.1`  

    Man page for the `beep` executable.

    If your distribution package ships a special `README.Distro` file
    (such as `README.Debian` or `README.fedora`), you might want to
    add a reference to that file in the man page's __SEE ALSO__
    section instead of the commented out placeholder reference to
    `README.Distro`.

### Documentation files

  * `/usr/share/doc/beep/README.md`  
    General description of `beep`.

  * `/usr/share/doc/beep/PERMISSIONS.md`  
    Gives package users and system administrators the information they
    need to successfully set up device permissions for non-root users.

    Covers installing from source tree, and distro package following
    and not following the beep suggested udev rule setup.

  * `/usr/share/doc/beep/NEWS.md`  

  * `/usr/share/doc/beep/COPYING`  

  * `/usr/share/doc/beep/CREDITS.md`  

The files [`INSTALL.md`](INSTALL.md),
[`DEVELOPMENT.md`](DEVELOPMENT.md), [`PACKAGING.md`](PACKAGING.md) are
not useful for binary packages.  They only make sense for source
trees.


### System setup including permissions

  * `/etc/modprobe.d/pcspkr-beep.conf`  
    When the system administrator uncomments the `alias` line, this
    tells the system to load the `pcspkr.ko` kernel module:

        alias platform:pcspkr pcspkr

    Without `pcspkr.ko` loaded, there will be no sound from the PC
    speaker at all.

    Use, change, replace as is fitting for your distribution.

  * `/usr/lib/udev/rules.d/70-pcspkr-beep.rules` or `/lib/udev/rules.d/70-pcspkr-beep.rules` (location depends on distribution)  

    If you want to use the suggested permission setup, ship this
    file. For the contents, cf. [`PERMISSIONS.md`](PERMISSIONS.md).

    Use, change, replace as is fitting for your distribution.

  * `/usr/lib/udev/rules.d/90-pcspkr-beep.rules` or `/lib/udev/rules.d/90-pcspkr-beep.rules` (location depends on distribution)  

    If you want to use the suggested permission setup, ship this
    file. For the contents, cf. [`PERMISSIONS.md`](PERMISSIONS.md).

    Use, change, replace as is fitting for your distribution.

  * Shipping the two permission approaches (`TAG+="uaccess"` and the
    group based one) in separate udev rule files allows the sysadmin
    to override or completely disable either one of those rules by
    placing a file of the same name into `/etc/udev/rules.d/`, which
    does not require changing things in `/usr` which might be a
    readonly filesystem or be overwritten by the next update of the
    beep packge.

  * Add a `beep` system user group  

    If you want to use the suggested permission setup
    (cf. [`PERMISSIONS.md`](PERMISSIONS.md)), have the package run
    something like e.g.

        # Fedora, RHEL, etc.
        getent group beep >/dev/null || groupadd --system beep

        # Debian, Ubuntu, etc.
        getent group beep >/dev/null || addgroup --system beep

    inside the package installation scripts.

  * `/usr/share/doc/beep/PERMISSIONS.md`  

    The [`PERMISSIONS.md`](PERMISSIONS.md) file describes to system
    administrators how to give users permissions to actually beep the
    PC speaker, both with the suggested udev rules, and generally.

    If your distribution ships a different set of udev rules, you
    should describe those in some distribution specific README file
    (like e.g. `README.Debian` or `README.fedora`).

All files installed into `/etc` are config files which the system
administrator may change and expect to not be overwritten with
package updates.

The sysadmin may add their own udev rules to `/etc/udev/rules.d/`
which should not be touched by the `beep` package.


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

The following is a list of links to some distributions' packages of
`beep`:

  * [archlinux](https://www.archlinux.org/)  
    [overview](https://www.archlinux.org/packages/extra/x86_64/beep/)
    [sources](https://git.archlinux.org/svntogit/packages.git/tree/trunk?h=packages/beep)
    [bugs](https://bugs.archlinux.org/?project=1&string=beep)

  * [Debian](https://www.debian.org/)  
    [overview](https://packages.debian.org/search?keywords=beep)
    [tracker](https://tracker.debian.org/pkg/beep)
    [sources](https://sources.debian.org/src/beep/)
    [bugs](https://bugs.debian.org/cgi-bin/pkgreport.cgi?package=beep)
    [unstable bugs](https://bugs.debian.org/cgi-bin/pkgreport.cgi?dist=unstable;package=beep)

  * [Fedora](https://getfedora.org/)  
    [overview](https://apps.fedoraproject.org/packages/beep)
    [bugs](https://apps.fedoraproject.org/packages/beep/bugs/)
    [sources](https://src.fedoraproject.org/rpms/beep)

  * [Gentoo](https://www.gentoo.org/)  
    [overview](https://packages.gentoo.org/packages/app-misc/beep)
    [sources](https://gitweb.gentoo.org/repo/gentoo.git/tree/app-misc/beep)
    [bugs](https://bugs.gentoo.org/buglist.cgi?quicksearch=app-misc%2Fbeep)

  * [OpenWRT](https://openwrt.org/)  
    [overview](https://openwrt.org/packages/pkgdata/beep)
    [bugs](https://github.com/openwrt/packages/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+beep)
    [sources](https://github.com/openwrt/packages/tree/master/utils/beep)
    [sources openwrt-18.06](https://github.com/openwrt/packages/tree/openwrt-18.06/utils/beep)

  * [openSUSE](https://www.opensuse.org/)  
    [overview](https://software.opensuse.org/package/beep)
    [overview](https://build.opensuse.org/package/show/home%3AChristianMauderer%3Aopensuse-installations/beep)

  * [Raspbian](https://www.raspbian.org/)  
    Sources: See Debian.

  * [Ubuntu](https://www.ubuntu.com/)  
    http://packages.ubuntu.com/src:beep
    Sources: See Debian.

More information on miscellaneous distributions' `beep` packages can be found
at the [repology.org beep page](https://repology.org/project/beep/packages).

[![Packaging status](https://repology.org/badge/vertical-allrepos/beep.svg?columns=2&minversion=1.4.2)](https://repology.org/project/beep/versions)
