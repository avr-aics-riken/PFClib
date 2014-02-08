**************************************************
  展開処理  example
**************************************************

1.入力データ(original)
  ./IN_Cioディレクトリ配下を参照して下さい
 
  ・ボクセルサイズと領域分割
      GlobalVoxel         = (8, 8, 8)
      GlobalDivision      = (2, 2, 2)
  ・タイムステップ
      0step～490step (50slice)

2.入力データ（POD圧縮結果）
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

3.展開実行条件
　・領域方向分割数
　　　domainDivision　　　= (1, 1, 1)
  ・展開処理そのものはMPI実行する必要はありませんが
    CIOlibがMPIを要求するため、並列数１でMPI実行しています。

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

　　　
