#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "beep-log.h"


int log_level = 0;


const char *progname = "beep-log";


/** beep-log internal function to actually print a message */
static
void log_internal_vf(const char *levelstr, const char *format, va_list args)
    __attribute__ ((nonnull (1,2)));


static
void log_internal_vf(const char *levelstr, const char *format, va_list args)
{
    fprintf(stdout, "%s: %s: ", progname, levelstr);
    vfprintf(stdout, format, args);
}


void log_output(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}


void log_error(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    log_internal_vf("Error", format, args);
    va_end(args);
}


void log_warning(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    log_internal_vf("Warning", format, args);
    va_end(args);
}

void log_verbose(const char *format, ...)
{
    va_list args;

    if (log_level > 0) {
        va_start(args, format);
        log_internal_vf("Verbose", format, args);
        va_end(args);
    }
}


void log_init(const int argc, char *const argv[]) {
    /* if argv[0] is "./foo/bar/beep", set progname to "beep" */
    if (argc >= 1) {
        const char *last_slash = strrchr(argv[0], '/');
        if (last_slash) {
            const char *post_last_slash = last_slash+1;
            if (post_last_slash) {
                progname = post_last_slash;
            }
        } else {
            progname = argv[0];
        }
    }
}


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
