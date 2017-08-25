#!/usr/bin/env perl

use strict;

my $ARGLEN = 5;                   # max length of one arg
my $CMDLEN = 5;                   # nr of args in 1 cmd
my $DB     = '/tmp/loadtest.db';  # db to use
my $NCMDS  = 180000000;           # nr of cmds to generate: 1 per second,
                                  # for 8 hrs a day, 5 days a week,
				  # 25 weeks per year, for 50 years


sub genstr($) {
  my $count = shift;
  my $ret;
  for (1..$count) {
    $ret .= chr(int(rand(25) + 97));
  }
  return $ret;
}

sub gencmd($) {
  my $maxlen = shift;
  my $ret;

  # First one is a three letter thing.
  $ret = genstr(3);

  for my $count (1..rand($maxlen)) {
    $ret .= " " . genstr($ARGLEN);
  }
  return $ret;
}

sub dbsize() {
  my @statres = stat($DB) or return 0;
  return $statres[7];
}

sub progress($$$$$) {
  printf("Added %8d commands with total length %10d, dbsize %10d in %4d sec, step %d\n", @_);
}
  

# main
unlink($DB);
my $totlen = 0;
my $prevtime = time();
my $starttime = $prevtime;
for my $i (1..$NCMDS) {
  my $cmd = gencmd($CMDLEN);

  system("/tmp/his -d$DB -F3 -a '$i $cmd'") and die("his failed");
  $totlen += length($cmd);

  if ($i % 100 == 0) {
    progress($i, $totlen, dbsize(), time() - $starttime, time() - $prevtime);
    $prevtime = time();
  }
}

# At the end
progress($NCMDS, $totlen, dbsize(), time() - $starttime, time() - $prevtime);

