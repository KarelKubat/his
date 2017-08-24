#!/usr/bin/env perl

use strict;

die <<"EOF" if ($#ARGV != 2);

Usage: txt2h.pl input.txt output.h DEFINENAME
Makes a #define for you in output.h

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
  print $of ('    ', '"', $line, "\\n", '"', "\\", "\n");
}
print $of ('    ', "\"\\n\"", "\n");

