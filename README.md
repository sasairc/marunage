marunage
===

Slack Outgoing Webhooksの処理言語非依存なサーバのようなもの。

## Install

```shellsession
% make
# make install
```

## Usage 

```shellsession
% marunage --port [PORT] --call-parser [CALL_PARSER]
```

`marunage`は`-p, --port`で指定されたポートをリッスンし、Slack Outgoing Webhooksからの電文を受信します。		
その際、HTTPヘッダ等を除いたbody部を第一引数として`-c, --call-parser`で指定された任意のプログラムを起動。標準出力に対して割り当てられたパイプ経由でJSON形式のデータを取得し、そのデータをレスポンスとして送信します。

パーサプログラムにて処理をtoken単位で管理すれば、複数のSlack Outgoing Webhooksアプリケーションを一元化できます。

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
