/* beep-drivers.h - interface to the beep driver infrastructure
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

#include "beep-drivers.h"
#include "beep-log.h"


static
beep_driver *first_driver = NULL;


void beep_drivers_register(beep_driver *driver)
{
    log_verbose("beep_drivers_register %p (%s)",
                (void *) driver, driver->name);
    if (first_driver) {
        driver->next = first_driver;
        first_driver = driver;
    } else {
        driver->next = NULL;
        first_driver = driver;
    }
}


beep_driver *beep_drivers_detect(const char *const console_device)
{
    for (beep_driver *driver = first_driver; driver; driver=driver->next) {
        if (driver->detect(driver, console_device)) {
            return driver;
        }
    }
    return NULL;
}


void beep_drivers_init(beep_driver *driver)
{
    driver->init(driver);
}


void beep_drivers_fini(beep_driver *driver)
{
    driver->fini(driver);
}


void beep_drivers_begin_tone(beep_driver *driver, const uint16_t freq)
{
    driver->begin_tone(driver, freq);
}


void beep_drivers_end_tone(beep_driver *driver)
{
    driver->end_tone(driver);
}


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
