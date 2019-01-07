/* beep-driver-noop.c - implement the beep noop driver
 * Copyright (C) 2019 Hans Ulrich Niedermann
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


#include <stddef.h>

#include "beep-driver-noop.h"
#include "beep-log.h"


static
bool driver_detect(beep_driver *driver, const char *const console_device)
{
    if (console_device) {
        log_verbose("noop driver_detect %p %s",
                    (void *)driver, console_device);
    } else {
        log_verbose("noop driver_detect %p %p",
                    (void *)driver, (const void *)console_device);
    }
    return false;
}


static
void driver_init(beep_driver *driver)
{
    log_verbose("noop driver_init %p", (void *)driver);
}


static
void driver_fini(beep_driver *driver)
{
    log_verbose("noop driver_fini %p", (void *)driver);
}


static
void driver_begin_tone(beep_driver *driver, const uint16_t freq)
{
    log_verbose("noop driver_begin_tone %p %u", (void *)driver, freq);
}


static
void driver_end_tone(beep_driver *driver)
{
    log_verbose("noop driver_end_tone %p", (void *)driver);
}


beep_driver noop_driver =
    {
     "noop",
     NULL,
     driver_detect,
     driver_init,
     driver_fini,
     driver_begin_tone,
     driver_end_tone,
     0,
     NULL
    };


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
