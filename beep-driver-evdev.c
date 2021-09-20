/** \file beep-driver-evdev.c
 * \brief implement the beep evdev driver
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
 * \defgroup beep_driver_evdev Linux evdev API driver
 * \ingroup beep_driver
 *
 * @{
 *
 */


#include <stddef.h>

#include <errno.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <linux/input.h>


#include "beep-compiler.h"
#include "beep-drivers.h"

#include "beep-library.h"
#include "beep-log.h"


#define LOG_MODULE "evdev"


static
int open_checked_device(const char *const device_name)
{
    const int fd = open_checked_char_device(device_name);
    if (fd == -1) {
        return -1;
    }

    if (-1 == ioctl(fd, EVIOCGSND(0))) {
        LOG_VERBOSE("%d does not implement EV_SND API", fd);
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
        static
            const char *const default_name =
            "/dev/input/by-path/platform-pcspkr-event-spkr";
        const int fd = open_checked_device(default_name);
        if (fd >= 0) {
            driver->device_fd = fd;
            driver->device_name = default_name;
            return true;
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

    struct input_event e;

    memset(&e, 0, sizeof(e));
    e.type = EV_SND;
    e.code = SND_TONE;
    e.value = freq;

    if (sizeof(e) != write(driver->device_fd, &e, sizeof(e))) {
        /* If we cannot use the sound API, we cannot silence the sound either */
        safe_error_exit("write EV_SND");
    }
}


static
void driver_end_tone(beep_driver *driver)
{
    LOG_VERBOSE("driver_end_tone %p", (void *)driver);

    struct input_event e;

    memset(&e, 0, sizeof(e));
    e.type = EV_SND;
    e.code = SND_TONE;
    e.value = 0;

    if (sizeof(e) != write(driver->device_fd, &e, sizeof(e))) {
        safe_error_exit("write EV_SND");
    }
}


static
beep_driver driver_data =
    {
     "evdev",
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
void beep_driver_evdev_constructor(void)
    CONSTRUCTOR_FUNCTION;

static
void beep_driver_evdev_constructor(void)
{
    LOG_VERBOSE("beep_driver_evdev_constructor");
    beep_drivers_register(&driver_data);
}


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
