**************************************************
  Performance Monitor 使用方法
**************************************************
  当ディレクトリ配下はsampleソースと_Readme.txtのみ置いています。

1. PFClib　はPerformance Monitor用にビルドしたものを使用すること
     （コンパイルオプション -DUSE_PMLIB を指定してビルド）

2. ソースへの組み込み

   初期化／終了化はユーザプログラム側に組み込む必要があります。

(1) ヘッダのinclude

     #include "PfcPerfMon.h"

(2) 初期化
　　　圧縮の場合
　　　　PfcPerfMon::InitCompress( rankID );
　　　展開の場合
　　　　PfcPerfMon::InitRestore( rankID );

(3) 測定区間の開始・終了

      PfcPerfMon::Start( t_key );
      PfcPerfMon::Stop ( t_key );

      ユーザプログラム側で設定可能なkeyは以下です。
　　　　圧縮： t1_c　～　t5_c
　　　　展開： t1_r　～　t5_r

(4) 終了

      PfcPerfMon::Term();

      ここで、ファイルへの出力と終了化処理が行われます
      

