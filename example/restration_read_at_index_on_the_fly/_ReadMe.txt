**************************************************
  展開処理  インデックス指定  On The Fly example
**************************************************

[ 実行方法 ]

  ./pfcRestoreOTF

---------------------------------------

1.入力データ（POD圧縮結果）
  ./IN_Compressディレクトリ配下を参照して下さい

  ・ボクセルサイズと領域分割
      GlobalVoxel         = (8, 8, 8)
      GlobalDivision      = (2, 2, 1)
  ・タイムステップ
      0step～30step (4slice)
  ・圧縮情報
      CompressInfo {
        CompressFormat     = "pod"
        CompressError      =  1.000
        CalculatedLayer    = 1
        Version            = "1.0.0"
        StartStep          = 0
        EndStep            = 30
      }

2.展開実行条件

  圧縮データをメモリに展開せずに、
  インデックス(i,j,k,step)を指定して値を取得します。
  （読み出す度にファイルを読みに行きます）

----------------------------------------------
入力データ(original)の設定について

  以下の値を設定しています

  for(unsigned step=0; step<numStep; step++)
  {
    for(int k=head[2]; k<=tail[2]; k++ ) {
      for(int j=head[1]; j<=tail[1]; j++ ) {
        for(int i=head[0]; i<=tail[0]; i++ ) {
          for(int nc=0; nc<ncomp; nc++ ) {
            val =  step + k + j + i + nc;
            index =   ncomp*dsize[0]*dsize[1]*(k-head[2])
                    + ncomp*dsize[0]*(j-head[1])
                    + ncomp*(i-head[0])
                    + nc;
            d_v[index] = val;
          }
        }
      }
    }
  }

  ※上記の値が設定されていることを前提に
　　main.cppのダンプ出力が行われていますので
　　上記設定値でない場合、ダンプ出力のorig値は無効となります。

　　　
