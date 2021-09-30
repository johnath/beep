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
 * Usage: See print_usage() function just below the "#include"s.
 *
 * @{
 *
 */

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

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "beep-compiler.h"


/**
 * Print usage message for the issue-6-benchmark program.
 */
static
void print_usage(FILE *file, const char *const argv0)
{
    fprintf(file,
            "issue-6-benchmark - benchmark program for open(2) speed (github issue #6)\n"
            "Benchmark open(2)-and-close(2) cycles on console versus evdev speaker API.\n"
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
            "Obviously, the `pcspkr.ko' kernel module must be loaded for the PC speaker\n"
            "evdev character device file to be available.\n"
            "\n"
            "See also the https://github.com/spkr-beep/beep/issues/6 discussion.\n"
            ,
            argv0,
            argv0,
            argv0
            );
}


/** The minimum measurement period considered to create a reliable result.
 *
 * A value of 23 seconds works nicely as we are basically running 4
 * times this value plus a bit of change. The total will then still
 * fit into the "takes a minute or two" range.
 */
#define MINIMUM_RELIABLE_PERIOD 23.0


/**
 * Print the counter array.
 *
 * @param counters The array of two counters to print.
 *
 */

static
void print_counters(unsigned long *counters)
    __attribute__(( nonnull(1) ));

static
void print_counters(unsigned long *counters)
{
    printf("      counters[succ] = %lu\n"
           "      counters[fail] = %lu\n",
           counters[true],
           counters[false]);
}


/**
 * Record all resource and time usage data for repeats on device.
 */
struct issue6_rusage {
    /** The device name for which resource and time usage are recorded. */
    const char *device;

    /** The number of repeats for which resource and time usage are recorded. */
    unsigned long repeats;

    /** The counters which have been recorded during the benchmark. */
    unsigned long counters[2];

    /** The recorded usr time converted to a double */
    double time_usr;

    /** The recorded sys time converted to a double */
    double time_sys;

    /** The recorded elapsed wall clock time converted to a double */
    double time_wall;

    /** The difference between getrusage() before and after the
     *  repeats, except for the usr and sys times which need to be
     *  looked at in the above doubles. */
    struct rusage rusage;
};


/**
 * Print the given struct issue6_rusage in human readable form.
 */

static
void print_issue6_rusage(const struct issue6_rusage *const usage)
    __attribute__(( nonnull(1) ));

static
void print_issue6_rusage(const struct issue6_rusage *const usage)
{
    if (usage->repeats <= 0) {
        return;
    }
    if (usage->device == NULL) {
        return;
    }

    const double time_unaccounted =
        usage->time_wall - usage->time_usr - usage->time_sys;
    const double time_unaccounted_rel =
        time_unaccounted / usage->time_wall;

    /*
    printf("  %lu repeats on device %s\n",
           usage->repeats,
           usage->device);
    */

    printf("    Time and resource usage (according to getrusage(2) and clock_gettime(2)):\n"
           "      usr time (seconds):             %12.6f\n"
           "      sys time (seconds):             %12.6f\n"
           "      wall clock time (seconds):      %12.6f\n"
           "      voluntary context switches:     %5ld\n"
           "      involuntary context switches:   %5ld\n"
           "      major (I/O) page faults:        %5ld\n"
           "      minor (reclaim) page faults:    %5ld\n"
           "    Time unaccounted for (wall clock - usr - sys):\n"
           "      in seconds                      %12.6f\n"
           "      as fraction of wall clock time    %6.2f%%\n"
           "\n",
           usage->time_usr,
           usage->time_sys,
           usage->time_wall,
           usage->rusage.ru_nvcsw,
           usage->rusage.ru_nivcsw,
           usage->rusage.ru_majflt,
           usage->rusage.ru_minflt,
           time_unaccounted,
           100.0 * time_unaccounted_rel);
}


/**
 * Repeatedly open(2) (O_WRONLY) and close(2) a given device.
 *
 * Repeatedly open(2) with O_WRONLY and close(2) a given device for a
 * given number of times.  Count the number of times the open(2) call
 * succeed and fails, so that the compiler cannot do any shortcut
 * optimizations for this loop.
 *
 * Timing the run and printing the results are left to the caller.
 *
 * @param repeats  The number of times to repeat the open(2)-and-close(2) sequence.
 * @param device   The name of the device file.
 * @param counters The counters this run generates.
 *
 * @return EXIT_FAILURE if some error occurred
 * @return EXIT_SUCCESS otherwise
 */

static
int run_cycles(const unsigned long repeats,
               const char *const device,
               unsigned long *counters)
    __attribute__(( nonnull(2,3), warn_unused_result ));

static
int run_cycles(const unsigned long repeats,
               const char *const device,
               unsigned long *counters)
{
    for (unsigned long u=repeats; u>0; u--) {
        const int fd = open(device, O_WRONLY);
        counters[(fd >= 0)]++;
        close(fd);
    }

    if (counters[false] > 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}


/**
 * Measure running the given number of cycles on given device.
 *
 * @param repeats The number of repeats to time.
 * @param device  The device to run the repeats on.
 * @param usage   The struct issue6_rusage to store resource and time required.
 * @param verbose Whether to call print_issue6_rusage() after the cycles.
 *
 * @return Negative value in case of any error
 * @return average cycle time otherwise
 */

static
double measure_cycles(const unsigned long repeats,
                      const char *const device,
                      struct issue6_rusage *usage,
                      const bool verbose)
    __attribute__(( nonnull(3) ));

static
double measure_cycles(const unsigned long repeats,
                      const char *const device,
                      struct issue6_rusage *usage,
                      const bool verbose)
{
    if (repeats == 0) {
        return -1.0;
    }
    if (device == NULL) {
        return -1.0;
    }

    printf("  Measuring %lu repeats for %s\n",
           repeats, device);
    usage->repeats = repeats;
    usage->device  = device;

    struct timespec time_begin;
    struct timespec time_end;
    memset(&time_begin, 0, sizeof(time_begin));
    memset(&time_end,   0, sizeof(time_end));

    struct rusage usage_begin;
    struct rusage usage_end;
    memset(&usage_begin, 0, sizeof(usage_begin));
    memset(&usage_end,   0, sizeof(usage_end));

    const int getrusage_ret_begin =
        getrusage(RUSAGE_SELF, &usage_begin);
    const int gettime_ret_begin =
        clock_gettime(CLOCK_BOOTTIME, &time_begin);

    const int run_cycles_retval =
        run_cycles(repeats, device, usage->counters);

    const int getrusage_ret_end =
        getrusage(RUSAGE_SELF, &usage_end);
    const int gettime_ret_end =
        clock_gettime(CLOCK_BOOTTIME, &time_end);

    print_counters(usage->counters);

    if ((getrusage_ret_begin != 0) ||
        (getrusage_ret_end   != 0)) {
        fprintf(stderr, "error running getrusage(2)\n");
        return -1.0;
    }

    if ((gettime_ret_begin   != 0) ||
        (gettime_ret_end     != 0)) {
        fprintf(stderr, "error running clock_gettime(2)\n");
        return -1.0;
    }

    const double wtime_delta = 0.0
        + 1.000000000 * ((double)time_end.tv_sec)
        + 0.000000001 * ((double)time_end.tv_nsec)
        - 1.000000000 * ((double)time_begin.tv_sec)
        - 0.000000001 * ((double)time_begin.tv_nsec);
    usage->time_wall = wtime_delta;

    const double utime_delta = 0.0
        + 1.000000 * ((double)usage_end.ru_utime.tv_sec)
        + 0.000001 * ((double)usage_end.ru_utime.tv_usec)
        - 1.000000 * ((double)usage_begin.ru_utime.tv_sec)
        - 0.000001 * ((double)usage_begin.ru_utime.tv_usec);
    usage->time_usr = utime_delta;

    const double stime_delta = 0.0
        + 1.000000 * ((double)usage_end.ru_stime.tv_sec)
        + 0.000001 * ((double)usage_end.ru_stime.tv_usec)
        - 1.000000 * ((double)usage_begin.ru_stime.tv_sec)
        - 0.000001 * ((double)usage_begin.ru_stime.tv_usec);
    usage->time_sys = stime_delta;

    /* all documented fields of struct rusage except the two struct timeval */
    usage->rusage.ru_maxrss   = usage_end.ru_maxrss   - usage_begin.ru_maxrss;
    usage->rusage.ru_ixrss    = usage_end.ru_ixrss    - usage_begin.ru_ixrss;
    usage->rusage.ru_idrss    = usage_end.ru_idrss    - usage_begin.ru_idrss;
    usage->rusage.ru_isrss    = usage_end.ru_isrss    - usage_begin.ru_isrss;
    usage->rusage.ru_minflt   = usage_end.ru_minflt   - usage_begin.ru_minflt;
    usage->rusage.ru_majflt   = usage_end.ru_majflt   - usage_begin.ru_majflt;
    usage->rusage.ru_nswap    = usage_end.ru_nswap    - usage_begin.ru_nswap;
    usage->rusage.ru_inblock  = usage_end.ru_inblock  - usage_begin.ru_inblock;
    usage->rusage.ru_oublock  = usage_end.ru_oublock  - usage_begin.ru_oublock;
    usage->rusage.ru_msgsnd   = usage_end.ru_msgsnd   - usage_begin.ru_msgsnd;
    usage->rusage.ru_msgrcv   = usage_end.ru_msgrcv   - usage_begin.ru_msgrcv;
    usage->rusage.ru_nsignals = usage_end.ru_nsignals - usage_begin.ru_nsignals;
    usage->rusage.ru_nvcsw    = usage_end.ru_nvcsw    - usage_begin.ru_nvcsw;
    usage->rusage.ru_nivcsw   = usage_end.ru_nivcsw   - usage_begin.ru_nivcsw;

    if (verbose) {
        print_issue6_rusage(usage);
    }

    if ((run_cycles_retval == 0) && (usage->repeats > 0)) {
        const double avg_cycle_time =
            usage->time_wall / ((double)usage->repeats);
        return avg_cycle_time;
    } else {
        return -1.0;
    }
}


/**
 * Determine number of repeats required for a reliable measurement.
 *
 * This determines the number of repeats needed to certainly take more
 * than #MINIMUM_RELIABLE_PERIOD seconds on the wall clock. Then we
 * consider the measurement reliable enough and calculate the average
 * measured cycle time.
 *
 * @param repeats Number of repeats on which basis to calculate
 *                the number of repeats needed for a proper measurement.
 * @param device  Name of the device to measure the
 *                open(2)-and-close(2) cycle duration on.
 *
 * @return 0 if some error occurred
 * @return the repeats to run on the given device for reliable measurement
 *
 */

static
unsigned long repeats_for_measurement(const unsigned long repeats,
                                      const char *const device)
    __attribute__(( warn_unused_result ));

static
unsigned long repeats_for_measurement(const unsigned long repeats,
                                      const char *const device)
{
    if (device == NULL) {
        return 0;
    }

    /* printf("  Trying %lu repeats for device %s\n", repeats, device); */

    struct issue6_rusage issue6_rusage;
    memset(&issue6_rusage, 0, sizeof(issue6_rusage));

    const double retval = measure_cycles(repeats, device, &issue6_rusage, false);

    if (retval < 0.0) {
        fprintf(stderr, "Aborting due to error(s) in measure_cycles()\n");
        return 0;
    }

    printf("    Time spent: %g s\n", issue6_rusage.time_wall);

    const double d_repeats = (double)issue6_rusage.repeats;
    const double d_reliable_repeats = 1.10 *
        ((double)MINIMUM_RELIABLE_PERIOD) * d_repeats / issue6_rusage.time_wall;

    const unsigned long rounded_cycles =
        (unsigned long)lrint(0.5+d_reliable_repeats);

    return rounded_cycles;
}


/**
 * Run externally timed main_argc3() process via fork(2), execv(3), and waitpid(2).
 *
 * This runs a separate instance of `issue-6-benchmark` via
 * `/usr/bin/time -v`. GNU time gives a lot more information about the
 * system resources used.
 *
 * @param argv0   The name issue-6-benchmark has been called, and
 *                which `/usr/bin/time -v` is going run now.
 * @param repeats The number of repeats to time.
 * @param device  The device to run the repeats on.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 *
 */

static
int execute_time_ext(const char *const argv0,
                     const unsigned long repeats,
                     const char *const device)
    __attribute__(( nonnull(1,3) ));

static
int execute_time_ext(const char *const argv0,
                     const unsigned long repeats,
                     const char *const device)
{
    printf("  /usr/bin/time -v %s %lu %s\n",
           argv0, repeats, device);
    fflush(stdout);
    fflush(stderr);
    /* We need to have flushed everything before fork(2). Otherwise
     * the same unflushed data is present in the buffers of *both*
     * parent process and child process. */
    const pid_t pid = fork();
    if (pid == -1) {
        /* we are the parent process, and fork(2) has failed */
        perror("fork(2)");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        /* we are the child process */
        char repeat_buf[20];
        snprintf(repeat_buf, sizeof(repeat_buf), "%lu", repeats);
        char *dup_argv0  = strdup(argv0);
        char *dup_device = strdup(device);
        char *argv[10] = { "/usr/bin/time", "-v",
                           dup_argv0, repeat_buf, dup_device,
                           NULL
        };
        execv("/usr/bin/time", argv);
        perror("execv(3)");
        free(dup_device);
        free(dup_argv0);
        return EXIT_FAILURE;
    } else {
        /* we are the parent process, and now wait for child <pid> to finish */
        int wstatus = -1;
        const pid_t w = waitpid(pid, &wstatus, 0);
        if (w != pid) {
            fprintf(stderr, "some error in wait4(2)\n");
            return EXIT_FAILURE;
        } else {
            printf("\n");
            if (WIFEXITED(wstatus)) {
                /* printf("Child has exited properly.\n"); */
                const int child_exit_code = WEXITSTATUS(wstatus);
                /* printf("Child has exit code %d\n", child_exit_code); */
                if (child_exit_code == 0) {
                    return EXIT_SUCCESS;
                } else {
                    return EXIT_FAILURE;
                }
            } else {
                printf("Child has not exited, which is a weird error.\n");
                return EXIT_FAILURE;
            }
        }
    }
}


/**
 * Print summary on average time for open-and-close cycle.
 *
 * @param api_str Either "console" or "evdev".
 * @param issue6_rusage The struct issue6_rusage to print.
 */

static
void print_summary(const char *const api_str, struct issue6_rusage *issue6_rusage)
{
    if (issue6_rusage->repeats <= 0) {
        return;
    }
    if (issue6_rusage->device == NULL) {
        return;
    }

    const double avg_cycle_time =
        issue6_rusage->time_wall / ((double)issue6_rusage->repeats);

    printf("    %s device: %s\n"
           "        time per open(2)-and-close(2): %11.3f us\n"
           "        open(2)-and-close(2) rate:     %11.3f / s\n",
           api_str, issue6_rusage->device,
           1000000.0*avg_cycle_time, 1.0/avg_cycle_time
           );
}


/**
 * Well-known name for the evdev device.
 */
static
const char *const evdev_device_str =
    "/dev/input/by-path/platform-pcspkr-event-spkr";


/**
 * Run the benchmark and print a report about the results.
 *
 * We start with a reasonable guess for the initial repeats, and then
 * let the benchmark do a reliable measurement of the average cycle
 * time.
 *
 * @param argv0              The argv[0] from main().
 * @param console_device_str The console device to test. If NULL, skip
 *                           benchmarking the console API device.
 */
static
int benchmark_and_report(const char *const argv0,
                         const char *const console_device_str)
{
    printf("Running benchmark(s). This will literally take a minute or two.\n\n");

    printf("For reproducible results, make sure the CPU load very close to 0,\n"
           "that there is a sufficient amount of available RAM, and there is\n"
           "no heavy I/O load. Stopping web browsers running Javascript might help.\n\n");

    printf("Beginning with a quick test run to dimension the actual benchmark:\n");

    /* These both take between around 1 to 10 seconds on my
     * system. That should be a good enough base to extrapolate to
     * MINIMUM_RELIABLE_PERIOD from. */
    const unsigned long repeats_console =
        repeats_for_measurement(250000UL, console_device_str);
    const unsigned long repeats_evdev =
        repeats_for_measurement(250UL, evdev_device_str);

    printf("\nNow for some actual benchmarks, measured internally:\n");

    struct issue6_rusage issue6_rusage_console;
    memset(&issue6_rusage_console, 0, sizeof(issue6_rusage_console));

    struct issue6_rusage issue6_rusage_evdev;
    memset(&issue6_rusage_evdev, 0, sizeof(issue6_rusage_evdev));

    const double avg_cycle_time_console =
        measure_cycles(repeats_console, console_device_str,
                       &issue6_rusage_console, true);

    const double avg_cycle_time_evdev =
        measure_cycles(repeats_evdev, evdev_device_str,
                       &issue6_rusage_evdev, true);

    printf("Summary:\n");

    print_summary("console", &issue6_rusage_console);
    print_summary("evdev",   &issue6_rusage_evdev);

    if ((avg_cycle_time_evdev > 0.0) && (avg_cycle_time_console > 0.0)) {
        printf("\n"
               "So opening an evdev device takes %g times as long\n"
               "as opening a console device.\n",
               avg_cycle_time_evdev/avg_cycle_time_console);
    }

    printf("\n"
           "Notes:\n"
           "  * All measured times can only be relied upon when this benchmark\n"
           "    is running on an otherwise idle machine, as wall clock time matters.\n"
           "  * The really weird thing is that for the evdev device, almost 100%% of\n"
           "    the elapsed wall clock time is spent doing something which is\n"
           "    *neither* system time *nor* user time. What kind of time is it then?\n"
           );

    if ((repeats_console > 0) || (repeats_evdev > 0)) {
        printf("\n"
               "Using the external command '%s' for the most details:\n",
               "/usr/bin/time -v");

        if (console_device_str) {
            execute_time_ext(argv0, repeats_console, console_device_str);
        }
        execute_time_ext(argv0, repeats_evdev, evdev_device_str);
    }

    return EXIT_SUCCESS;
}


/**
 * Examine all /dev/ttyN devices until we find a writable tty device.
 *
 * @return Name of a writeable /dev/ttyN virtual console device.
 * @return NULL if no writable device has been found, or if an error occurred.
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

            const size_t ttydev_name_sz =
                strlen("/dev/") + strlen(de->d_name) + 1 + 1;
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
 * Run benchmarks of console (autodetected /dev/ttyN) versus evdev.
 *
 * Implement issue-6-benchmark when called with zero command line
 * parameters: Compare the performances of the autodetected /dev/ttyN
 * console device and the well-known evdev device.
 *
 * @param argc Copied from main(): Length of the `argv` string array.
 * @param argv Copied from main(): Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
static
int main_argc1(const int argc, const char *const argv[])
{
    if (argc != 1) {
        fprintf(stderr,
                "%s: Exactly zero arguments needed.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    char *console_device = find_writable_tty();
    if (console_device) {
        printf("Writable TTY device found: %s\n\n", console_device);
    } else {
        printf("ERROR: No writable TTY device found.\n"
               "\n"
               "If you want to run the virtual console device benchmark, Ctrl-C RIGHT NOW.\n"
               "\n"
               "Then either log in on a virtual console to provide a console device\n"
               "we can open, or run as a user which can open a console device\n"
               "(*cough* root *cough* /dev/tty0).\n"
               "\n");
    }

    const int retval = benchmark_and_report(argv[0], console_device);

    free(console_device);
    return retval;
}


/**
 * Run benchmarks of console (device from cmdline) versus evdev.
 *
 * Implement issue-6-benchmark when called with one command line
 * parameter: Compare the performances of the console device given on
 * the command line and the well-known evdev device.
 *
 * After the measurement, a report on the measured values is printed.
 *
 * This program contains race conditions between the consistency
 * checks and the actual open(2) for O_WRONLY. Those should not matter
 * in the use cases of this manually invoked test program run as a
 * non-privileged user.
 *
 * @param argc Copied from main(): Length of the `argv` string array.
 * @param argv Copied from main(): Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
static
int main_argc2(const int argc, const char *const argv[])
{
    if (argc != 2) {
        fprintf(stderr,
                "%s: Exactly one argument needed: the console 'device'.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const char *const console_device = argv[1];
    if (*console_device == '\0') {
        fprintf(stderr, "%s: 'device' argument must be non-empty string\n",
                argv[0]);
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
        fprintf(stderr,
                "%s: stat(2): 'device' argument is not a character device\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const int ret_open = open(console_device, O_WRONLY);
    if (ret_open < 0) {
        const int saved_errno = errno;
        fprintf(stderr, "%s: open(2) O_WRONLY error for %s: %s\n",
                argv[0], console_device, strerror(saved_errno));
        return EXIT_FAILURE;
    }
    close(ret_open);
    /* console_device considered valid now */

    return benchmark_and_report(argv[0], console_device);
}


/**
 * Run one benchmark of given repeats for given device.
 *
 * Implement issue-6-benchmark when called with two command line
 * parameters.
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
 * non-privileged user.
 *
 * @param argc Copied from main(): Length of the `argv` string array.
 * @param argv Copied from main(): Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
static
int main_argc3(const int argc, const char *const argv[])
{
    if (argc != 3) {
        fprintf(stderr,
                "%s: Exactly two arguments needed: 'repeats' and 'device'.\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    const char *repeats_str = argv[1];
    if (*repeats_str == '\0') {
        fprintf(stderr, "%s: 'repeats' argument must be non-empty string\n",
                argv[0]);
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

    const char *device = argv[2];
    if (*device == '\0') {
        fprintf(stderr, "%s: 'device' argument must be non-empty string\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct stat sb;
    const int ret_stat = stat(device, &sb);
    if (ret_stat == 0) {
        /* successful stat(2) call */
    } else if (ret_stat == -1) {
        const int saved_errno = errno;
        fprintf(stderr,
                "%s: stat(2): 'device' argument: %s\n", argv[0],
                strerror(saved_errno));
        return EXIT_FAILURE;
    } else {
        fprintf(stderr,
                "%s: stat(2): 'device' argument: undocumented result %d\n",
                argv[0], ret_stat);
        return EXIT_FAILURE;
    }

    if ((sb.st_mode & S_IFMT) == S_IFCHR) {
        /* character device */
    } else {
        fprintf(stderr,
                "%s: stat(2): 'device' argument is not a character device\n",
                argv[0]);
        return EXIT_FAILURE;
    }
    /* device considered valid now */

    unsigned long counters[2];
    memset(&counters, 0, sizeof(counters));
    const int run_cycles_retval = run_cycles(repeats, device, counters);
    print_counters(counters);
    return run_cycles_retval;
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
    switch (argc) {
    case 1:
        return main_argc1(argc, argv);
    case 2:
        if (strcmp("--help", argv[1]) == 0) {
            print_usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        }
        return main_argc2(argc, argv);
    case 3:
        return main_argc3(argc, argv);
    }
    print_usage(stderr, argv[0]);
    return EXIT_FAILURE;
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
