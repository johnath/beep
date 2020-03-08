/** \file beep-drivers.h
 * \brief interface to the beep driver infrastructure
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
 * \addtogroup beep_drivers
 *
 * @{
 *
 */


#ifndef BEEP_DRIVERS_H
#define BEEP_DRIVERS_H


#include "beep-compiler.h"
#include "beep-driver.h"


/**
 * Register a beep driver with the beep driver infrastructure.
 *
 * To be called from driver's CONSTRUCTOR function which means once
 * per program invocation.
 *
 * @param driver The driver to register.
 */
void beep_drivers_register(beep_driver *driver)
    __attribute__(( nonnull(1) ));


/**
 * Return the first driver whose detect() function returns true.
 *
 * @param device_name The device name.
 *
 * @return NULL if no driver has been detected.
 * @return Otherwise, the detected driver.
 */
beep_driver *beep_drivers_detect(const char *const device_name);


/**
 * Initialize the beep driver via driver's init() function.
 *
 * @param driver The beep driver.
 */
void beep_drivers_init(beep_driver *driver)
    __attribute__(( nonnull(1) ));


/**
 * Deinitialize the beep driver via driver's fini() function.
 *
 * @param driver The beep driver.
 */
void beep_drivers_fini(beep_driver *driver)
    __attribute__(( nonnull(1) ));


/**
 * Call the beep driver's begin_tone() function.
 *
 * @param driver The beep driver.
 * @param freq   The frequency to play.
 */
void beep_drivers_begin_tone(beep_driver *driver, const uint16_t freq)
    __attribute__(( nonnull(1) ));


/**
 * Call the beep driver's end_tone() function.
 *
 * @param driver The beep driver.
 */
void beep_drivers_end_tone(beep_driver *driver)
    __attribute__(( nonnull(1) ));


#endif /* !defined(BEEP_DRIVERS_H) */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
