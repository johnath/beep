/** \file beep-compiler.h
 * \brief compiler convenience definitions
 * \author Copyright (C) 2018-2019 Hans Ulrich Niedermann
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
 * \defgroup beep_compiler Compiler convenience definitions
 *
 * @{
 *
 */

#ifndef BEEP_COMPILER_H
#define BEEP_COMPILER_H


/** Mark an unused function parameter.
 *
 * Usage:
 *
 *     void foo(int unused UNUSED_PARAM)
 *     {
 *         ...
 *     }
 */
#define UNUSED_PARAM __attribute__(( unused ))


/** Mark a constructor function to be executed once on program startup.
 *
 * Usage:
 *
 *     static void foo(void) CONSTRUCTOR_FUNCTION;
 *     static void foo(void)
 *     {
 *         ...
 *     }
 */
#define CONSTRUCTOR_FUNCTION __attribute__(( constructor ))


/**
 * Dummy declaration to avoid empty compilation unit when checking
 * headers compile by themselves.
 *
 */
extern char do_not_use_this_dummy[];


#endif /* !defined(BEEP_COMPILER_H) */


/** @} */


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
