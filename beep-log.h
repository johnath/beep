/** \file beep-log.h
 * \brief interface to logging and message output
 * \author Copyright (C) 2018-2019 Hans Ulrich Niedermann
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
 * \addtogroup beep_log
 *
 * @{
 *
 */


#ifndef BEEP_LOG_H
#define BEEP_LOG_H


#include <stdlib.h>


/** Currently active log level.
 *
 * Default is 0. Values greater than 0 are for verbose output.
 */
extern int log_level;


/** Program name to use in log messages.
 *
 * Set this first thing in main(). Either just set it to some constant
 * string, or run log_init() to set this up from the non-path part of
 * argv[0].
 */
extern const char *progname;


/** Write a standard message */
void log_output(const char *const format, ...)
    __attribute__(( nonnull(1) ))
    __attribute__(( format(printf, 1, 2) ));


/** Log an error message */
#define LOG_ERROR(...) log_error(__VA_ARGS__)

/** Log an error message */
void log_error(const char *const format, ...)
    __attribute__(( nonnull(1) ))
    __attribute__(( format(printf, 1, 2) ));


/** Log a warning message */
#define LOG_WARNING(...) log_warning(LOG_MODULE, __VA_ARGS__)

/** Log a warning message */
void log_warning(const char *const module,
                 const char *const format, ...)
    __attribute__(( nonnull(1, 2) ))
    __attribute__(( format(printf, 2, 3) ));


/** Log a verbose message */
#define LOG_VERBOSE(...) log_verbose(LOG_MODULE, __VA_ARGS__)

/** Log a verbose message */
void log_verbose(const char *const module,
                 const char *const format, ...)
    __attribute__(( nonnull(1, 2) ))
    __attribute__(( format(printf, 2, 3) ));


/** Log a range of data */
void log_data(const void *const buf, const size_t start_ofs, const size_t size)
    __attribute__(( nonnull(1) ));


/** Initialize the log message prefix from the non-path part of argv[0]. */
void log_init(const int argc, char *const argv[])
    __attribute__(( nonnull(2) ));


#endif /* !defined(BEEP_LOG_H) */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
