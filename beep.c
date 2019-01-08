/* beep - beep the pc speaker any number of ways
 * Copyright (C) 2000-2010 Johnathan Nightingale
 * Copyright (C) 2010-2013 Gerfried Fuchs
 * Copyright (C) 2013-2018 Hans Ulrich Niedermann
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
 */

/*
 * For more documentation on beep, see the beep-usage.txt and
 * beep.1.in files.
 */


#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/kd.h>
#include <linux/input.h>

#include "beep-log.h"
#include "beep-usage.h"


/* Use PIT_TICK_RATE value from the kernel. */
#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193182UL
#endif

static
const char version_message[] =
    PACKAGE_TARNAME " " PACKAGE_VERSION "\n"
    "Copyright (C) 2002-2016 Johnathan Nightingale\n"
    "Copyright (C) 2013-2018 Hans Ulrich Niedermann\n"
    "Use and Distribution subject to GPL.\n"
    "For information: http://www.gnu.org/copyleft/.\n";

/* Meaningful Defaults */
#define DEFAULT_FREQ       440   /* Middle A */
#define DEFAULT_LENGTH     200   /* milliseconds */
#define DEFAULT_REPS       1
#define DEFAULT_DELAY      100   /* milliseconds */
#define DEFAULT_END_DELAY  NO_END_DELAY
#define DEFAULT_STDIN_BEEP NO_STDIN_BEEP

/* Other Constants */
#define NO_END_DELAY    0
#define YES_END_DELAY   1

#define NO_STDIN_BEEP   0
#define LINE_STDIN_BEEP 1
#define CHAR_STDIN_BEEP 2

typedef struct beep_parms_t {
  unsigned int freq; /* tone frequency (Hz)      */
  unsigned int length;     /* tone length    (ms)      */
  unsigned int reps;       /* # of repetitions         */
  unsigned int delay;      /* delay between reps  (ms) */
  unsigned int end_delay;  /* do we delay after last rep? */
  int stdin_beep; /* are we using stdin triggers?  We have three options:
		     - just beep and terminate (default)
		     - beep after a line of input
		     - beep after a character of input
		     In the latter two cases, pass the text back out again,
		     so that beep can be tucked appropriately into a text-
		     processing pipe.
		  */
  struct beep_parms_t *next;  /* in case -n/--new is used. */
} beep_parms_t;

/* Use an enum and switch statements to have compiler warn us about
 * unhandled cases. */
typedef enum {
	      /* When the beep_type has not been set yet, do nothing */
	      BEEP_TYPE_UNSET   = 0,
	      /* Use the console API */
	      BEEP_TYPE_CONSOLE = 1,
	      /* Use the evdev API */
	      BEEP_TYPE_EVDEV   = 2,
} beep_type_E;

/* Momma taught me never to use globals, but we need something the signal
   handlers can get at.*/
static
int console_fd = -1;
static
beep_type_E console_type = BEEP_TYPE_UNSET;
static
const char *console_device = NULL;


/* We do not know for certain whether perror does strange things with
 * global variables or malloc/free inside its code.
 */
static
void safe_error_exit(const char *const msg)
    __attribute__(( noreturn ));

static
void safe_error_exit(const char *const msg)
{
    const int saved_errno = errno;
    char strerr_buf[128];
    const int ret = strerror_r(saved_errno, strerr_buf, sizeof(strerr_buf));
    if (ret != 0) {
        if (write(STDERR_FILENO, "strerror_r error\n",
                  0+0+0+0+strlen("strerror_r error\n"))) {
            /* ignore al write errors */
        }
        _exit(EXIT_FAILURE);
    }
    const size_t msglen = strlen(msg);
    const size_t errlen = strlen(strerr_buf);
    if (write(STDERR_FILENO, msg, msglen)) {
        /* ignore al write errors */
    }
    if (write(STDERR_FILENO, ": ", 2)) {
        /* ignore al write errors */
    }
    if (write(STDERR_FILENO, strerr_buf, errlen)) {
        /* ignore al write errors */
    }
    if (write(STDERR_FILENO, "\n", 1)) {
        /* ignore al write errors */
    }
    _exit(EXIT_FAILURE);
}


static
void do_beep(unsigned int freq) {
  switch (console_type) {
  case BEEP_TYPE_CONSOLE: if (1) {
      const uintptr_t argp = ((freq != 0) ? (CLOCK_TICK_RATE/freq) : freq) & 0xffff;
      if (-1 == ioctl(console_fd, KIOCSOUND, argp)) {
	/* If we cannot use the sound API, we cannot silence the sound either */
	safe_error_exit("ioctl KIOCSOUND");
      }
    }
    break;
  case BEEP_TYPE_EVDEV: if (1) {
      struct input_event e;

      memset(&e, 0, sizeof(e));
      e.type = EV_SND;
      e.code = SND_TONE;
      e.value = (freq & 0xffff);

      if (sizeof(e) != write(console_fd, &e, sizeof(e))) {
	/* If we cannot use the sound API, we cannot silence the sound either */
	safe_error_exit("write EV_SND");
      }
    }
    break;
  case BEEP_TYPE_UNSET:
    /* Do nothing, if this case should ever happen which it should not. */
    break;
  }
}


/* If we get interrupted, it would be nice to not leave the speaker
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
 *   * memset(3):     MT-safe
 *   * write(2):      safe
 *   * strerror_r(3): MT-safe
 *   * strlen(3):     MT-safe
 */
void handle_signal(int signum);

void handle_signal(int signum)
{
  switch(signum) {
  case SIGINT:
  case SIGTERM:
    if(console_fd >= 0) {
      /* Kill the sound, quit gracefully */
      do_beep(0);
      (void) close(console_fd);
      _exit(signum);
    } else {
      /* Just quit gracefully */
      _exit(signum);
    }
  }
}


/* print usage and leave exit code up to the caller */
static
void print_usage(void)
{
    fputs(beep_usage, stdout);
}


/* print usage and exit */
static
void usage_bail(void)
    __attribute__(( noreturn ));

static
void usage_bail(void)
{
    print_usage();
    exit(EXIT_FAILURE);
}


/* whether character is a digit */
static
int is_digit(char c)
{
  return (('0' <= c) && (c <= '9'));
}


/* whether string consists of at least one digit, and only digits */
static
int is_number(const char *const str)
{
  if (str[0] == '\0') {
    return 0;
  }
  for (size_t i=0; str[i] != '\0'; ++i) {
    if (!is_digit(str[i])) {
      return 0;
    }
  }
  return 1;
}


/* whether device is on whitelist */
static
int is_device_whitelisted(const char *const dev)
{
  if (strncmp("/dev/input/", dev, strlen("/dev/input/")) == 0) {
    /* If device name starts with /dev/input/, we can assume evdev
     * input device beeping is wished for and the corresponding
     * device is somewhere in /dev/input/.
     */
    return 1;
  } else if (strcmp("/dev/console", dev) == 0) {
    return 1;
  } else if ((strncmp("/dev/tty", dev, 8) == 0) && is_number(&dev[8])) {
    /* match numbered /dev/tty<N> devices */
    return 1;
  } else if ((strncmp("/dev/vc/", dev, 8) == 0) && is_number(&dev[8])) {
    /* match numbered /dev/vc/<N> devices */
    return 1;
  }
  return 0;
}


/* Parse the command line.  argv should be untampered, as passed to main.
 * Beep parameters returned in result, subsequent parameters in argv will over-
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
 * March 29, 2002 - Daniel Eisenbud points out that c should be int, not char,
 * for correctness on platforms with unsigned chars.
 */
static
void parse_command_line(const int argc, char *const argv[], beep_parms_t *result) {
  int c;

  struct option opt_list[7] = {{"help", 0, NULL, 'h'},
			       {"version", 0, NULL, 'V'},
			       {"new", 0, NULL, 'n'},
			       {"verbose", 0, NULL, 'X'},
			       {"debug", 0, NULL, 'X'},
			       {"device", 1, NULL, 'e'},
			       {0,0,0,0}};
  while((c = getopt_long(argc, argv, "f:l:r:d:D:schvVne:", opt_list, NULL))
	!= EOF) {
    /* handle parsed numbers for various arguments */
    int          argval_i = -1;
    unsigned int argval_u = ~0U;
    float        argval_f = -1.0f;
    switch (c) {
    case 'f':  /* freq */
        if (sscanf(optarg, "%f", &argval_f) != 1) {
            usage_bail();
        }
        if ((argval_f < 0.0f) || (argval_f > 20000.0f)) {
            usage_bail();
        }
        argval_i = (int) (argval_f + 0.5f);
        argval_u = (unsigned int) argval_i;
	if (result->freq != 0) {
            log_warning("multiple -f values given, only last one is used.");
	}
	result->freq = argval_u;
        break;
    case 'l' : /* length */
        if (sscanf(optarg, "%u", &argval_u) != 1) {
            usage_bail();
        }
        if (argval_u > 300000) { /* 5 minutes */
            usage_bail();
        }
        result->length = argval_u;
        break;
    case 'r' : /* repetitions */
        if (sscanf(optarg, "%u", &argval_u) != 1) {
            usage_bail();
        }
        if (argval_u > 300000) { /* 5 minutes */
            usage_bail();
        }
        result->reps = argval_u;
        break;
    case 'd' : /* delay between reps - WITHOUT delay after last beep*/
        if (sscanf(optarg, "%u", &argval_u) != 1) {
            usage_bail();
        }
        if (argval_u > 300000) { /* 5 minutes */
            usage_bail();
        }
        result->delay = argval_u;
        result->end_delay = NO_END_DELAY;
        break;
    case 'D' : /* delay between reps - WITH delay after last beep */
        if (sscanf(optarg, "%u", &argval_u) != 1) {
            usage_bail();
        }
        if (argval_u > 300000) { /* 5 minutes */
            usage_bail();
        }
        result->delay = argval_u;
        result->end_delay = YES_END_DELAY;
        break;
    case 's' :
      result->stdin_beep = LINE_STDIN_BEEP;
      break;
    case 'c' :
      result->stdin_beep = CHAR_STDIN_BEEP;
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
      result->next = (beep_parms_t *)malloc(sizeof(beep_parms_t));
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
      log_level += 1;
      break;
    case 'e' : /* also --device */
      if (console_device) {
        log_error("You cannot give the --device parameter more than once.");
        exit(EXIT_FAILURE);
      }
      if (1) {
	static char realpath_optarg[PATH_MAX+1];
	if (realpath(optarg, realpath_optarg) == NULL) {
	  const int saved_errno = errno;
          log_error("could not run realpath(3) on '%s': %s",
                    optarg, strerror(saved_errno));
	  exit(EXIT_FAILURE);
	}
	if (is_device_whitelisted(realpath_optarg)) {
	  console_device = realpath_optarg;
	} else {
          log_error("Not using device '%s'. If you do need this device, please "
                    "report that fact to <https://github.com/spkr-beep/beep/issues>.",
                    realpath_optarg);
	  exit(EXIT_FAILURE);
	}
      }
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
  if (result->freq == 0) {
      result->freq = DEFAULT_FREQ;
  }
}


static
void play_beep(beep_parms_t parms) {
  unsigned int i; /* loop counter */

  log_verbose("%d times %d ms beeps (%d delay between, "
              "%d delay after) @ %d Hz",
              parms.reps, parms.length, parms.delay, parms.end_delay, parms.freq);

  /* Beep */
  for (i = 0; i < parms.reps; i++) {                    /* start beep */
    do_beep(parms.freq);
    /* Look ma, I'm not ansi C compatible! */
    usleep(1000U*parms.length);                          /* wait...    */
    do_beep(0);                                         /* stop beep  */
    if (parms.end_delay || ((i+1) < parms.reps)) {
       usleep(1000U*parms.delay);                        /* wait...    */
    }
  }                                                     /* repeat.    */
}


/* Open only character device special file (with race condition).
 *
 * We check whether this is a character device special file before
 * opening as for some devices, opening has an effect and we can avoid
 * this effect for those devices here.
 *
 * We still need to make sure that the file we have actually opened
 * actually is a character device special file after we have actually
 * opened it.
 */
static
int open_chr(const char *filename, int flags)
{
  struct stat sb;
  if (-1 == stat(filename, &sb)) {
    return -1;
  }
  if (S_ISCHR(sb.st_mode)) {
    return open(filename, flags);
  } else {
    log_error("console file '%s' is not a character device special file",
              filename);
    exit(EXIT_FAILURE);
  }
}


/* If stdout is a TTY, print a bell character to stdout as a fallback. */
static
void fallback_beep(void)
{
    /* Printing '\a' can only beep if we print it to a tty */
    if (isatty(STDOUT_FILENO)) {
        fputc('\a', stdout);
    }
}


int main(const int argc, char *const argv[]) {
  log_init(argc, argv);

  char sin[4096], *ptr;

  /* Bail out if running setuid or setgid.
   *
   * It is near impossible to make beep setuid-safe:
   *
   *   * We open files for writing, and may even write to them.
   *
   *   * Checking the device file with realpath leaks information.
   *
   * So we refuse running setuid or setgid.
   */
  if ((getuid() != geteuid()) || (getgid() != getegid())) {
    log_error("Running setuid or setgid, "
              "which is not supported for security reasons.");
    log_error("Set up permissions for the pcspkr evdev device file instead.");
    exit(EXIT_FAILURE);
  }

  /* Bail out if running under sudo.
   *
   * For the reasoning, see the setuid comment above.
   */
  if (getenv("SUDO_COMMAND") || getenv("SUDO_USER") || getenv("SUDO_UID") || getenv("SUDO_GID")) {
    log_error("Running under sudo, "
              "which is not supported for security reasons.");
    log_error("Set up permissions for the pcspkr evdev device file instead.");
    exit(EXIT_FAILURE);
  }

  /* Parse command line */
  beep_parms_t *parms = (beep_parms_t *)malloc(sizeof(beep_parms_t));
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

  /* Try opening a console device */
  if (console_device) {
    console_fd = open_chr(console_device, O_WRONLY);
  } else {
    static char *console_device_list[] =
      { "/dev/input/by-path/platform-pcspkr-event-spkr",
	"/dev/tty0",
	"/dev/vc/0",
      };
    for (size_t i=0; i<(sizeof(console_device_list)/sizeof(console_device_list[0])); ++i) {
      if ((console_fd = open_chr(console_device_list[i], O_WRONLY)) != -1) {
	console_device = console_device_list[i];
	break;
      }
    }
  }

  if (console_fd == -1) {
    const int saved_errno = errno;
    log_error("Could not open %s for writing: %s",
              ((console_device != NULL) ? console_device : "console device"),
              strerror(saved_errno));
    /* Output the only beep we can, in an effort to fall back on usefulness */
    fallback_beep();
    exit(EXIT_FAILURE);
  }

  /* Verify that console_fd is actually a character device special file */
  if (1) {
    struct stat sb;
    if (-1 == fstat(console_fd, &sb)) {
      perror("fstat");
      exit(EXIT_FAILURE);
    }
    if (S_ISCHR(sb.st_mode)) {
      /* GOOD: console_fd is a character device special file. Use it. */
    } else {
      /* BAD: console_fd is not a character device special file. Do
       * not use it any further, and certainly DO NOT WRITE to it.
       */
      log_error("opened console '%s' is not a character special device",
                console_device);
      exit(EXIT_FAILURE);
    }
  }

  /* Determine the API supported by the opened console device */
  if (ioctl(console_fd, EVIOCGSND(0)) != -1) {
    log_verbose("Using BEEP_TYPE_EVDEV on '%s'", console_device);
    console_type = BEEP_TYPE_EVDEV;
  } else if (ioctl(console_fd, KIOCSOUND, 0) >= 0) {
    /* turning off the beeps should be a safe way to check for API support */
    log_verbose("Using BEEP_TYPE_CONSOLE on '%s'", console_device);
    console_type = BEEP_TYPE_CONSOLE;
  } else {
    log_error("console device '%s' does not support either API",
              console_device);
    /* Output the only beep we can, in an effort to fall back on usefulness */
    fallback_beep();
    exit(EXIT_FAILURE);
  }

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
   * actually been set up properly, we can finally install the signal
   * handlers. As we do not start making any noises, there is no need
   * to install the signal handlers any earlier.
   */
  signal(SIGINT,  handle_signal);
  signal(SIGTERM, handle_signal);

  /* this outermost while loop handles the possibility that -n/--new has been
     used, i.e. that we have multiple beeps specified. Each iteration will
     play, then free() one parms instance. */
  while(parms) {
    beep_parms_t *next = parms->next;

    if(parms->stdin_beep) {
      /* in this case, beep is probably part of a pipe, in which case POSIX 
	 says stdin and out should be fuly buffered.  This however means very 
	 laggy performance with beep just twiddling it's thumbs until a buffer
	 fills. Thus, kill the buffering.  In some situations, this too won't 
	 be enough, namely if we're in the middle of a long pipe, and the 
	 processes feeding us stdin are buffered, we'll have to wait for them,
	 not much to  be done about that. */
      setvbuf(stdin, NULL, _IONBF, 0);
      setvbuf(stdout, NULL, _IONBF, 0);
      while(fgets(sin, 4096, stdin)) {
	if(parms->stdin_beep==CHAR_STDIN_BEEP) {
	  for(ptr=sin;*ptr;ptr++) {
	    putchar(*ptr);
	    fflush(stdout);
	    play_beep(*parms);
	  }
	} else {
	  fputs(sin, stdout);
	  play_beep(*parms);
	}
      }
    } else {
      play_beep(*parms);
    }

    /* Junk each parms struct after playing it */
    free(parms);
    parms = next;
  }

  (void) close(console_fd);
  console_fd = -1;

  return EXIT_SUCCESS;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
