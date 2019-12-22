/** \file issue-6-benchmark.c
 * \brief automatic open(2) benchmark for issue #6
 * \author Copyright (C) 2019 Hans Ulrich Niedermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * \defgroup issue-6-benchmark The issue-6-benchmark program
 *
 * Usage: See print_usage() function below.
 *
 * @{
 *
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "beep-compiler.h"


/**
 * Print usage message.
 */
static
void print_usage(FILE *file, const char *const argv0)
{
    fprintf(file,
            "issue-6-benchmark - benchmark program for open(2) speed (github issue #6)\n"
            "Benchmark open(2)-and-close(2) cycles work console versus evdev API.\n"
            "\n"
            "Usage:\n"
            "\n"
            "    %s\n"
            "        Autodetect a writable /dev/ttyN for the console API to use,\n"
            "        then run the benchmark for the console API and the evdev API.\n"
            "\n"
            "    %s /dev/ttyN\n"
            "        Run benchmarks using the given /dev/ttyN for the console API,\n"
            "        and using the wellknown device name for the evdev API.\n"
            "\n"
            "    %s 2342 /dev/chardevice\n"
            "        Repeat 2342 open-and-close cycles for the given /dev/chardevice.\n"
            "        Timing this is left to the caller.\n"
            "\n"
            "See also the https://github.com/spkr-beep/beep/issues/6 discussion.\n"
            ,
            argv0,
            argv0,
            argv0
            );
}


/**
 * Repeatedly open(2) (O_WRONLY) and close(2) a given device.
 *
 * Repeatedly open(2)s with O_WRONLY and close(2)s a given device for
 * a given number of times.  Counts the number of times the open(2)
 * call succeeds, so that the compiler cannot do any shortcut
 * optimizations for this loop.
 *
 * @param repeats  The number of times to repeat the open(2)-and-close(2) sequence.
 * @param device   The name of the device file.
 *
 * @return EXIT_FAILURE if some error occured
 * @return EXIT_SUCCESS otherwise
 */

static
int run_cycles(const unsigned long repeats,
               const char *const device)
    __attribute__(( nonnull(2), warn_unused_result ));

static
int run_cycles(const unsigned long repeats,
               const char *const device)
{
    unsigned long counters[2] = {0, 0};
    for (unsigned long u=repeats; u>0; u--) {
        const int fd = open(device, O_WRONLY);
        counters[(fd >= 0)]++;
        close(fd);
    }

    printf("    device_name    = %s\n"
           "    counters[succ] = %lu\n"
           "    counters[fail] = %lu\n",
           device,
           counters[true],
           counters[false]);

    if (counters[false] > 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}


/**
 * Time run_cycles() for given repeats on given device.
 *
 * @param repeats    Number of repeats to time.
 * @param device_str Device to time the open-and-close cycles of.
 *
 * @return Negative value in case of any errors
 * @return Duration in seconds otherwise
 */

static
double time_cycles(const unsigned long repeats,
                   const char *const device_str)
    __attribute__(( nonnull(2), warn_unused_result ));

static
double time_cycles(const unsigned long repeats,
                   const char *const device_str)
{
    struct timespec time_begin;
    struct timespec time_end;
    const int gettime_ret_begin = clock_gettime(CLOCK_BOOTTIME, &time_begin);
    const int retval = run_cycles(repeats, device_str);
    const int gettime_ret_end   = clock_gettime(CLOCK_BOOTTIME, &time_end);
    if (gettime_ret_begin != 0 || gettime_ret_end != 0) {
        fprintf(stderr, "error running clock_gettime\n");
        return -1.0;
    }
    if (retval == EXIT_FAILURE) {
        fprintf(stderr, "Aborting due to error(s) in run_cycles()\n");
        return -1.0;
    }
    const double d_begin = time_begin.tv_sec + 0.000000001 * time_begin.tv_nsec;
    const double d_end   = time_end.tv_sec   + 0.000000001 * time_end.tv_nsec;
    const double d_delta = d_end - d_begin;
    return d_delta;
}


/** The minimum measurement period considered to create a reliable result. */
#define MINIMUM_RELIABLE_PERIOD (15.0)


/**
 * Get reliable cycle time by running time_cycles() for longer than #MINIMUM_RELIABLE_PERIOD.
 *
 * This increases the number of repeats until it certainly takes more
 * than #MINIMUM_RELIABLE_PERIOD seconds on the wall clock. Then we
 * consider the measurement reliable enough and can return the average
 * measured cycle time.
 *
 * @param initial_repeats Initial number of repeats, which will then
 *                        used as a baseline for increasing the
 *                        repeats such that they will take more than
 *                        #MINIMUM_RELIABLE_PERIOD seconds.
 *
 * @param device_str Name of the device to measure the
 *                   open(2)-and-close(2) cycle duration on.
 *
 * @return negative value if some error occurred
 * @return the duration of a single open(2)-and-close(2) cycle otherwise
 *
 */

static
double average_cycle_time(const unsigned long initial_repeats,
                          const char *const device_str)
    __attribute__(( nonnull(2), warn_unused_result ));

static
double average_cycle_time(const unsigned long initial_repeats,
                          const char *const device_str)
{
    struct timespec time_test;
    const int gettime_ret_test = clock_gettime(CLOCK_BOOTTIME, &time_test);
    if (gettime_ret_test != 0) {
        perror("clock_gettime(CLOCK_BOOTTIME, ...)");
        return -1.0;
    }

    double d_delta = 0.0;
    unsigned long repeats = initial_repeats;
    while (true) {
        if (repeats < initial_repeats) {
            return -1.0;
        }

        printf("  Trying %lu repeats for device %s\n", repeats, device_str);

        d_delta = time_cycles(repeats, device_str);
        if (d_delta < 0) {
            fprintf(stderr, "Error in time_cycles()\n");
            return -1.0;
        }
        printf("  Time spent: %g s\n", d_delta);
        if (d_delta >= MINIMUM_RELIABLE_PERIOD) {
            return (d_delta / ((double)repeats));
        }

        /* product of positive numbers must be positive */
        const double add_repeats =
            1.10 *
            ((double)repeats) *
            (((double)(MINIMUM_RELIABLE_PERIOD))/d_delta);

        const unsigned long rounded_add_repeats =
            (unsigned long)lrint(0.5+add_repeats);

        repeats += rounded_add_repeats;
    }
}


/**
 * Print and execute external '/usr/bin/time' command if we know how.
 *
 * @param argv0      The argv0 which issue-6-benchmark has been and will be called
 * @param cycle_time The average cycle time for the given `device`
 * @param device     The device to run cycles on.
 */
static
void conditional_command(const char *const argv0,
                         const double cycle_time,
                         const char *const device)
    __attribute__(( nonnull(1) ));

static
void conditional_command(const char *const argv0,
                         const double cycle_time,
                         const char *const device)
{
    if (device == NULL) {
        /* skip this */
        return;
    }
    if (cycle_time < 0.0) {
        /* skip this */
        return;
    }

    const double reliable_repeats = 1.10 *
        ((double)MINIMUM_RELIABLE_PERIOD)/cycle_time;
    unsigned long cmd_repeats = (unsigned long)lrint(0.5+reliable_repeats);
    printf("  /usr/bin/time -v %s %lu %s\n", argv0, cmd_repeats, device);
}


/**
 * Well-known name for the evdev device.
 */
static
const char *const evdev_device_str = "/dev/input/by-path/platform-pcspkr-event-spkr";


/**
 * Run the benchmark and print a report about the results.
 *
 * We start with a reasonable guess for the initial repeats, and then
 * let the benchmark do a reliable measurement of the average cycle
 * time.
 *
 * @param argv0              The argv[0] from main().
 *
 * @param console_device_str The console device to test. If NULL, skip
 *                           benchmarking the console API device.
 */
static
int benchmark_and_report(const char *const argv0,
                         const char *const console_device_str)
{
    printf("Running benchmarks:\n");

    double cycle_time_console = -1.0;
    if (console_device_str) {
        cycle_time_console =
            average_cycle_time(1000000, console_device_str);
    }

    const double cycle_time_evdev   =
        average_cycle_time(200, evdev_device_str);

    printf("\n"
           "Summary:\n");

    if (console_device_str && (cycle_time_console > 0.0)) {
        printf("    console device: %s\n"
               "        time per open(2)-and-close(2): %g us\n"
               "        open(2)-and-close(2) rate:     %g / s\n",
               console_device_str,
               1000000.0*cycle_time_console, 1.0/cycle_time_console
               );
    } else {
        printf("    console device: no writable TTY device found\n");
    }

    if (cycle_time_evdev > 0.0) {
        printf("    evdev device:   %s\n"
               "        time per open(2)-and-close(2): %g ms\n"
               "        open(2)-and-close(2) rate:     %g / s\n",
               evdev_device_str,
               1000.0*cycle_time_evdev, 1.0/cycle_time_evdev
               );
    }

    if ((cycle_time_evdev > 0.0) && (cycle_time_console > 0.0)) {
        printf("\n"
               "So opening an evdev device takes %g times as long as opening\n"
               "a console device.\n",
               cycle_time_evdev/cycle_time_console);
    }

    printf("\n"
           "Note that any of those numbers can only be relied on when run on an\n"
           "otherwise idle machine.\n");

    if ((cycle_time_console > 0.0) || (cycle_time_evdev > 0.0)) {
        printf("\n"
               "For more details, you can run the following commands:\n");

        conditional_command(argv0, cycle_time_console, console_device_str);
        conditional_command(argv0, cycle_time_evdev, evdev_device_str);
    }

    return EXIT_SUCCESS;
}


/**
 * Take a look at all /dev/ttyN devices and find a writable console tty device.
 *
 * @return Name of a writeable /dev/ttyN virtual console device.
 * @return NULL if no writable device has been found, or if an error occured.
 */

static
char *find_writable_tty(void)
    __attribute__(( malloc ));

static
char *find_writable_tty(void)
{
    DIR *dir = opendir("/dev");
    if (dir == NULL) {
        perror("opendir /dev");
        return NULL;
    }

    while (true) {
        errno = 0;
        const struct dirent *de = readdir(dir);
        if (de == NULL) {
            if (errno == 0) { /* last entry */
                fprintf(stderr, "no matching /dev directory entry found\n");
                closedir(dir);
                return NULL;
            } else { /* readdir(3) error */
                perror("readdir(3)");
                closedir(dir);
                return NULL;
            }
        } else {
            if (strncmp("tty", de->d_name, 3) != 0) {
                /* name does not start with "tty" */
                continue;
            }

            char *ttydev_endptr;
            const unsigned long ttydev_num =
                strtoul(&de->d_name[3], &ttydev_endptr, 10);
            if (ttydev_endptr == &de->d_name[3]) {
                /* no digits found at all */
                continue;
            }
            if (*ttydev_endptr != '\0') {
                /* non-digit characters after last digit found */
                continue;
            }
            if (ttydev_num == ULONG_MAX) {
                /* number range error */
                continue;
            }
            /* found valid device name */

            const size_t ttydev_name_sz = strlen("/dev/") + strlen(de->d_name) + 1 + 1;
            char *ttydev_name = calloc(ttydev_name_sz, 1);
            snprintf(ttydev_name, ttydev_name_sz, "/dev/%s", de->d_name);
            /* full path to tty device name */

            struct stat sb;
            const int ret_stat = stat(ttydev_name, &sb);
            if (ret_stat != 0) {
                perror("stat(ttydev_name)");
                closedir(dir);
                return NULL;
            }
            if ((sb.st_mode & S_IFMT) != S_IFCHR) {
                /* not a char device */
                continue;
            }

            const int fd = open(ttydev_name, O_WRONLY);
            if (fd < 0) {
                /* cannot open(2) this device */
                continue;
            }
            close(fd);

            closedir(dir);
            return ttydev_name;
        }
    }
}


/**
 * Run open-and-close-benchmark of console API (using the first writable /dev/ttyN) versus evdev API (0 cmdline arguments).
 *
 * @param argc Length of the `argv` string array.
 * @param argv Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
static
int main1(const int argc, const char *const argv[])
{
    assert(argc == 1);
    char *console_device = find_writable_tty();
    if (console_device) {
        printf("Writable TTY device found: %s\n", console_device);
    } else {
        printf("No writable TTY device found.\n");
    }

    const int retval = benchmark_and_report(argv[0], console_device);

    free(console_device);
    return retval;
}


/**
 * Run open-and-close-benchmark of console API (with devicename given on cmdline) versus evdev API (1 cmdline argument).
 *
 * This program first parses the command line to determine the given
 * console device. Then it runs the benchmarks measuring the
 * performance of open(2)-and-close(2) cycles for the given console
 * device and for the well-known evdev device.
 *
 * After the measurement, a report on the measured values is printed.
 *
 * This program contains race conditions between the consistency
 * checks and the actual open(2) for O_WRONLY. Those should not matter
 * in the use cases of this manually invoked test program run as a
 * non-priviledged user.
 *
 * @param argc Length of the `argv` string array.
 * @param argv Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
static
int main2(const int argc, const char *const argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s: Exactly one argument needed: the console 'device'.\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *const console_device = argv[1];
    if (*console_device == '\0') {
        fprintf(stderr, "%s: 'device' argument must be non-empty string\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct stat sb;
    const int ret_stat = stat(console_device, &sb);
    if (ret_stat != 0) {
        const int saved_errno = errno;
        fprintf(stderr, "%s: stat(2): 'device' argument: %s\n", argv[0],
                strerror(saved_errno));
        return EXIT_FAILURE;
    }

    if ((sb.st_mode & S_IFMT) == S_IFCHR) {
        /* character device */
    } else {
        fprintf(stderr, "%s: stat(2): 'device' argument is not a character device\n", argv[0]);
        return EXIT_FAILURE;
    }

    const int ret_open = open(console_device, O_WRONLY);
    if (ret_open != 0) {
        const int saved_errno = errno;
        fprintf(stderr, "%s: open(2) O_WRONLY error for %s: %s\n",
                argv[0], console_device, strerror(saved_errno));
        return EXIT_FAILURE;
    }
    /* console_device considered valid now */

    return benchmark_and_report(argv[0], console_device);
}


/**
 * Repeat open-and-close cycle for given repeats for given device (2 cmdline arguments).
 *
 * This program first parses the command line to determine the number
 * of repeats to test, and the character device to test. Then it runs
 * the benchmark of repeating the open(2)-and-close(2) cycle for the
 * given number of repeats on the given character device.
 *
 * Timing this benchmark is left to the caller.
 *
 * This program contains race conditions between the consistency
 * checks and the actual open(2) for O_WRONLY. Those should not matter
 * in the use cases of this manually invoked test program run as a
 * non-priviledged user.
 *
 * @param argc Length of the `argv` string array.
 * @param argv Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
static
int main3(const int argc, const char *const argv[])
{
    if (argc != 3) {
        fprintf(stderr, "%s: Exactly two arguments needed: 'repeats' and 'device'.\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *repeats_str = argv[1];
    if (*repeats_str == '\0') {
        fprintf(stderr, "%s: 'repeats' argument must be non-empty string\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *repeats_endptr;
    unsigned long int repeats = strtoul(repeats_str, &repeats_endptr, 10);
    if (*repeats_endptr != '\0') {
        fprintf(stderr, "%s: 'repeats' argument must be a number\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (repeats == ULONG_MAX) {
        fprintf(stderr, "%s: 'repeats' argument out of range\n", argv[0]);
        return EXIT_FAILURE;
    }
    /* repeats contains a valid number of repeats now */

    const char *device_str = argv[2];
    if (*device_str == '\0') {
        fprintf(stderr, "%s: 'device' argument must be non-empty string\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct stat sb;
    const int ret_stat = stat(device_str, &sb);
    if (ret_stat == 0) {
        /* successful stat(2) call */
    } else if (ret_stat == -1) {
        const int saved_errno = errno;
        fprintf(stderr, "%s: stat(2): 'device' argument: %s\n", argv[0],
                strerror(saved_errno));
        return EXIT_FAILURE;
    } else {
        fprintf(stderr, "%s: stat(2): 'device' argument: undocumented result %d\n", argv[0],
                ret_stat);
        return EXIT_FAILURE;
    }

    if ((sb.st_mode & S_IFMT) == S_IFCHR) {
        /* character device */
    } else {
        fprintf(stderr, "%s: stat(2): 'device' argument is not a character device\n", argv[0]);
        return EXIT_FAILURE;
    }
    /* device_str considered valid now */

    return run_cycles(repeats, device_str);
}


/**
 * The main() function.
 *
 * @param argc Length of the `argv` string array.
 * @param argv Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
int main(const int argc, const char *const argv[])
{
    if (false) {
        /* do nothing */
    } else if (argc == 1) {
        return main1(argc, argv);
    } else if (argc == 2) {
        if (strcmp("--help", argv[1]) == 0) {
            print_usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        }
        return main2(argc, argv);
    } else if (argc == 3) {
        return main3(argc, argv);
    } else {
        print_usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
