#########################################################################
#
# PFClib - Parallel File Compression library
#
# Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
# #######################################################################

noinst_PROGRAMS=pfcCompressCmd

DISTCLEANFILES=*~
CLEANFILES=data/*.log

pfcCompressCmd_SOURCES = main.cpp

pfcCompressCmd_CXXFLAGS = \
    -I$(top_builddir)/include \
    @MPI_CFLAGS@ \
    @CIO_CFLAGS@ \
    @TP_CFLAGS@

pfcCompressCmd_LDADD = \
    -L$(top_builddir)/src \
    @PFC_LIBS@ \
    @CIO_LDFLAGS@ \
    @MPI_LDFLAGS@ \
    @MPI_LIBS@ \
    @TP_LDFLAGS@

dist_noinst_DATA=IN_Cio OUT_Compress_sample _ReadMe.txt go.sh run.sh pfc_cntl Makefile_hand

.PHONY:$(dist_noinst_DATA)
