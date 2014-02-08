#########################################################################
#
# PFClib - Parallel File Compression library
#
# Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
# #######################################################################

noinst_PROGRAMS=pfcCompress

DISTCLEANFILES=*~
CLEANFILES=data/*.log

pfcCompress_SOURCES = main.cpp

pfcCompress_CXXFLAGS = \
    -I$(top_builddir)/include \
    @MPI_CFLAGS@ \
    @CPM_CFLAGS@ \
    @CIO_CFLAGS@ \
    @TP_CFLAGS@

pfcCompress_LDADD = \
    -L$(top_builddir)/src \
    @PFC_LIBS@ \
    @CPM_LDFLAGS@ \
    @CIO_LDFLAGS@ \
    @MPI_LDFLAGS@ \
    @MPI_LIBS@ \
    @TP_LDFLAGS@

dist_noinst_DATA=IN_Cio OUT_Compress_sample _ReadMe.txt go.sh run.sh \
  compile.Kei.sh Makefile_include.Kei Makefile_hand Makefile_include.linux-gnu

.PHONY:$(dist_noinst_DATA)
