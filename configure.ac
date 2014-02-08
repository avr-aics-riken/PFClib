###
#########################################################################
#
# PFClib - Parallel File Compression library
#
# Copyright (c) 2013-2014 Advanced Institute for Computational Science, RIKEN.
# All rights reserved.
#
#########################################################################
###
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([PFClib],[0.7.0],[keno@riken.jp],[PFClib])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src/PfcCioDfiUtil.cpp])
AM_CONFIG_HEADER([config.h])


#
# Revision No.
#
PFC_REVISION="20140208_1700"
AC_SUBST(PFC_REVISION)


#
# Compression version
#
PFC_CMP_VERSION="1.0.0"
AC_SUBST(PFC_CMP_VERSION)


#
# default prefix setting
#
AC_PREFIX_DEFAULT([/usr/local/FFV/PFClib])
if test "$prefix" = "NONE" ; then
  PFC_INST_DIR=/usr/local/FFV/PFClib
else
  PFC_INST_DIR="$prefix"
fi


#
# MPI environment
#
enable_mpi="none"


PFC_CXX="$CXX"
AC_SUBST(PFC_CXX)

case "$PFC_CXX" in
  mpicxx)   enable_mpi="wrappermpi" ;;
  mpicc)    enable_mpi="wrappermpi" ;;
  mpiCC)    enable_mpi="wrappermpi" ;;
  mpic++)   enable_mpi="wrappermpi" ;;
  mpiFCCpx) enable_mpi="wrappermpi" ;;
esac


AC_SUBST(MPI_DIR)
AC_SUBST(MPI_CFLAGS)
AC_SUBST(MPI_LDFLAGS)
AC_SUBST(MPI_LIBS)


# 
# MPI library
#
AC_ARG_WITH(MPI, [AC_HELP_STRING([--without-MPI],[do not use MPI library])], , with_MPI=yes)
if test "$with_MPI" = "no" ; then
  CXXFLAGS=-D_PFC_WITHOUT_MPI_
else

  # openmpi
  AC_ARG_WITH( ompi, [AC_HELP_STRING([--with-ompi=dir],[Specify OpenMPI install directory])], , with_ompi=none)
  if test "$with_ompi" != "none" ; then
    MPI_DIR=$with_ompi;
    MPI_CFLAGS=-I$MPI_DIR/include
    MPI_LDFLAGS=-L$MPI_DIR/lib
    MPI_LIBS=-lmpi
    enable_mpi="ompi"
  fi
fi


#
# Warning
#

if test "$with_MPI" = "yes" ; then
  if test "$enable_mpi" != "wrappermpi" ; then
    if test "$with_ompi" = "none" ; then
      echo "  Warning: OpenMPI library is not specified."
      echo "           If compilation error occurs about MPI, specify --with-ompi option."
    fi
  fi
fi



#
# example test
#
AC_ARG_WITH(example, [AC_HELP_STRING([--with-example=(yes|no)],[make example [yes]])], , with_example=yes)



#
# CPMlib
#
AC_ARG_WITH(cpm, [AC_HELP_STRING([--with-cpm=dir],[Specify CPMlib installed directory])], , with_cpm=none)
AC_SUBST(CPM_DIR)
AC_SUBST(CPM_CFLAGS)
AC_SUBST(CPM_LDFLAGS)
if test "$with_cpm" != "none" ; then
  CPM_DIR=$with_cpm;
  CPM_CFLAGS=`$CPM_DIR/bin/cpm-config --cflags`
  CPM_LDFLAGS=`$CPM_DIR/bin/cpm-config --libs`
  if test "$enable_mpi" = "none" ; then
    echo "  Error: MPI library is not specified."
    echo "         --with-cpm option used MPI."
    exit 1
  fi
fi


#
# TextParser environment
#
AC_ARG_WITH(parser, [AC_HELP_STRING([--with-parser=dir],[Specify TextParser install directory])], , with_parser=none)
AC_SUBST(TP_DIR)
AC_SUBST(TP_CFLAGS)
AC_SUBST(TP_LDFLAGS)

if test "$with_parser" != "none" ; then
  TP_DIR=$with_parser;
  TP_CFLAGS=`$TP_DIR/bin/tp-config --cflags`
  TP_LDFLAGS=`$TP_DIR/bin/tp-config --libs`
else
  echo "  Error: TextParser library is not specified."
  echo "         Specify --with-parser option."
  exit 1
fi


#
# CIOlib
#
AC_ARG_WITH(cio, [AC_HELP_STRING([--with-cio=dir],[Specify CIOlib installed directory])], , with_cio=none)
AC_SUBST(CIO_DIR)
AC_SUBST(CIO_CFLAGS)
AC_SUBST(CIO_LDFLAGS)
if test "$with_cio" != "none" ; then
  CIO_DIR=$with_cio;
  CIO_CFLAGS=`$CIO_DIR/bin/cio-config --cflags`
  CIO_LDFLAGS=`$CIO_DIR/bin/cio-config --libs`
  if test "$enable_mpi" = "none" ; then
    echo "  Error: MPI library is not specified."
    echo "         --with-cpm option used MPI."
    exit 1
  fi
fi


#
# PFC special flags
#
PFC_CFLAGS="-I$PFC_INST_DIR/include"
AC_SUBST(PFC_CFLAGS)

PFC_LDFLAGS="-L$PFC_INST_DIR/lib"
AC_SUBST(PFC_LDFLAGS)

PFC_LIBS="-lPFC -mkl"
AC_SUBST(PFC_LIBS)



#
# MAKE_SUB_DIRS
#
AC_SUBST(MAKE_SUB_DIRS)
MAKE_SUB_DIRS="src doc utility/staging/src"

# example
if test "$with_example" = "yes" ; then
  MAKE_SUB_DIRS="$MAKE_SUB_DIRS"" example/compress example/compress_cmd example/restration_read_at_index"
  MAKE_SUB_DIRS="$MAKE_SUB_DIRS"" example/restration_read_at_index_on_the_fly example/restration_read_in_range"
fi




#
# Checks for programs.
#
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB



#
# Checks for header files.
#
AC_CHECK_HEADERS([float.h stdlib.h string.h unistd.h])


#
# Checks for typedefs, structures, and compiler characteristics.
#
AC_C_INLINE
AC_TYPE_SIZE_T


#
# Checks for library functions.
#
AC_CHECK_FUNCS([memset mkdir pow sqrt strcasecmp strdup strerror])



AC_CONFIG_FILES([Makefile \
                 doc/Makefile \
                 src/Makefile \
                 pfc-config \
                 include/pfcVersion.h \
                 utility/staging/src/Makefile])

# example
if test "$with_example" = "yes" ; then
  AC_CONFIG_FILES([ \
      example/compress/Makefile \
      example/compress_cmd/Makefile \
      example/restration_read_at_index/Makefile \
      example/restration_read_at_index_on_the_fly/Makefile \
      example/restration_read_in_range/Makefile \
  ])
fi



AC_OUTPUT

chmod +x ./pfc-config

