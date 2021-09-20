/** \file beep-driver-console.c
 * \brief implement the beep console driver
 * \author Copyright (C) 2000-2010 Johnathan Nightingale
 * \author Copyright (C) 2010-2013 Gerfried Fuchs
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
 * \defgroup beep_driver_console Linux console API driver
 * \ingroup beep_driver
 *
 * @{
 *
 */


#include <stddef.h>
#include <unistd.h>

#include <linux/kd.h>
#include <sys/ioctl.h>


#include "beep-compiler.h"
#include "beep-drivers.h"

#include "beep-library.h"
#include "beep-log.h"


#define LOG_MODULE "console"


#ifndef CLOCK_TICK_RATE
/** The clock tick rate for calculating the PWM counter for the PIT from the frequency.
 *
 * Using the PIT_TICK_RATE value from the kernel.
 */
#define CLOCK_TICK_RATE 1193182UL
#endif /* !defined(CLOCK_TICK_RATE) */


static
int open_checked_device(const char *const device_name)
{
    const int fd = open_checked_char_device(device_name);
    if (fd == -1) {
        return -1;
    }

    if (-1 == ioctl(fd, KIOCSOUND, 0)) {
        LOG_VERBOSE("%d does not implement KIOCSOUND API", fd);
        return -1;
    }

    return fd;
}


static
bool driver_detect(beep_driver *driver, const char *console_device)
{
    if (console_device) {
        LOG_VERBOSE("driver_detect %p %s",
                    (void *)driver, console_device);
    } else {
        LOG_VERBOSE("driver_detect %p %p",
                    (void *)driver, (const void *)console_device);
    }
    if (console_device) {
        const int fd = open_checked_device(console_device);
        if (fd >= 0) {
            driver->device_fd = fd;
            driver->device_name = console_device;
            return true;
        }
    } else {
        /** \todo We could iterate over all `/dev/tty[0-9]+` and
         *        `/dev/vc/[0-9]+` devices until one is a character
         *        device special file which can be `open(2)`ed with
         *        `O_WRONLY`. (We already have that code in
         *        `issue-6-benchmark.c`.)  Is that advisable from a
         *        robustness and/or security point of view?
         */
        static
            const char *const console_device_list[] =
            {
             "/dev/tty0",
             "/dev/vc/0",
            };
        for (size_t i=0; i<(sizeof(console_device_list)/sizeof(console_device_list[0])); ++i) {
            const char *const device_name = console_device_list[i];
            const int fd = open_checked_device(device_name);
            if (-1 != fd) {
                driver->device_fd = fd;
                driver->device_name = device_name;
                return true;
            }
        }
    }
    return false;
}


static
void driver_init(beep_driver *driver)
{
    LOG_VERBOSE("driver_init %p", (void *)driver);
}


static
void driver_fini(beep_driver *driver)
{
    LOG_VERBOSE("driver_fini %p", (void *)driver);
    close(driver->device_fd);
    driver->device_fd = -1;
}


static
void driver_begin_tone(beep_driver *driver, const uint16_t freq)
{
    LOG_VERBOSE("driver_begin_tone %p %u", (void *)driver, freq);
    const uintptr_t argp = ((freq != 0) ? (CLOCK_TICK_RATE/freq) : freq) & 0xffff;
    if (-1 == ioctl(driver->device_fd, KIOCSOUND, argp)) {
        /* If we cannot use the sound API, we cannot silence the sound either */
        safe_error_exit("ioctl KIOCSOUND");
    }
}


static
void driver_end_tone(beep_driver *driver)
{
    LOG_VERBOSE("driver_end_tone %p", (void *)driver);
    if (-1 == ioctl(driver->device_fd, KIOCSOUND, 0)) {
        safe_error_exit("ioctl KIOCSOUND");
    }
}


static
beep_driver driver_data =
    {
     "console",
     NULL,
     driver_detect,
     driver_init,
     driver_fini,
     driver_begin_tone,
     driver_end_tone,
     0,
     NULL
    };


static
void beep_driver_console_constructor(void)
    CONSTRUCTOR_FUNCTION;

static
void beep_driver_console_constructor(void)
{
    LOG_VERBOSE("beep_driver_console_constructor");
    beep_drivers_register(&driver_data);
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
