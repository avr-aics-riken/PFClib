**************************************************
  Performance Monitor 使用方法
**************************************************
  当ディレクトリ配下はsampleソースと_Readme.txtのみ置いています。

1. PFClib はPerformance Monitor用にビルドしたものを使用すること
     Performance Monitor libraryを一般的なユーザが使うことは
     想定していないので configure/make では環境を構築出来ません。
      ・src/Makefile_handを使用して -DUSE_PMLIB を指定して
        ビルドして下さい。
      ・configure/makeで環境を構築したインストール先
        (--prefixでで指定したディレクトリ/include)には
        PfcPerfMon.hはコピーされないことに注意
        インストール元のinclude/PfcPerfMon.hを使用する必要があります。

2. ソースへの組み込み

   初期化／終了化はユーザプログラム側に組み込む必要があります。

(1) ヘッダのinclude

     #include "PfcPerfMon.h"

(2) 初期化
　　　圧縮の場合
　　　　PfcPerfMon::InitCompress( numRank, rankID, n_thread );
　　　展開の場合
　　　　PfcPerfMon::InitRestore( numRank, rankID, n_thread );

(3) 終了

      PfcPerfMon::Term();

      ここで、ファイルへの出力と終了化処理が行われます

(4) 備考
      測定区間の開始・終了は以下で指示しますが、
      PFClibの内部のみで指定可能であり、外部から呼びだすことは出来ません。

      PfcPerfMon::Start( t_key );
      PfcPerfMon::Stop ( t_key );

