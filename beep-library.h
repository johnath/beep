/** \file beep-library.h
 * \brief library of miscellaneous macros and functions
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
 * \addtogroup beep_library
 *
 * @{
 *
 */


#ifndef BEEP_LIBRARY_H
#define BEEP_LIBRARY_H


/**
 * Open the named character device special file.
 *
 * This makes certain that the returned file descriptor is actually
 * for a character device special file.
 *
 * @param device_name The device name.
 */
int open_checked_char_device(const char *const device_name)
    __attribute__(( nonnull(1) ));


/**
 * Safely exit the program with error message.
 *
 * The "safety" aspect refers to multithreading/signals/etc.
 *
 * @param msg The message to print before exiting.
 */
void safe_error_exit(const char *const msg)
    __attribute__(( nonnull(1) ))
    __attribute__(( noreturn ));


#endif /* !defined(BEEP_LIBRARY_H) */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
