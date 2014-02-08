#!/bin/sh -x
#

LD=../../utility/staging/src/pfc_staging.exe

#-----------------------------------------------------
# (usage)
#   -i  領域分割情報が記述されたファイル
#   -o  振り分け先のディレクトリ
#   -c  圧縮制御データファイル（圧縮コマンド用）
#         指定なき場合、制御データファイルのcopyなし
#   -s  開始ステップ 
#         指定なき場合、DFIファイルに記述されている先頭ステップとする
#   -e  終了ステップ 
#         指定なき場合、DFIファイルに記述されている最終ステップとする
#
#   *.dfi 振り分け対象とするDFIファイル名　（複数指定可）  
#
#
#-----------------------------------------------------


#${LD} -i proc_1x2x1.txt -o OUT -c pfc_cntl -s 10 -e 70 IN_Cio/vel.dfi 2>&1 | tee Log.txt

${LD} -i proc_1x2x1.txt -o OUT -c pfc_cntl -s 0 -e 70 IN_Cio/vel.dfi 2>&1 | tee Log.txt
