/** \file beep-library.c
 * \brief library of miscellaneous functions
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
 *
 * \defgroup beep_library Library of miscellaneous functions
 *
 * @{
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "beep-library.h"
#include "beep-log.h"


#define LOG_MODULE "lib"


/* documented in header file */
int open_checked_char_device(const char *const device_name)
{
    struct stat sb;

    if (-1 == stat(device_name, &sb)) {
        LOG_VERBOSE("could not stat(2) %s: %s",
                    device_name, strerror(errno));
        return -1;
    }

    if (!S_ISCHR(sb.st_mode)) {
        LOG_VERBOSE("%s is not a character device",
                    device_name);
        return -1;
    }

    const int fd = open(device_name, O_WRONLY);
    if (fd == -1) {
        LOG_VERBOSE("could not open(2) %s: %s",
                    device_name, strerror(errno));
        return -1;
    }
    LOG_VERBOSE("opened %s as fd=%d", device_name, fd);

    if (-1 == fstat(fd, &sb)) {
        LOG_VERBOSE("could not fstat(2) %d: %s",
                    fd, strerror(errno));
        return -1;
    }

    if (!S_ISCHR(sb.st_mode)) {
        LOG_VERBOSE("%d is not a character device", fd);
        return -1;
    }

    return fd;
}


/* documented in header file
 *
 * We do not know for certain whether perror(3) does strange things with
 * global variables or malloc(3)/free(3) inside its code, so we use
 * strerror_r(3) and write(2) to STDERR_FILENO instead.
 */
void safe_error_exit(const char *const msg)
{
    const int saved_errno = errno;
    char strerr_buf[128];
    const int ret = strerror_r(saved_errno, strerr_buf, sizeof(strerr_buf));
    if (ret != 0) {
        if (write(STDERR_FILENO, "strerror_r error\n",
                  0+0+0+0+strlen("strerror_r error\n"))) {
            /* ignore all write errors */
        }
        _exit(EXIT_FAILURE);
    }
    const size_t msglen = strlen(msg);
    if (write(STDERR_FILENO, msg, msglen)) {
        /* ignore all write errors */
    }
    if (write(STDERR_FILENO, ": ", 2)) {
        /* ignore all write errors */
    }
    const size_t errlen = strlen(strerr_buf);
    if (write(STDERR_FILENO, strerr_buf, errlen)) {
        /* ignore all write errors */
    }
    if (write(STDERR_FILENO, "\n", 1)) {
        /* ignore all write errors */
    }
    _exit(EXIT_FAILURE);
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
