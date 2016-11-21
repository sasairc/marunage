#!/usr/bin/perl

use strict;
use warnings;
use utf8;

use Encode;
use URI::Escape;
use JSON;

sub main {
    my $qstr = $ARGV[0];
    $qstr =~ tr/+/ /;
    
    my @query = split(/&/, $qstr);
    my ($str, $key, $val, %req);

    foreach (@query) {
        ($key, $val) = split(/=/, $_);
        $req{$key} = $val;
    }
    $str = uri_unescape($req{text});
    $str = decode_utf8($str);

    if ($str =~ /^はい$/) {
        my $data = {
            token   => $req{token},
            text    => "@" . $req{user_name} . " はいじゃないが",
        };
        print encode_json($data);
    } else {
        print "nosend";
    }

    return 0;
}

main();
