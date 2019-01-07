/* beep-driver.h - the interface all beep drivers implement
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


#ifndef BEEP_DRIVER_H
#define BEEP_DRIVER_H


#include <stdbool.h>
#include <stdint.h>


typedef struct _beep_driver beep_driver;


typedef bool (*beep_driver_detect_func)     (beep_driver *driver,
                                             const char *console_device);
typedef void (*beep_driver_init_func)       (beep_driver *driver);
typedef void (*beep_driver_fini_func)       (beep_driver *driver);
typedef void (*beep_driver_begin_tone_func) (beep_driver *driver,
                                             const uint16_t freq);
typedef void (*beep_driver_end_tone_func)   (beep_driver *driver);


struct _beep_driver {
    char        *name;
    beep_driver *next;

    beep_driver_detect_func     detect;
    beep_driver_init_func       init;
    beep_driver_fini_func       fini;
    beep_driver_begin_tone_func begin_tone;
    beep_driver_end_tone_func   end_tone;

    /* As long as all drivers need these data items, we do not need to
     * hide them in the driver implementation.
     */
    int         device_fd;
    const char *device_name;
};


#endif /* BEEP_DRIVER_H */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
