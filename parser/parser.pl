#!/usr/bin/perl

use strict;
use warnings;
use utf8;

use FindBin;

#
# trigger_word => filename
#
my %apps = (
    '^yasuna'   => 'sasairc-yasuna.pl',
    '^clangsay' => 'clangsay.pl',
);

#
# processing of message
#
sub if_app {
    my $app = "";

    # check special function
    while (my ($key, $value) = each(%apps)) {
        if ($_[0] =~ /$key/) {
            $app = $value;
        }
    }

    return $app;
}

sub main {
    my $qstr = $ARGV[0];
    $qstr =~ tr/+/ /;
    my @query = split(/&/, $qstr);
    my ($app, $key, $val, %req);

    foreach (@query) {
        ($key, $val) = split(/=/, $_);
        $req{$key} = $val;
    }
    if (($app = if_app($req{trigger_word})) eq "") {
        print "nosend";

        return 0;
    }

    exec("$FindBin::Bin/$app", $ARGV[0]);
}

main();
