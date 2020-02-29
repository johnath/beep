/** \file beep-types.h
 * \brief Some types used internally for beeps.
 *
 * \author Copyright (C) 2000-2010 Johnathan Nightingale
 * \author Copyright (C) 2010-2013 Gerfried Fuchs
 * \author Copyright (C) 2013-2018 Hans Ulrich Niedermann
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
 */

#ifndef BEEP_TYPES_H
#define BEEP_TYPES_H


/**
 * Whether to to delay after the current repetition set of tones.
 */
typedef enum
    {
     END_DELAY_NO = 0,
     END_DELAY_YES = 1,
    } end_delay_E;


/**
 * Whether to and how to react to data from stdin.
 */
typedef enum
    {
     STDIN_BEEP_NONE = 0,
     STDIN_BEEP_LINE = 1,
     STDIN_BEEP_CHAR = 2,
    } stdin_beep_E;


#endif /* !defined(BEEP_TYPES_H) */
