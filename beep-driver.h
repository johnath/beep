/** \file beep-driver.h
 * \brief the interface all beep drivers implement
 * \author Copyright (C) 2019 Hans Ulrich Niedermann
 * \author Copyright (C) 2022 Hans Ulrich Niedermann
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
 * \defgroup beep_driver beep driver definition
 *
 * @{
 *
 */


#ifndef BEEP_DRIVER_H
#define BEEP_DRIVER_H


#include <stdbool.h>
#include <stdint.h>


/**
 * Abstract declaration of beep_driver.
 */
typedef struct beep_driver beep_driver;


/**
 * Driver function to detect whether the driver can actually work.
 */
typedef bool (*beep_driver_detect_func)     (beep_driver *driver,
                                             const char *console_device);

/**
 * Driver function to initialize the driver.
 */
typedef void (*beep_driver_init_func)       (beep_driver *driver);

/**
 * Driver function to destroy and clean up the driver.
 */
typedef void (*beep_driver_fini_func)       (beep_driver *driver);

/**
 * Driver function to have the driver begin a tone.
 */
typedef void (*beep_driver_begin_tone_func) (beep_driver *driver,
                                             const uint16_t freq);

/**
 * Driver function to have the driver end a tone.
 */
typedef void (*beep_driver_end_tone_func)   (beep_driver *driver);


/**
 * Internal beep driver data structure.
 */
struct beep_driver {
    /** Unique name for the driver */
    char        *name;

    /** Link to next driver in linked list of drivers.
     *
     * To be filled out by the beep_drivers_register() function once,
     * and only be read from everywhere else.
     */
    beep_driver *next;

    /** beep driver detect function */
    beep_driver_detect_func     detect;

    /** beep driver init function */
    beep_driver_init_func       init;

    /** beep driver fini function */
    beep_driver_fini_func       fini;

    /** beep driver begin_tone function */
    beep_driver_begin_tone_func begin_tone;

    /** beep driver end_tone function */
    beep_driver_end_tone_func   end_tone;

    /* As long as all drivers need the following data items, we do not
     * need to hide them in the driver implementation.
     */

    /** device file descriptor for this driver */
    int         device_fd;

    /** device file name for this driver */
    const char *device_name;
};


#endif /* !defined(BEEP_DRIVER_H) */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
