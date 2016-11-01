#!/usr/bin/perl
#
# clangsay
#

use strict;
use warnings;
use utf8;

use JSON;

use Encode;
use URI::Escape;

sub main {
    my $qstr = $ARGV[0];
    $qstr =~ tr/+/ /;
    
    my @query = split(/&/, $qstr);
    my ($str, $key, $val, %req);

    foreach (@query) {
        ($key, $val) = split(/=/, $_);
        $req{$key} = $val;
    }
    $req{text} = uri_unescape($req{text});
    $req{text} =~ s/clangsay//g;

    if ($req{text} =~ /(`|\$|\||&|;|>|<)/) {
        my $data = {
            token   => $req{token},
            text    => "@" . $req{user_name} . " システム関係を触っちゃだめ！",
        };
        print encode_json($data);
    } elsif ($req{text} =~ /\S(.+)/) {
        my @stream = decode_utf8(`clangsay $req{text} 2>&1`);
        foreach my $line (@stream) {
            $str = $str . $line;
        }

        my $data = {
            token   => $req{token},
            text    => '```' . "@" . $req{user_name} . "\n" .  $str . '```',
        };
        print encode_json($data);
    } else {
        my $data = {
            token   => $req{token},
            text    => "@" . $req{user_name} . " せっかくだから何か書きなよ・・・",
        };
        print encode_json($data);
    }

    return 0;
}

main();
