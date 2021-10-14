#!/usr/bin/env perl
use strict;
use warnings;
use feature 'say';


open(my $fh, "<$ARGV[0]") or die "Can't open '$ARGV[0]': $!";
my $s = do { local $/; <$fh> };
while ($s =~ /(?:\[(=*)\[(?:.|\n)+?\]\1\](?:.|\n)+?)*^\K[ \t]*?(?!end(?:if|foreach|function|macro))\w+(?=\s*?\()/gm) {
    seek($fh, 0, 0);
    my $prev;
    read($fh, $prev, pos $s);
    my $linenum = $prev =~ tr/\n//;
    say $linenum + 1;
}
