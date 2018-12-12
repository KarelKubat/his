#!/usr/bin/env perl

use strict;

die <<"END" if ($#ARGV != 0);

Usage: import-bash-history.pl FILE
The FILE is usually ~/.bash_history.

END

my $consecutive_errors = 0;
sub import_cmd($$) {
  my ($t, $c) = @_;
  return if (!$t or $c =~ m{^\s*$});
  print("[$t] $c\n");
  if (system('his', '-a', '-F3', "$t $c")) {
    warn("$t $c failed\n");
    die("too many errors\n") if (++$consecutive_errors > 10);
  }
} 

open(my $if, $ARGV[0]) or die("Cannot read $ARGV[0]: $!\n");
my $timestamp = 0;
while (my $line = <$if>) {
  chomp($line);
  if ($line =~ m{^#([0-9]+)}) {
    $timestamp = $1;
  } else {
    import_cmd($timestamp, $line);
  }
}
