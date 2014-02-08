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

lib_LIBRARIES = libPFC.a

libPFC_a_CXXFLAGS = @MPI_CFLAGS@ @CPM_CFLAGS@ @CIO_CFLAGS@ @TP_CFLAGS@ -I$(top_builddir)/include

libPFC_a_SOURCES = \
    PfcCioDfiUtil.cpp \
    PfcCoeffTree.cpp \
    PfcCompress.cpp \
    PfcCompressCioPod.cpp \
    PfcCompressCmd.cpp \
    PfcCompressCmdElm.cpp \
    PfcCompressInfo.cpp \
    PfcCompressPod.cpp \
    PfcCompressPod_CalPod.cpp \
    PfcCompressPod_CheckFinish.cpp \
    PfcCompressPod_Output.cpp \
    PfcCompressPod_SwapBinaryData.cpp \
    PfcCompress_WriteIndexPfcFile.cpp \
    PfcCompress_WriteProcFile.cpp \
    PfcDivision.cpp \
    PfcDomain.cpp \
    PfcFileInfo.cpp \
    PfcFilePath.cpp \
    PfcFunction.cpp \
    PfcMatrix.cpp \
    PfcMpiCom.cpp \
    PfcPathUtil.cpp \
    PfcPodFile.cpp \
    PfcRegion.cpp \
    PfcRestration.cpp \
    PfcRestrationRegion.cpp \
    PfcRestrationRegionPod.cpp \
    PfcRestrationRegionPod_Expand.cpp \
    PfcTextParser.cpp \
    PfcTimeSlice.cpp \
    PfcUnitList.cpp

PFC_includedir = $(includedir)
PFC_include_HEADERS = \
    $(top_builddir)/include/PfcCioDfiUtil.h \
    $(top_builddir)/include/PfcCoeffTree.h \
    $(top_builddir)/include/PfcCompress.h \
    $(top_builddir)/include/PfcCompressCioPod.h \
    $(top_builddir)/include/PfcCompressCmd.h \
    $(top_builddir)/include/PfcCompressCmdElm.h \
    $(top_builddir)/include/PfcCompressInfo.h \
    $(top_builddir)/include/PfcCompressPod.h \
    $(top_builddir)/include/PfcDefine.h \
    $(top_builddir)/include/PfcDivision.h \
    $(top_builddir)/include/PfcDomain.h \
    $(top_builddir)/include/PfcEndianChange.h \
    $(top_builddir)/include/PfcFileInfo.h \
    $(top_builddir)/include/PfcFilePath.h \
    $(top_builddir)/include/PfcFunction.h \
    $(top_builddir)/include/PfcMatrix.h \
    $(top_builddir)/include/PfcMpiCom.h \
    $(top_builddir)/include/PfcPathUtil.h \
    $(top_builddir)/include/PfcPodFile.h \
    $(top_builddir)/include/PfcRegion.h \
    $(top_builddir)/include/PfcRestration.h \
    $(top_builddir)/include/PfcRestrationRegion.h \
    $(top_builddir)/include/PfcRestrationRegionPod.h \
    $(top_builddir)/include/PfcTextParser.h \
    $(top_builddir)/include/PfcTimeSlice.h \
    $(top_builddir)/include/PfcUnitList.h \
    $(top_builddir)/include/mpi_stubs.h
