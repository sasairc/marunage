#!/usr/bin/perl
#
# yasunaのslack outgoing webhooks 移植
#

use strict;
use warnings;
use utf8;

use Encode;
use FindBin;
use URI::Escape;
use JSON;

#
# special functions
#
{
    my  $str = "";

    # ping pong
    sub ping {
        $str = "pong";

        return $str;
    }

    # revision slack_yasuna
    sub revision {
        my $sha = decode_utf8(`git -C $FindBin::Bin rev-parse HEAD`);
        
        $str = "sasairc_yasuna(slack outgoing webhooks): $sha";

        return $str;
    }
    
    # system uptime
    sub uptime {
        my $hostname    = decode_utf8(`hostname`);
        my $uptime      = decode_utf8(`uptime`);

        chomp($hostname);
        $str = $hostname . ": " . $uptime;

        return $str;
    }

    # encode n_cipher
    sub encode_n_cipher {
        ($str = $_[0]) =~ /encode\s/;
        $str = uri_unescape($str);
        $str = decode_utf8(`n_cipher_encode "$'"`);
        if ($?) {
            $str = "暗号になってない！！";
        }

        return $str;
    }

    # decode n_cipher
    sub decode_n_cipher {
        ($str = $_[0]) =~ /decode\s/;
        $str = decode_utf8(uri_unescape($str));
        $str = decode_utf8(`n_cipher_decode "$'"`);
        if ($?) {
            $str = "暗号になってない！！";
        }

        return $str;
    }

    # yasuna will talk
    sub yasuna_talk {
        $str = decode_utf8(`yasuna`);
    }

    # yasuna --number N option
    sub yasuna_number {
        my  $max    = `yasuna -l | wc -l`;
        our @number = split(/(number|n)\s/, $_[0]);
        our $arrnum = @number - 1;

        chomp($max);
        chomp(@number);

        if ($number[$arrnum] < $max) {
            $str = decode_utf8(`yasuna -n $number[$arrnum]`);
        } else {
            $str = "numberは $max 以内で指定して下さい";
        }

        return $str;
    }

    # yasuna --version option
    sub yasuna_version {
        $str = decode_utf8(`yasuna --version`);

        return $str;
    }
}

#
# regex/function table
#
my %regex = (
    'ping$'                 => \&ping,
    'revision$'             => \&revision,
    'uptime$'               => \&uptime,
    'encode\s(.+)'          => \&encode_n_cipher,
    'decode\s(.+)'          => \&decode_n_cipher,
    'talk(?:.*)\z'          => \&yasuna_talk,
    '(number|n)\s[0-9]+$'   => \&yasuna_number,
    'version$'              => \&yasuna_version,
);

#
# processing of message
#
sub if_message_type {
    my $str = "";

    # check special function
    while (my ($key, $value) = each(%regex)) {
        if ($_[0] =~ /$key/) {
            $str = $value->($_[0]);
        }
    }
    if ($str eq "") {
#       $str = "nosend";
        $str = yasuna_talk();
    }

    return $str;
}

sub main {
    my $qstr = $ARGV[0];
    $qstr =~ /yasuna\s(.+)/;
    $qstr =~ tr/+/ /;
    
    my @query = split(/&/, $qstr);
    my ($str, $key, $val, %req);

    foreach (@query) {
        ($key, $val) = split(/=/, $_);
        $req{$key} = $val;
    }

    if (($str = if_message_type($req{text})) eq "nosend") {
        print $str;

        return 0;
    }

    my $data = {
        token   => $req{token},
        text    => "@" . $req{user_name} . " " .$str,
    };

    print encode_json($data);

    return 0;
}

main();
