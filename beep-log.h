#ifndef BEEP_LOG_H
#define BEEP_LOG_H

/** Currently active log level.
 *
 * Default is 0. Values greater than 0 are for verbose output.
 */
int log_level;


/** Program name to use in log messages.
 *
 * Set this first thing in main(). Either just set it to some constant
 * string, or run log_init() to set this up from the non-path part of
 * argv[0].
 */
const char *progname;


/** Write a standard message */
void log_output(const char *format, ...)
    __attribute__ ((nonnull (1)))
    __attribute__ ((format (printf, 1, 2)));


/** Log an error message */
void log_error(const char *fmt, ...)
    __attribute__ ((nonnull (1)))
    __attribute__ ((format (printf, 1, 2)));


/** Log a warning message */
void log_warning(const char *fmt, ...)
    __attribute__ ((nonnull (1)))
    __attribute__ ((format (printf, 1, 2)));


/** Log a verbose message */
void log_verbose(const char *fmt, ...)
    __attribute__ ((nonnull (1)))
    __attribute__ ((format (printf, 1, 2)));


/** Initialize the log message prefix from the non-path part of argv[0]. */
void log_init(const int argc, char *const argv[])
    __attribute__ ((nonnull (2)));


#endif /* BEEP_LOG_H */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
