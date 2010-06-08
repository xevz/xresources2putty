#!/usr/bin/perl -w

#
# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 42):
# <mikael@distopic.net> wrote this file. As long as you retain this notice you
# can do whatever you want with this stuff. If we meet some day, and you think
# this stuff is worth it, you can buy me a beer in return Mikael Svantesson
# ----------------------------------------------------------------------------
#

use strict;
use warnings;

use FindBin '$Bin';

use Carp;

my $RGB     = '/usr/share/X11/rgb.txt';
my $LITERAL = "$Bin/../src/literal_colors.h";

exit 1 unless (-r $RGB);

open my $literal, '>', $LITERAL || croak "Could not open $LITERAL.";

print {$literal} <<EOF;
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <mikael\@distopic.net> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Mikael Svantesson
 * ----------------------------------------------------------------------------
 */

/*
 * Generated using scripts/generate_literal.pl
 */

#ifndef LITERAL_COLORS_H
#define LITERAL_COLORS_H 1

struct literal_color {
    const char *name;
    const long  color;
};

struct literal_color literal_colors[] = { 
EOF

open my $rgb, '<', $RGB || croak "Could not open $RGB.";

while (my $line = <$rgb>) {
	next unless ($line =~ /\s*(\d+)\s+(\d+)\s+(\d+)\s+(.+)/);

	printf {$literal} "\t{ \"%s\", 0x%x%x%x },\n", $4, $1, $2, $3;
}

close $rgb;

print {$literal} <<EOF;
};

#define LITERAL_COLORS_LENGTH (sizeof(literal_colors) / sizeof(struct literal_color))

#endif

EOF

close $literal;

