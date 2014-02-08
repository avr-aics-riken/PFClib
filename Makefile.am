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

SUBDIRS = @MAKE_SUB_DIRS@

bin_SCRIPTS = pfc-config
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = pfc-config.in LICENSE include/mpi_stubs.h example/staging
dist_data_DATA = AUTHORS COPYING ChangeLog README LICENSE
dist_include_HEADERS = include/mpi_stubs.h

clean-local:
	find ./ -name *.obj | xargs rm -f
	find ./ -name *.exe | xargs rm -f
	find ./ -name *.lib | xargs rm -f

distclean-local: clean-local
	rm -f ./bin/*
	rm -f ./lib/*

