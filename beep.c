/*  beep - just what it sounds like, makes the console beep - but with
 * precision control.  See the man page for details.
 *
 * Try beep -h for command line args
 *
 * This code is copyright (C) Johnathan Nightingale, 2000.
 *
 * This code may distributed only under the terms of the GNU Public License 
 * which can be found at http://www.gnu.org/copyleft or in the file COPYING 
 * supplied with this code.
 *
 * This code is not distributed with warranties of any kind, including implied
 * warranties of merchantability or fitness for a particular use or ability to 
 * breed pandas in captivity, it just can't be done.
 *
 * Bug me, I like it:  http://johnath.com/  or johnath@johnath.com
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
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


/* Use PIT_TICK_RATE value from the kernel. */
#ifndef CLOCK_TICK_RATE
#define CLOCK_TICK_RATE 1193182UL
#endif

/* The name to use as the name of this program in user messages. */
#define PROG_NAME "beep"

/* Prefix for error messages */
#define PROG_PREFIX PROG_NAME ": "

#define VERSION_STRING "beep-1.3"
char *copyright = 
"Copyright (C) Johnathan Nightingale, 2002.  "
"Use and Distribution subject to GPL.  "
"For information: http://www.gnu.org/copyleft/.";

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

/* global output verbosity */
int verbose = 0;

/* Momma taught me never to use globals, but we need something the signal
   handlers can get at.*/
int console_fd = -1;
beep_type_E console_type = BEEP_TYPE_UNSET;
const char *console_device = NULL;


void do_beep(unsigned int freq) {
  switch (console_type) {
  case BEEP_TYPE_CONSOLE: if (1) {
      const uintptr_t argp = ((freq != 0) ? (CLOCK_TICK_RATE/freq) : freq) & 0xffff;
      if (-1 == ioctl(console_fd, KIOCSOUND, argp)) {
	/* If we cannot use the sound API, we cannot silence the sound either */
	perror("ioctl KIOCSOUND");
	exit(1);
      }
    }
    break;
  case BEEP_TYPE_EVDEV: if (1) {
      struct input_event e;

      memset(&e, 0, sizeof(e));
      e.type = EV_SND;
      e.code = SND_TONE;
      e.value = freq;

      if (sizeof(e) != write(console_fd, &e, sizeof(e))) {
	/* If we cannot use the sound API, we cannot silence the sound either */
	perror("write EV_SND");
	exit(1);
      }
    }
    break;
  case BEEP_TYPE_UNSET:
    /* Do nothing, if this case should ever happen which it should not. */
    break;
  }
}


/* If we get interrupted, it would be nice to not leave the speaker beeping in
   perpetuity. */
void handle_signal(int signum) {
  switch(signum) {
  case SIGINT:
  case SIGTERM:
    if(console_fd >= 0) {
      /* Kill the sound, quit gracefully */
      do_beep(0);
      close(console_fd);
      exit(signum);
    } else {
      /* Just quit gracefully */
      exit(signum);
    }
  }
}


/* print usage and exit */
void usage_bail(void) {
  printf("Usage:\n"

         "  " PROG_NAME " [-f freq] [-l length] [-r reps] [-d delay] "
         "[-D delay] [-s] [-c] [--verbose | --debug] [-e device]\n"

         "  " PROG_NAME " [Options...] [-n] [--new] [Options...] ... \n"
         "  " PROG_NAME " [-h] [--help]\n"
         "  " PROG_NAME " [-v] [-V] [--version]\n");
  exit(1);
}


/* whether character is a digit */
int is_digit(char c)
{
  return (('0' <= c) && (c <= '9'));
}


/* whether string consists of at least one digit, and only digits */
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
void parse_command_line(int argc, char **argv, beep_parms_t *result) {
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
    int argval = -1;    /* handle parsed numbers for various arguments */
    float argfreq = -1; 
    switch(c) {      
    case 'f':  /* freq */
      if(!sscanf(optarg, "%f", &argfreq) || (argfreq >= 20000.0f /* ack! */) ||
	 (argfreq <= 0.0f)) {
        usage_bail();
      } else {
	if (result->freq != 0) {
	  fprintf(stderr, "WARNING: multiple -f values given, only last "
	    "one is used.\n");
	}
	result->freq = ((unsigned int)rintf(argfreq));
      }
      break;
    case 'l' : /* length */
      if(!sscanf(optarg, "%d", &argval) || (argval < 0) || (argval > 2100000))
        usage_bail();
      else
	result->length = argval;
      break;
    case 'r' : /* repetitions */
      if(!sscanf(optarg, "%d", &argval) || (argval < 0) || (argval > 2100000))
        usage_bail();
      else
	result->reps = argval;
      break;
    case 'd' : /* delay between reps - WITHOUT delay after last beep*/
      if(!sscanf(optarg, "%d", &argval) || (argval < 0) || (argval > 2100000))
        usage_bail();
      else {
	result->delay = argval;
	result->end_delay = NO_END_DELAY;
      }
      break;
    case 'D' : /* delay between reps - WITH delay after last beep */
      if(!sscanf(optarg, "%d", &argval) || (argval < 0) || (argval > 2100000))
	usage_bail();
      else {
	result->delay = argval;
	result->end_delay = YES_END_DELAY;
      }
      break;
    case 's' :
      result->stdin_beep = LINE_STDIN_BEEP;
      break;
    case 'c' :
      result->stdin_beep = CHAR_STDIN_BEEP;
      break;
    case 'v' :
    case 'V' : /* also --version */
      printf("%s\n",VERSION_STRING);
      exit(0);
      break;
    case 'n' : /* also --new - create another beep */
      if (result->freq == 0)
	result->freq = DEFAULT_FREQ;
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
      verbose = 1;
      break;
    case 'e' : /* also --device */
      if (1) {
	static char realpath_optarg[PATH_MAX+1];
	if (realpath(optarg, realpath_optarg) == NULL) {
	  const int saved_errno = errno;
          fprintf(stderr, PROG_PREFIX
		  "could not run realpath(3) on '%s': %s\n",
                  optarg, strerror(saved_errno));
	  exit(EXIT_FAILURE);
	}
	if (is_device_whitelisted(realpath_optarg)) {
	  console_device = realpath_optarg;
	} else {
          fprintf(stderr, PROG_PREFIX
		  "Not using device '%s'. If you do need this device, please "
		  "report that fact to <https://github.com/ndim/beep/issues>.\n",
                  realpath_optarg);
	  exit(EXIT_FAILURE);
	}
      }
      break;
    case 'h' : /* notice that this is also --help */
    default :
      usage_bail();
    }
  }
  if (result->freq == 0)
    result->freq = DEFAULT_FREQ;
}  

void play_beep(beep_parms_t parms) {
  unsigned int i; /* loop counter */

  if (verbose == 1) {
    fprintf(stderr, "[DEBUG] %d times %d ms beeps (%d delay between, "
	    "%d delay after) @ %d Hz\n",
	    parms.reps, parms.length, parms.delay, parms.end_delay, parms.freq);
  }

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
int open_chr(const char *filename, int flags)
{
  struct stat sb;
  if (-1 == stat(filename, &sb)) {
    return -1;
  }
  if (S_ISCHR(sb.st_mode)) {
    return open(filename, flags);
  } else {
    fprintf(stderr, PROG_PREFIX
	    "console file '%s' is not a character device special file\n",
	    filename);
    exit(1);
  }
}


int main(int argc, char **argv) {
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
    fprintf(stderr,
            PROG_PREFIX "Running setuid or setgid, "
            "which is not supported for security reasons.\n"
            PROG_PREFIX
            "Set up permissions for the pcspkr evdev device file instead.\n");
    exit(1);
  }

  /* Bail out if running under sudo.
   *
   * For the reasoning, see the setuid comment above.
   */
  if (getenv("SUDO_COMMAND") || getenv("SUDO_USER") || getenv("SUDO_UID") || getenv("SUDO_GID")) {
    fprintf(stderr,
            PROG_PREFIX "Running under sudo, "
            "which is not supported for security reasons.\n"
            PROG_PREFIX
            "Set up permissions for the pcspkr evdev device file instead.\n");
    exit(1);
  }

  /* Parse command line */
  beep_parms_t *parms = (beep_parms_t *)malloc(sizeof(beep_parms_t));
  if (NULL == parms) {
    perror("malloc");
    exit(1);
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
    fprintf(stderr, PROG_PREFIX "Could not open %s for writing: %s\n",
	    ((console_device != NULL) ? console_device : "console device"),
	    strerror(saved_errno));
    /* Output the only beep we can, in an effort to fall back on usefulness */
    printf("\a");
    exit(1);
  }

  /* Verify that console_fd is actually a character device special file */
  if (1) {
    struct stat sb;
    if (-1 == fstat(console_fd, &sb)) {
      perror("fstat");
      exit(1);
    }
    if (S_ISCHR(sb.st_mode)) {
      /* GOOD: console_fd is a character device special file. Use it. */
    } else {
      /* BAD: console_fd is not a character device special file. Do
       * not use it any further, and certainly DO NOT WRITE to it.
       */
      fprintf(stderr,
	      "%s: opened console '%s' is not a character special device\n",
	      argv[0], console_device);
      exit(1);
    }
  }

  /* Determine the API supported by the opened console device */
  if (ioctl(console_fd, EVIOCGSND(0)) != -1) {
    if (verbose) {
      printf("Using BEEP_TYPE_EVDEV on '%s'\n", console_device);
    }
    console_type = BEEP_TYPE_EVDEV;
  } else if (ioctl(console_fd, KIOCSOUND, 0) >= 0) {
    /* turning off the beeps should be a safe way to check for API support */
    if (verbose) {
      printf("Using BEEP_TYPE_CONSOLE on '%s'\n", console_device);
    }
    console_type = BEEP_TYPE_CONSOLE;
  } else {
    fprintf(stderr,
	    "%s: console device '%s' does not support either API\n",
	    argv[0], console_device);
    /* Output the only beep we can, in an effort to fall back on usefulness */
    printf("\a");
    exit(1);
  }

  /* At this time, we know what API to use on which device, and we do
   * not have to fall back onto printing '\a' any more.
   */

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

  close(console_fd);
  console_fd = -1;

  return EXIT_SUCCESS;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
