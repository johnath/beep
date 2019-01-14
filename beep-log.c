/* beep-log.c - implement logging and message output
 * Copyright (C) 2018-2019 Hans Ulrich Niedermann
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
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "beep-log.h"


int log_level = 0;


const char *progname = "beep-log";


/** beep-log internal function to actually print a message */
static
void log_internal_vf(const char *levelstr, const char *const format, va_list args)
    __attribute__ ((nonnull (1,2)));


static
void log_internal_vf(const char *levelstr, const char *const format, va_list args)
{
    fprintf(stdout, "%s: %s: ", progname, levelstr);
    vfprintf(stdout, format, args);
    fputc('\n', stdout);
}


void log_output(const char *const format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}


void log_error(const char *const format, ...)
{
    va_list args;

    va_start(args, format);
    log_internal_vf("Error", format, args);
    va_end(args);
}


void log_warning(const char *const format, ...)
{
    va_list args;

    va_start(args, format);
    log_internal_vf("Warning", format, args);
    va_end(args);
}

void log_verbose(const char *const format, ...)
{
    va_list args;

    if (log_level > 0) {
        va_start(args, format);
        log_internal_vf("Verbose", format, args);
        va_end(args);
    }
}


/** Log a range of data */
void log_data(const void *const buf, const size_t start_ofs, const size_t size)
{
    if (log_level <= 1) {
        return;
    }
    const unsigned char *const ucbuf = buf;
    static const char hexchar[] = "0123456789abcdef";
    char linebuf[80] =
        "0000  __ __ __ __ __ __ __ __-__ __ __ __ __ __ __ __  ........-........";
    for (size_t line_ofs=0, remaining=size; line_ofs<size; line_ofs+=16, remaining-=16) {
        linebuf[0]    = hexchar[(line_ofs>>12) & 0xf];
        linebuf[1]    = hexchar[(line_ofs>> 8) & 0xf];
        linebuf[2]    = hexchar[(line_ofs>> 4) & 0xf];
        linebuf[3]    = hexchar[(line_ofs>> 0) & 0xf];
        const size_t line_cols = (remaining<16)? remaining : 16;
        for (size_t col_ofs=0; col_ofs<line_cols; ++col_ofs) {
            const size_t ofs = start_ofs + line_ofs + col_ofs;
            const unsigned char ch = ucbuf[ofs];
            const char hex_hi = hexchar[(ch >> 4) & 0x0f];
            const char hex_lo = hexchar[(ch >> 0) & 0x0f];
            linebuf[6+3*col_ofs+0]    = hex_hi;
            linebuf[6+3*col_ofs+1]    = hex_lo;
            linebuf[6+3*16+1+col_ofs+(col_ofs/8)] = ((0x20 <= ch) && (ch <= 0x7f)) ? (char)ch : '.';
        }
        for (size_t col_ofs=line_cols; col_ofs<16; ++col_ofs) {
            linebuf[6+3*col_ofs+0]    = ' ';
            linebuf[6+3*col_ofs+1]    = ' ';
            linebuf[6+3*16+1+col_ofs+(col_ofs/8)] = ' ';
        }
        if (line_cols <= 8) {
            linebuf[6+3*8-1]    = ' ';
            linebuf[6+3*16+1+8] = ' ';
        }
        fprintf(stdout, "%s: %s: %s\n", progname, "Data", linebuf);
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
