# Copyright 2009, UCAR/Unidata
# See the COPYRIGHT file for more information.

# Use automake or CMake for building under nix
# Use CMake for building under windows

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I conf

AM_LDFLAGS = 
AM_CFLAGS = -Wall -g -O0

lib_LTLIBRARIES = libcpoco.la
libcpoco_la_LDFLAGS = -no-undefined
libcpoco_la_CPPFLAGS = -DDLL_EXPORT
include_HEADERS = cpoco.h
libcpoco_la_SOURCES = cpoco.c
libcpoco_la_SOURCES += cp_unix.c
# Never used for *nix* see CMakeLists.txt
#libcpoco_la_SOURCES += cp_win32.c

MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = Makefile.in ./configure aclocal.m4 conf/* config.h.in

EXTRA_DIST = cmake_uninstall.cmake.in CMakeInstallation.cmake CMakeLists.txt README.md COPYRIGHT

##################################################
# Testing

check_LTLIBRARIES = libcpt.la
libcpt_la_SOURCES = cptestlib.c
libcpt_la_LDFLAGS = -no-undefined
# Normally check libraries are created only as archives,
# but we need a shared lib. This appears to do the trick
#(note /tmp is an arbitrary absolute path).
libcpt_la_LDFLAGS += = -module -avoid-version -shared -export-dynamic -no-undefined -rpath ${abs_topbuilddir}

LDADD = libcpoco.la
check_PROGRAMS = cp_test
TESTS = cp_test

