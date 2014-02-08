#!/bin/sh

# riken kobe K computer
#. /home/system/Env_base  # login node

SYSTEM="Kei"

make -f Makefile_fjqs System=${SYSTEM} clean
echo "#### make start: >& CompileList.txt  #####"
make -f Makefile_fjqs System=${SYSTEM} 2>&1 | tee CompileList.txt
echo "#### make end: >& CompileList.txt   #####"
