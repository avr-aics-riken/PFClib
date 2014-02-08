#!/bin/sh -x
#
#PJM --rsc-list "rscgrp=group1"
#PJM -L "rscunit=unit1"

#PJM --rsc-list "node=1"
#PJM --mpi "proc=8"

#PJM --no-stging
#PJM --rsc-list "elapse=00:10:00"
#PJM -s

#. /work/system/Env_base

LD=./compress.exe

#export XOS_MMM_L_ARENA_FREE=2

TIMEX="/usr/bin/time -p"

${TIMEX} mpiexec ${MPIFLAGS} ${MCA} ${LPG} ${LD}
