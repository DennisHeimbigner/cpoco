# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.62)
AC_INIT(cpoco, 1.1.0, support@unidata.ucar.org)
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR(conf)

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([conf])
AC_CANONICAL_HOST
AC_SUBST(host)

dnl library versionning: Update these when the interface changes. Generally,
dnl assume that the interface tracks the major and minor release numbers.

# Unfortunately, we can't use AC_SUBST variables in specifying SUBDIRS
# in Makefile.am because autoconf checks for existence before substitution.
# So, these must match Makefile.am
LIBCP_VERSION=1.0.0

AC_SUBST([LIBCP_VERSION])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_INSTALL

##################################################

AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strdup])
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Apparently AX_PTHREAD is not available
AC_CHECK_LIB(pthread, pthread_create)
if test "x$ac_cv_lib_pthread_pthread_create" != x ; then
have_pthread=yes
else
have_pthread=no
fi


# Give the option to not use pthread
AC_MSG_CHECKING([If libpthread should be used])
AC_ARG_ENABLE([pthread],[AS_HELP_STRING([--disable-pthread],
                              [disable using libpthread])])
test "x$enable_pthread" = xno || enable_pthread=yes
AC_MSG_RESULT($enable_pthread)

if test "x$have_pthread" = xno ; then
AC_MSG_NOTICE([libpthread is not available => --disable-pthread])
enable_pthread=no
fi

# Give the option to not use mutual exclusion
AC_MSG_CHECKING([If mutual exclusion should be used])
AC_ARG_ENABLE([mutex],[AS_HELP_STRING([--disable-mutex],
                              [disable using mutual exclusion])])
test "x$enable_mutex" = xno || enable_mutex=yes
AC_MSG_RESULT($enable_mutex)

# Mutex disabled => disable-pthreads
if test "x$enable_mutex" = xno ; then
AC_MSG_NOTICE([--disable-mutex => --disable-pthread])
enable_pthread=no
fi

if test "x$enable_mutex" = xyes ; then
AC_DEFINE([USE_MUTEX], [1], [if true, use mutual exclusion])
fi
AM_CONDITIONAL(USE_MUTEX, [test x$enable_mutex = xyes])
AM_CONDITIONAL(USE_PTHREADS, [test x$enable_pthreads = xyes])

##################################################
# Process the Makefile.am (and other) files
AC_CONFIG_FILES([Makefile
                 ])
AC_OUTPUT

##################################################
