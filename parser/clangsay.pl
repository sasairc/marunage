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

sub replace {
    my $str = $_[0];
    my @reg = (
            '"', '\'', '`', '\$', '\|', '&', ';', '>', '<', '\*',
            '\(', '\)', '=', '\?',
    );
    my @rep = (
            '\"', '\\\'', '\`', '\$', '\|', '\&', '\;', '\>', '\<', '\*',
            '\(', '\)', ' =', '\?',
    );
    $str =~ s/&amp;/&/g;
    $str =~ s/&quot;/"/g;
    $str =~ s/&lt;/</g;
    $str =~ s/&gt;/>/g;
    $str =~ s/<BR>/\n/g;

    for (my $i = 0; $i < ($#reg + 1); $i++) {
        $str =~ s/$reg[$i]/$rep[$i]/g;
    }

    return $str;
}

sub main {
    my $timeout = 3;
    my @stream  = "";
    my $qstr    = $ARGV[0];
    $qstr       =~ tr/+/ /;
    
    my @query = split(/&/, $qstr);
    my ($str, $key, $val, %req);

    foreach (@query) {
        ($key, $val) = split(/=/, $_);
        $req{$key} = $val;
    }
    $str = uri_unescape($req{text});
    $str =~ s/(\s|　)*clangsay(\s|　)*//g;
    if ($str eq "") {
        print "nosend";

        return 0;
    }
    $str = replace($str);

    if ($str =~ /\S+/) {
        eval {  
            local $SIG{ALRM} = sub{die};
            alarm($timeout);
            @stream = decode_utf8(`clangsay $str 2>&1`);
            alarm 0;
        };
        if ($@) {
            print "nosend";

            return 0;
        } else {
            $str = "";
            foreach my $line (@stream) {
                $str = $str . $line;
            }
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
