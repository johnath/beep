beep - beep the PC loudspeaker on Linux
=======================================

![Build Status](https://github.com/spkr-beep/beep/actions/workflows/beep-build.yml/badge.svg)

`beep` allows you to have the PC speaker issue beeps and beep patterns
with given frequencies, durations, and spacing on Linux systems.

Beginning in 2019, this version of `beep` can be found at
[github.com/spkr-beep/beep/](https://github.com/spkr-beep/beep/). Please
report bugs and other problems at
[github.com/spkr-beep/beep/issues](https://github.com/spkr-beep/beep/issues).

This version of `beep` has been forked from Johnathan Nightingales'
original `beep` when [CVE-2018-0492 and
CVE-2018-1000532](https://github.com/johnath/beep/issues/11) required
fixes in 2018, while Johnathan Nightingales'
[github.com/johnath/beep/](https://github.com/johnath/beep/) and
[johnath.com/beep/](http://johnath.com/beep/) was only maintained from
around 2000 until around 2013.


What beep does
--------------

  * Parse the command line arguments

  * Find a suitable device file and API to use on it.

  * Set up signal handlers to silence the beeping in case the `beep`
    process is interrupted or killed before it has a chance to silence
    the PC speaker again.

  * Depending on the command line arguments, either

      * beep in the frequencies, sequencing and timing given on the
        command line until the sequence has finished.

      * when used in a pipe, beep for every character or line passing
        through the pipe until said pipe is finished.


What beep does not do
---------------------

  * `beep` does not serialize or multiplex access to the one PC
    speaker hardware.  The sound from a `beep` process playing a very
    long note will be silenced as soon as another short `beep` process
    starts its note, and after the short `beep` finishes, the PC
    speaker will remain silent for the remainder of the long note's
    duration.

  * `beep` does not work on platforms and systems without a PC speaker
    or compatible hardware.


How beep works internally
=========================

The evdev API
-------------

The Linux evdev API uses `write(2)` to write `EV_SND`/`SND_TONE`
input_event data to the
`/dev/input/by-path/platform-pcspkr-event-spkr` device file.

The system administrator can set up normal file permissions on the
device file to allow beeping access for certain users and groups.

See [`PERMISSIONS.md`](PERMISSIONS.md) for more details on permission
setup.


The console API
---------------

In order to be allowed to run the `KIOCSOUND` `ioctl(2)` of the
classical Linux console API, the Linux kernel insists you must either
be root or own the current TTY (e.g. non-root user logged in on
`/dev/tty4`).

`beep` only uses this API as a fallback.


Documentation
=============

The documentation files shipped with the `beep` source tree, sorted
alphabetically:

  * [`COPYING`](COPYING)  
    The licensing text.

  * [`CREDITS.md`](CREDITS.md)  
    Giving credit where credit is due.

  * [`DEVELOPMENT.md`](DEVELOPMENT.md)  
    Information for developers working on improving beep.

  * [`INSTALL.md`](INSTALL.md)  
    How to build and install `beep` from source tree.

  * [`NEWS.md`](NEWS.md)  
    Contains a list of the high level changes between `beep` releases.

  * [`PACKAGING.md`](PACKAGING.md)  
    How to package `beep` for a Linux distribution.

  * [`PERMISSIONS.md`](PERMISSIONS.md)  
    How to set up the permissions to allow non-root users to run
    `beep`. Covers installing from source tree, and distro package
    following and not following the beep suggested udev rule setup.

  * [`README.md`](README.md)  
    This basic overview on `beep` including the history kept over from
    the old `README` file.


The history of beep
===================

In late 2018, while trying to come up with a fix for CVE-2018-1000532
for the Fedora package, Hans Ulrich Niedermann had to find find out
exactly how `beep` works, and found out that the API to use in this
day and age (the evdev API) was not even in available `beep`'s default
configuration, or documented with the proper device name.  So a few
bits in the code had to be changed, a lot of documentation had to be
changed, and eventually the changes had accumulated to so much that
the result could not really be called `beep-1.3` any more.

However, with the last repository activity in early 2013, Jonathan's
github beep repository had been basically unmaintained for almost 6 years,
so Hans Ulrich created https://github.com/spkr-beep/beep/ as a new home
for beep.  All contributors are welcome there, old and new.

This following part has been taken from the original `README` by
original `beep` author Johnathan Nightingale where he tells how `beep`
came about.

> I just got so tired of being limited to `printf("\a");` when I wanted a terminal
> beep.  This program isn't supposed to be anything stupendous, it's just
> supposed to get the job done.  Its intended purpose in life is to live inside
> shell/perl scripts, and allow a little more granularity than you get with the
> default terminal bell.  Maybe I'm the only one who thinks this is useful. :)
> 
> If for any reason you decide you need to, contact me:
> 
> `johnath@johnath.com`
> http://johnath.com/
> 
> And beep can generally be found at:
> 
> http://johnath.com/beep/
> 
> For installation instructions, see `INSTALL`.
> For copying and (non-)warranty information, see `COPYING`.
> For usage information, check the man page.
> 
> There is a github repository of this code at: git://github.com/johnath/beep.git
> 
> 
> Playing Songs
> -------------
> 
> A surprising number of people have sent in requests, or even patches, to help
> beep play multiple, different sounds off a single invocation.  I had always
> thought that if people wanted a more complex melody, they would just do 
> something like:
> 
>     $ cat << EOF > song.sh
>     #!/bin/sh
>     beep <first beep's options>
>     beep <second beep's options>
>     etc...
>     EOF
> 
> Nevertheless, because of repeated and vociferous demand, version 1.2 (and 
> presumably all later versions) include the -n/--new switch which allows you to
> use one command line to create multiple beeps.  Check the man page for 
> details.  I have also had a couple people suggest that I encourage the 
> development of such shell scripts/command lines, even collect the particularly
> melodious ones.  Certainly if anyone feels like sending some to me, I will put
> them somewhere visible, or even include them as a sample.  I think Dvorak's New
> World Symphony, 4th Movement, for example, would make a lovely shell script.
> I also wouldn't mind a rendition of BNL's If I had a million dollars.  But by
> all means, be creative.
> 
> All files copyright (C) Johnathan Nightingale, 2002.
> All files distributed under the GNU general public license.
