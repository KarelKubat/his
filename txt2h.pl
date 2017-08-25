#!/usr/bin/env perl

use strict;

die <<"EOF" if ($#ARGV != 3);

Usage: txt2h.pl input.txt output.h DEFINENAME REPLACEPERCENT

Makes a #define for you in output.h
If REPLACEPERCENT is nonzero then % is changed to %%, useful for printf()
fmt strings.

EOF

open(my $if,   $ARGV[0])  or die("Cannot read $ARGV[0]: $!\n");
open(my $of, ">$ARGV[1]") or die("Cannot write $ARGV[1]: $!\n");

print $of <<"TOP";
/* Auto-generated from $ARGV[0]. Do not edit. */
/* Instead adapt $ARGV[0] and run 'make $ARGV[1]' */

#define $ARGV[2] \\\
TOP

while (my $line = <$if>) {
  chomp($line);
  $line =~ s{"}{\\"}g;
  $line =~ s{'}{\\'}g;
  $line =~ s{%}{%%}g if ($ARGV[3] != 0);
  print $of ('    ', '"', $line, "\\n", '"', "\\", "\n");
}
print $of ('    ', "\"\\n\"", "\n");

