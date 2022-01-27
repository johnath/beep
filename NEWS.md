beep NEWS
=========

The big and user visible changes.

1.4.12
------
  * Document how to override and disable packaged udev rules.
  * Changed the build system to be closer to the GNU makefile
    conventions, i.e. only one compiler/toolchain per build, default
    installation is prefix=/usr/local instead of prefix=/usr, use GNU
    style docdir= instead of RPM spec file style pkgdocdir=, etc.
    Still not using a `configure` script, though, as that is complex.
    The user writing a `local.mk` with some definitions covers
    everyone wanting to use the same settings across `make`
    invocations, though.

1.4.11
------
  * Install the `contrib/morse/*` scripts just like the failure-beeps.

1.4.10
------
  * Rename `CREDITS` to `CREDITS.md` and `CHANGELOG` to `NEWS.md` and
    reformatted them to markdown.
  * Fix some `__VA_ARGS__` problems with some C language compiler
    options.
  * `contrib/morse/`: Add both `perl` and `tr`+`sed` script options to
    have beep produce morse code by converting the output of the
    `morse` utility from the BSD Games package to a `beep` command
    line.
  * If built without any drivers, running `beep` will now complain and
    abort instead of silently (pun intended) just not beeping.
  * Stop using Travis CI. Use GitHub Actions workflows for CI instead.
  * Skip test cases using PC speaker hardware if absent (e.g. on a VM)
  * Improved documentation (e.g. alsamixer requirement, loading pcspkr)

1.4.9
-----
  * Mention default values in `beep(1)` man page and `beep --help` output.

1.4.8
-----
  * At build time, avoid error messages related to maybe building without
    git installed, or from a tarball instead of a git source tree.
  * Improve `issue-6-benchmark` report to help people with solving
    [issue #6 (evdev open/close is slooow)](https://github.com/spkr-beep/beep/issues/6).

1.4.7
-----
  * Install contrib scripts for both successfully and failing sounding
    beep note sequences.
  * Install man page uncompressed, so packagers can use their favoured
    compression format.

1.4.6
-----
  * Let user define `CFLAGS_*` at `make` time while internal appends
    still work

1.4.5
-----
  * Use `va_copy()` macro in function with `va_list` parameter to
    avoid possible bug
  * Use `BEEP_LOG_LEVEL` environment variable for default log level

1.4.4
-----
  * Remove `udev/rules.d/` and `modprobe.d/` example files to force
    packagers to re-read `PACKAGING.md` and `PERMISSIONS.md`
  * Rewritten `PERMISSIONS.md` and `INSTALL.md`, adapting `README.md`
    and `PACKAGING.md` and `beep.1` for consistency
  * Allow beep to run as non-root, even if `SUDO_*` env variables are
    present
  * Fix newline escaping when generating `beep-usage.c`
  * By default, use plain `-g` instead of `-gstabs`
  * Ensure the gcc compiler used actually supports the default flags
    in `CFLAGS_gcc`

1.4.3
-----
  * Only use `-fcf-protection` `CFLAGS_gcc` if actually supported on
    this platform

1.4.2
-----
  * Improved `beep(1)` man page and `--help` output
  * Cleaned signal handlers of all function calls
  * Use beep driver infrastructure (`console` and `evdev` drivers)
  * Abort on unhandled command line parameters
  * Use `nanosleep(2)` instead of `usleep(3)`

1.4.1
-----
  * Safer signal handlers (`safe_error_exit()` without global
    variables).
  * Reduce accepted range of input numbers. 5 minute beeps should
    still be long enough.

1.4.0
-----
  * Fix CVE-2018-1000532 External Control of File Name or Path
    vulnerability in `--device` option
  * Fix CVE-2018-0492 race condition that allows local privilege
    escalation
  * Make `/dev/input/by-path/platform-pcspkr-event-spkr` the default
    device to use as the system administrator can allow access to that
    without needing any privilege escalation risks via setuid or
    `sudo`.
  * Adapt `--help` output, `beep(1)` man page, `README.md`,
    `INSTALL.md` to reflect the new device use.
  * Add basic suite of tests.
  * Constrained a few integers to avoid integer overflows.
  * Only issue fallback `\a` type beeps if that `\a` actually goes to
    a tty device which can actually beep
  * Stop promoting floating point frequencies which no Kernel API can
    even use

1.3
---
  * 8 years have passed!
  * Integrated a bunch of Gerfried Fuchs' changes maintained for the
    debian version for years and years
  * Added him to the `CREDITS`, too.
  * Support for devfs, and alternate console devices.
  * Warnings about multiply-specified frequency
  * Debug mode

1.2.2
-----
  * Man pages now `gzip -9` for better compression
  * Table of frequencies added to man page
  * Fix for platforms with unsigned chars
  * On `ioctl()` errors, beep will now do a `printf("\a")` so that, at
    very least, you get a beep.  :)


1.2.1
-----
  * fixed segfault when handling long options

1.2.0
-----
  * added `-n`/`--new` support - so beep FINALLY handles multiple
    beeps on a single command line.
  * `-f` now takes decimal frequencies, not just whole numbers.

1.0.2
-----
  * Added more common `-V` option, as companion to `-v` and
    `--version`
  * `README` now addresses the question of multiple beeps, and
    composing music with beep.

1.0.1
-----
  * Fixed some outdated comments in the source, and a typo in the man
    page.

1.0.0
-----
  * added a `SIGINT` handler, so Ctrl-C no longer leaves the speaker
    wailing forever
  * added `-v`/`--version` and `-h`/`--help` support
  * updated man page
  * this release is changed from 0.8 to 1.0.0, it's worthy of it.
    Feature complete and no outstanding bugs that I know of.

0.6.1
-----
  * changed man page to gzip format (instead of bzip2)
  * updated man page and `README` to explain ioctl problems


0.6
---
  * added `-D` option.  Both `-d` and `-D` set inter-repetition delay,
    but `-D` instructs beep to delay even after the last beep, where
    `-d` delays only between beeps, and terminates immediately after
    last beep.
  * incorporated Rick Franchuk's idea of stdin hooks - dear god `-c`
    is annoying.
  * added a man page


0.5
---
  * changed over from manually parsing command line parms to `getopt()`
  * changed `atoi()` calls into `sscanf()` calls, to get more
    meaningful error handling

0.4
---
  * first usable
  * initial options supported: `-f`, `-l`, `-d`, `-r`
