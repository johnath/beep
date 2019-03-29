Installing beep
===============

This document describes how to install `beep` from sources and how to
set up the system afterwards.

If you are using `beep` as shipped in a distribution package, the
package should have done most of those steps for you.  The notable
exception should be adding users to the `beep` group.


Compile and Install
===================

The easy way is

    $ make
    $ make install

By default, it'll put the executable `beep` in `/usr/bin`.  If you
don't like this, change the common variables as appropriate, e.g.

    $ make prefix=$HOME/.local
    $ make prefix=$HOME/.local install

or

    $ make
    $ make DESTDIR=$PWD/__installroot install

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
