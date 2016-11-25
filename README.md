marunage
===

[![version](http://img.shields.io/github/tag/sasairc/marunage.svg?style=flat&label=version)](https://github.com/sasairc/marunage/releases)
[![license](https://img.shields.io/badge/License-WTFPL2-blue.svg?style=flat)](http://www.wtfpl.net/txt/copying/)
[![build](https://img.shields.io/travis/sasairc/marunage.svg?style=flat)](https://travis-ci.org/sasairc/marunage)

Slack Outgoing Webhooksの処理言語非依存なサーバのようなもの。

## Install

```shellsession
% make
# make install
```

## Usage 

```shellsession
% marunage --port=[PORT] --call-parser=[CALL_PARSER] --logfile=[PATH]
```

`marunage`は`-p, --port`で指定されたポートをリッスンし、Slack Outgoing Webhooksからの電文を受信します。		
その際、HTTPヘッダ等を除いたbody部を第一引数として`-c, --call-parser`で指定された任意のプログラムを起動。標準出力に対して割り当てられたパイプ経由でJSON形式のデータを取得し、そのデータをレスポンスとして送信します。

パーサプログラムにて処理をtoken、またはtrigger_word単位で管理すれば、複数のSlack Outgoing Webhooksアプリケーションを一元化できます。

### Options

長いオプションに必須の引数は短いオプションにも必須です。

* `-p, --port=[PORT]`	
使用するポートを指定します。既定では8880番です。		
Apacheなどのサーバをフロントエンドとして使用している場合、そのリバースプロキシの転送先へ合わせて下さい。

* `-c, --call-parser=[PATH]`	
`POST`された際に呼び出すパーサプログラムを指定します。このオプションは必須です。

* `-f, --logfile=[PATH]`	
アクセスログを記録する任意のファイルを指定します。		
とくに指定されない場合は、`/var/log/marunage/marunage.log`が使用されます。

* `-l, --with-log`	
アクセスログを記録するモードです。既定で有効。

* `-n, --without-log`	
アクセスログを記録しないモードです。

## Parser Example

最も単純で危険な「perlでエコーを返す」サンプルです。		
自分(bot)の発言内容にもひっかかるため実行は自己責任で・・・

```perl
#!/usr/bin/perl
use strict;
use warnings;
use utf8;

use JSON;

my @query = split(/&/, $ARGV[0]);
my ($str, $key, $val, %req);

foreach (@query) {
    ($key, $val) = split(/=/, $_);
    $req{$key} = $val;
}

my $data = {
    token   => $req{token},
    text    => $req{text},
};

print encode_json($data);

return 0;
```

## License

[WTFPL version 2](http://www.wtfpl.net/txt/copying/)

## Author

sasairc (http://github.com/sasairc)
