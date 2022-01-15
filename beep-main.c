/** \file beep-main.c
 * \brief main program to beep the pc speaker any number of ways
 * \author Copyright (C) 2000-2010 Johnathan Nightingale
 * \author Copyright (C) 2010-2013 Gerfried Fuchs
 * \author Copyright (C) 2013-2022 Hans Ulrich Niedermann
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
 *
 * \defgroup beep_main The main program
 *
 * For more documentation on using `beep`, see the `beep-usage.txt` and
 * `beep.1.in` files.
 *
 * More general information can be found in the `README.md`,
 * `PERMISSIONS.md`, `INSTALL.md`, and if you want to create a binary
 * distribution package, `PACKAGING.md` file.
 *
 * In case you want to start developing `beep` itself,
 * `DEVELOPMENT.md` may be of interest to you.
 *
 * @{
 *
 */


#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "beep-config.h"
#include "beep-compiler.h"
#include "beep-drivers.h"
#include "beep-library.h"
#include "beep-log.h"
#include "beep-types.h"
#include "beep-usage.h"


#define LOG_MODULE "main"


/**
 * Version message to be printed for "beep --version".
 */
static
const char version_message[] =
    PACKAGE_TARNAME " " PACKAGE_VERSION "\n"
    "Copyright (C) 2002-2016 Johnathan Nightingale\n"
    "Copyright (C) 2013-2022 Hans Ulrich Niedermann\n"
    "Use and Distribution subject to GPL.\n"
    "For information: http://www.gnu.org/copyleft/.\n";


/**
 * Per note parameter set.
 */
typedef struct beep_parms_T beep_parms_T;


/* The default values are defined in beep-config.h */


/**
 * Per note parameter set (including heritage information and linked list pointer).
 */
struct beep_parms_T
{
    unsigned int freq;       /**< tone frequency (Hz)         */
    unsigned int length;     /**< tone length    (ms)         */
    unsigned int reps;       /**< number of repetitions       */
    unsigned int delay;      /**< delay between reps  (ms)    */
    end_delay_E  end_delay;  /**< do we delay after last rep? */

    /** Are we using stdin triggers?
     *
     * We have three options:
     *   - just beep and terminate (default)
     *   - beep after a line of input
     *   - beep after a character of input
     * In the latter two cases, pass the text back out again,
     * so that beep can be tucked appropriately into a text-
     * processing pipe.
     */
    stdin_beep_E stdin_beep;

    beep_parms_T *next;      /**< in case -n/--new is used. */
};


/**
 * Global flag for signalling program abort due to signal handlers.
 *
 * Initialized as false. Written only by the signal handlers (set to
 * true), read only by the main thread.
 */
static
volatile sig_atomic_t global_abort = false;


/**
 * Signal handler for signals like SIGINT and SIGTERM.
 *
 * If we get interrupted, it would be nice to not leave the speaker
 * beeping in perpetuity.
 *
 * Everything called from this signal handler must be thread-safe,
 * signal-safe, reentrant including all API functions.  Otherwise, we
 * get another CVE-2018-0492.
 *
 * So we make certain we keep to using the following API calls:
 *
 *   * close(2):      safe
 *   * _exit(2):      safe (which exit(3) is NOT)
 *   * bzero(3):      MT-safe
 *   * memset(3):     MT-safe
 *   * write(2):      safe
 *   * strerror_r(3): MT-safe
 *   * strlen(3):     MT-safe
 *
 * Just setting a global flag is MT-safe.
 *
 * @param unused_signum The signal number being handled. Unused.
 */

void handle_signal(int unused_signum UNUSED_PARAM);

void handle_signal(int unused_signum UNUSED_PARAM)
{
    global_abort = true;
}


/**
 * Print usage message, but leave exit code up to the caller.
 */
static
void print_usage(void)
{
    fputs(beep_usage, stdout);
}


/**
 * Print usage message and exit.
 */

static
void usage_bail(void)
    __attribute__(( noreturn ));

static
void usage_bail(void)
{
    print_usage();
    exit(EXIT_FAILURE);
}


/**
 * Global device name parameter.
 *
 * Written by parse_command_line(), read by main() initialization.
 */
static
char *param_device_name = NULL;


/**
 * Parse the command line.
 *
 * argv should be untampered, as passed to main().  Beep parameters
 * are returned in result, subsequent parameters in `argv` will over-
 * ride previous ones.
 *
 * Currently valid parameters:
 *  "-f <frequency in Hz>"
 *  "-l <tone length in ms>"
 *  "-r <repetitions>"
 *  "-d <delay in ms>"
 *  "-D <delay in ms>" (similar to -d, but delay after last repetition as well)
 *  "-s" (beep after each line of input from stdin, echo line to stdout)
 *  "-c" (beep after each char of input from stdin, echo char to stdout)
 *  "--verbose/--debug"
 *  "-h/--help"
 *  "-v/-V/--version"
 *  "-n/--new"
 *
 * March 29, 2002 - Daniel Eisenbud points out that `ch` should be
 * `int`, not `char`, for correctness on platforms with unsigned
 * chars.
 *
 * @param argc   Argument counter passed from main().
 * @param argv   Argument string array passed from main().
 * @param result Linked list to be built by parse_command_line().
 */
static
void parse_command_line(const int argc, char *const argv[], beep_parms_T *result)
    __attribute__(( nonnull(3) ));

static
void parse_command_line(const int argc, char *const argv[], beep_parms_T *result)
{
    int ch;

    static
        const struct option opt_list[] =
        { {"help",    no_argument,       NULL, 'h'},
          {"version", no_argument,       NULL, 'V'},
          {"new",     no_argument,       NULL, 'n'},
          {"verbose", no_argument,       NULL, 'X'},
          {"debug",   no_argument,       NULL, 'X'},
          {"device",  required_argument, NULL, 'e'},
          {NULL,      0,                 NULL,  0 }
        };

    while ((ch = getopt_long(argc, argv, "f:l:r:d:D:schvVne:", opt_list, NULL))
           != EOF) {
        /* Temporary storage for parsing numbers for various arguments */
        int          argval_i = -1;
        unsigned int argval_u = ~0U;
        float        argval_f = -1.0f;

        switch (ch) {
        case 'f':  /* freq */
            if (sscanf(optarg, "%f", &argval_f) != 1) {
                usage_bail();
            }
            if ((0.0f > argval_f) || (argval_f > 20000.0f)) {
                usage_bail();
            }
            argval_i = (int) (argval_f + 0.5f);
            argval_u = (unsigned int) argval_i;
            if (result->freq != 0) {
                LOG_WARNING("multiple -f values given, only last one is used.");
            }
            result->freq = argval_u;
            break;
        case 'l' : /* length */
            if (sscanf(optarg, "%u", &argval_u) != 1) {
                usage_bail();
            }
            if (argval_u > 300000U) {
                usage_bail();
            }
            result->length = argval_u;
            break;
        case 'r' : /* repetitions */
            if (sscanf(optarg, "%u", &argval_u) != 1) {
                usage_bail();
            }
            if (argval_u > 300000U) {
                usage_bail();
            }
            result->reps = argval_u;
            break;
        case 'd' : /* delay between reps - WITHOUT delay after last beep*/
            if (sscanf(optarg, "%u", &argval_u) != 1) {
                usage_bail();
            }
            if (argval_u > 300000U) {
                usage_bail();
            }
            result->delay = argval_u;
            result->end_delay = END_DELAY_NO;
            break;
        case 'D' : /* delay between reps - WITH delay after last beep */
            if (sscanf(optarg, "%u", &argval_u) != 1) {
                usage_bail();
            }
            if (argval_u > 300000U) {
                usage_bail();
            }
            result->delay = argval_u;
            result->end_delay = END_DELAY_YES;
            break;
        case 's' :
            result->stdin_beep = STDIN_BEEP_LINE;
            break;
        case 'c' :
            result->stdin_beep = STDIN_BEEP_CHAR;
            break;
        case 'v' :
        case 'V' : /* also --version */
            fputs(version_message, stdout);
            exit(EXIT_SUCCESS);
            /* break; unreachable */
        case 'n' : /* also --new - create another beep */
            if (result->freq == 0) {
                result->freq = DEFAULT_FREQ;
            }
            result->next = (beep_parms_T *)malloc(sizeof(beep_parms_T));
            result->next->freq       = 0;
            result->next->length     = DEFAULT_LENGTH;
            result->next->reps       = DEFAULT_REPS;
            result->next->delay      = DEFAULT_DELAY;
            result->next->end_delay  = DEFAULT_END_DELAY;
            result->next->stdin_beep = DEFAULT_STDIN_BEEP;
            result->next->next       = NULL;
            result = result->next; /* yes, I meant to do that. */
            break;
        case 'X' : /* --debug / --verbose */
            if (log_level < 999) {
                /* just limit to some finite value */
                log_level++;
            }
            break;
        case 'e' : /* also --device */
            if (param_device_name) {
                LOG_ERROR("You cannot give the --device parameter more than once.");
                exit(EXIT_FAILURE);
            }
            param_device_name = optarg;
            break;
        case 'h': /* also --help */
            print_usage();
            exit(EXIT_SUCCESS);
            /* break; unreachable */
        default:
            usage_bail();
            /* break; unreachable */
        }
    }
    if (optind < argc) {
        LOG_ERROR("non-option arguments left on command line");
        usage_bail();
    }
    if (result->freq == 0) {
        result->freq = DEFAULT_FREQ;
    }
}


/**
 * Sleep for a number of milliseconds.
 *
 * @param driver       The driver to close in case of #global_abort.
 * @param milliseconds The number of milliseconds to sleep.
 *
 * @return The nanosleep(2) result (0 if successful, -1 on signal handler or error)
 */

static
int sleep_ms(beep_driver *driver, unsigned int milliseconds)
    __attribute__(( nonnull(1) ));

static
int sleep_ms(beep_driver *driver, unsigned int milliseconds)
{
    const time_t seconds = milliseconds / 1000U;
    const long   nanoseconds = (milliseconds % 1000UL) * 1000UL * 1000UL;
    const struct timespec request =
        { seconds,
          nanoseconds };
    const int retcode = nanosleep(&request, NULL);
    if (global_abort) {
        beep_drivers_end_tone(driver);
        beep_drivers_fini(driver);
        exit(EXIT_FAILURE);
    }
    return retcode;
}


/**
 * Play one (possibly repeated) note.
 *
 * @param driver The driver to run the note on.
 * @param parms  The note parameters.
 */

static
void play_beep(beep_driver *driver, beep_parms_T parms)
    __attribute__(( nonnull(1) ));

static
void play_beep(beep_driver *driver, beep_parms_T parms)
{
    LOG_VERBOSE("%d times %d ms beeps (%d ms delay between, "
                "%d ms delay after) @ %d Hz",
                parms.reps, parms.length, parms.delay, parms.end_delay,
                parms.freq);

    /* repeat the beep */
    for (unsigned int i = 0; (!global_abort) && (i < parms.reps); i++) {
        beep_drivers_begin_tone(driver, parms.freq & 0xffff);
        sleep_ms(driver, parms.length);
        beep_drivers_end_tone(driver);
        if ((parms.end_delay == END_DELAY_YES) || ((i+1) < parms.reps)) {
            sleep_ms(driver, parms.delay);
        }
    }
}


/**
 * If stdout is a TTY, print a bell character to stdout as a fallback.
 */
static
void fallback_beep(void)
{
    /* Printing '\a' can only beep if we print it to a tty */
    if (isatty(STDOUT_FILENO)) {
        fputc('\a', stdout);
    }
}


/**
 * Main program for `beep(1)`.
 *
 * @param argc Length of the `argv` string array.
 * @param argv Command line argument string array.
 *
 * @return EXIT_FAILURE in case of any error
 * @return EXIT_SUCCESS otherwise
 */
int main(const int argc, char *const argv[])
{
    log_init(argc, argv);

    /* Check for setuid and/or sudo
     *
     * Old beep (pre-1.4) mostly needed to run as root in order to use
     * the console API to the PC speaker.  Two popular ways to achieve
     * that were to make the beep executable setuid-root and using
     * sudo and a shell alias to run beep as root.
     *
     * New beep (1.4 and later) uses the evdev API to the PC speaker
     * which does not require being root at all.
     *
     * As it is near impossible to make beep safe for running with
     * elevated privileges (beep opens files for writing, and usually
     * even writes to them, and checking the device file with realpath
     * leaks information), we now check for these two popular old
     * permission setups being carried over to a new beep installation
     * by mistake.
     *
     * These checks are not security measures for future attack
     * scenarios.  These are checks are for detecting remainders of
     * old setups which have no place in a contemporary system.
     */

    /* Bail out if running setuid or setgid.
     */
    if ((getuid() != geteuid()) || (getgid() != getegid())) {
        LOG_ERROR("Running setuid or setgid, "
                  "which is not supported for security reasons.");
        LOG_ERROR("Set up permissions for the pcspkr evdev device file instead.");
        exit(EXIT_FAILURE);
    }

    /* Bail out if running as root under sudo.
     */
    if ((getuid() == 0) || (geteuid() == 0) ||
        (getgid() == 0) || (getegid() == 0)) {
        LOG_VERBOSE("Running with root permissions. "
                    "Checking for SUDO_* in environment.");
        if (getenv("SUDO_COMMAND") || getenv("SUDO_USER") ||
            getenv("SUDO_UID") || getenv("SUDO_GID")) {
            LOG_ERROR("Running as root under sudo, "
                      "which is not supported for security reasons.");
            LOG_ERROR("Set up permissions for the pcspkr evdev device "
                      "file and run as non-root user instead.");
            exit(EXIT_FAILURE);
        }
    }

    /* Parse command line */
    beep_parms_T *parms = (beep_parms_T *)malloc(sizeof(beep_parms_T));
    if (NULL == parms) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    parms->freq       = 0;
    parms->length     = DEFAULT_LENGTH;
    parms->reps       = DEFAULT_REPS;
    parms->delay      = DEFAULT_DELAY;
    parms->end_delay  = DEFAULT_END_DELAY;
    parms->stdin_beep = DEFAULT_STDIN_BEEP;
    parms->next       = NULL;

    parse_command_line(argc, argv, parms);

    beep_driver *driver = NULL;

    if (param_device_name) {
        driver = beep_drivers_detect(param_device_name);
        if (!driver) {
            const int saved_errno = errno;
            LOG_ERROR("Could not open %s for writing: %s",
                      param_device_name, strerror(saved_errno));
            exit(EXIT_FAILURE);
        }
    } else {
        driver = beep_drivers_detect(NULL);
        if (!driver) {
            LOG_ERROR("Could not open any device");
            /* Output the only beep we can, in an effort to fall back on usefulness */
            fallback_beep();
            exit(EXIT_FAILURE);
        }
    }

    LOG_VERBOSE("using %s driver (fd=%d, dev=%s)",
                driver->name,
                driver->device_fd, driver->device_name);

    /* At this time, we know what API to use on which device, and we do
     * not have to fall back onto printing '\a' any more.
     */

    /* Memory barrier. All globals have been set up, so we make sure the
     * values are actually written to memory.  Only then do we install
     * the signal handlers.
     *
     * TBD: Use C11 atomic_signal_fence() instead of "__asm__ volatile"?
     */
    __asm__ volatile ("" : : : "memory");

    /* After all the initialization has happened and the global
     * variables used to communicate with the signal handlers have
     * actually been set up properly, we can finally install the
     * signal handlers. As we do not start making any noises until
     * later, there is no need to install the signal handlers any
     * earlier.
     */
    signal(SIGINT,  handle_signal);
    signal(SIGTERM, handle_signal);

    /* This outermost while loop handles the possibility that -n/--new
     * has been used, i.e. that we have a sequence of multiple beeps
     * specified.  Each loop iteration will play, then free() one parms
     * instance.
     */
    while ((!global_abort) && parms) {
        beep_parms_T *next = parms->next;

        if (parms->stdin_beep != STDIN_BEEP_NONE) {
            /* In this case, beep is probably part of a pipe, in which
               case POSIX says stdin and out should be fully buffered.
               This however means very laggy performance with beep
               just twiddling it's thumbs until a buffer fills. Thus,
               kill the buffering.  In some situations, this too won't
               be enough, namely if we're in the middle of a long
               pipe, and the processes feeding us stdin are buffered,
               we'll have to wait for them, not much to be done about
               that. */
            setvbuf(stdin, NULL, _IONBF, 0);
            setvbuf(stdout, NULL, _IONBF, 0);

            char sin[4096];
            while ((!global_abort) && (fgets(sin, 4096, stdin))) {
                if (parms->stdin_beep == STDIN_BEEP_CHAR) {
                    for (char *ptr=sin; (!global_abort) && (*ptr); ptr++) {
                        putchar(*ptr);
                        fflush(stdout);
                        play_beep(driver, *parms);
                    }
                } else { /* STDIN_BEEP_LINE */
                    fputs(sin, stdout);
                    play_beep(driver, *parms);
                }
            }
        } else {
            play_beep(driver, *parms);
        }

        /* Junk each parms struct after playing it */
        free(parms);
        parms = next;
    }

    beep_drivers_end_tone(driver);
    beep_drivers_fini(driver);

    if (global_abort) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
