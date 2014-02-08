#########################################################################
#
# PFClib Staging 
# 
# Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
# #######################################################################

bin_PROGRAMS = pfcfrm

pfcfrm_CXXFLAGS = \
  -I$(top_builddir)/include \
  -I$(top_builddir)/utility/staging/include \
  -I@CIO_DIR@/include \
  @CPM_CFLAGS@ \
  @CIO_CFLAGS@ \
  @TP_CFLAGS@ 

pfcfrm_SOURCES = \
  PfcFunction.cpp \
  Staging.cpp \
  ActiveSubDomain.cpp \
  main.cpp

pfcfrm_includedir = $(includedir)
pfcfrm_include_HEADERS = \
  $(top_builddir)/utility/staging/include/PfcFunction.h \
  $(top_builddir)/utility/staging/include/ActiveSubDomain.h \
  $(top_builddir)/utility/staging/include/Staging_Define.h \
  $(top_builddir)/utility/staging/include/Staging_Utility.h \
  $(top_builddir)/utility/staging/include/frm_EndianUtil.h \
  $(top_builddir)/utility/staging/include/frm_Version.h

pfcfrm_LDADD = \
  -L$(top_builddir)/src \
  @CPM_LDFLAGS@ \
  @CIO_LDFLAGS@ \
  @TP_LDFLAGS@

EXTRA_DIST = Makefile_hand
