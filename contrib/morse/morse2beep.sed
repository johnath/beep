#!/usr/bin/sed -f
# Turn Morse code into beeps.
#
# Copyright 2020 Hans Ulrich Niedermann <hun@n-dimensional.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Usage:
#   cat text-file.txt | morse | tr '\n' '\r' | sed -f morse2beep.sed | xargs beep
#
# Written as a replacement for Dan Jacobson's morse2beep.pl perl script
# for systems without a perl interpreter.
#
# Note that this does NOT strip the trailing End Of Work (...-.-) symbol.
#
i -l0 -D0
s/ daw\r\r/-n -l300 -D700 /g
s/ dit\r\r/-n -l100 -D700 /g
s/ daw\r/-n -l300 -D300 /g
s/ dit\r/-n -l100 -D300 /g
s/ daw/-n -l300 -D100 /g
s/ dit/-n -l100 -D100 /g
