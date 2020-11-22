#!/usr/bin/perl
# Turn Morse code into beeps.
#
# Copyright 2020 Dan Jacobson (https://www.jidanni.org/)
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
# echo cq cq cq sos | /usr/games/morse -s | perl morse2beep [milliseconds for "."] | xargs beep
#
# Note that this strips the trailing End Of Work (...-.-) symbol.

use strict;
use warnings FATAL => q(all);
## https://en.wikipedia.org/wiki/Morse_code#Representation,_timing,_and_speeds
my $short    = pop || 100;
my $long     = $short * 3;
my $word_gap = $short * 7;
my @elements;
sub d { die "Expected input from bsdgames package's /usr/games/morse -s." }
while (<>) {
    chomp;
    s/^ // or d;
    if    (/^\.\.\.-\.-$/) { die "Didn't get any input!" unless @elements; last; }
    elsif (/^$/)       { $elements[-1]{delay} += $word_gap if @elements; }
    else {
        for ( split // ) {
            if    ( $_ eq "." ) { push @elements, { lenght => $short, delay => $short }; }
            elsif ( $_ eq "-" ) { push @elements, { lenght => $long, delay => $short }; }
            else                { d; }
        }
        $elements[-1]{delay} = 0;
    }
}
for (@elements) { $_->{delay} = $long unless $_->{delay} }
my @strings;
push @strings, sprintf "-l %d -D %d", $_->{lenght}, $_->{delay} for @elements;
print join " -n ", @strings;
## Errors even generate one beep nicely, due to xargs beep!
