# Copyright 2009, UCAR/Unidata
# See the COPYRIGHT file for more information.

# Use automake or CMake for building under nix
# Use CMake for building under windows

############
# Set Project Properties
#############

cmake_minimum_required(VERSION 2.8.11)

#######
# Project Name: cpoco
#######
PROJECT(cpoco C)
SET(PACKAGE "cpoco" CACHE STRING "")

#####
# Version Info:
#
# Release Version
# Library Version
# SO Version
#
# SO Version is computed from library version. See:
# http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
#####

SET(VERSION 1.0.0)
SET(PACKAGE_VERSION ${VERSION})

#######
##
# Get system configuration, Use it to determine osname, os release, cpu. These
# will be used when committing to CDash.
##
find_program(UNAME NAMES uname)
IF(UNAME)
  macro(getuname name flag)
    exec_program("${UNAME}" ARGS "${flag}" OUTPUT_VARIABLE "${name}")
  endmacro(getuname)
  getuname(osname -s)
  getuname(osrel  -r)
  getuname(cpu    -m)
  set(BUILDNAME "${osname}-${osrel}-${cpu}" CACHE STRING "Build name variable for CDash")
ENDIF()

##
# For CMAKE_INSTALL_LIBDIR
##
INCLUDE(GNUInstallDirs)

SET(CMAKE_INSTALL_CMAKEDIR cmake)

IF(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  SET(CMAKE_INSTALL_LIBDIR lib)
ENDIF(NOT DEFINED CMAKE_INSTALL_LIBDIR)

IF(MSVC)
  SET(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF()

##
# Add custom CMake Module
##
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/"
  CACHE INTERNAL "Location of our custom CMake modules.")

##
# auto-configure style checks, other CMake modules.
##
# Are these actually used?
INCLUDE(${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckCXXSourceCompiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckCSourceCompiles.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/GetPrerequisites.cmake)

INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCXXCompilerFlag)
FIND_PACKAGE(PkgConfig QUIET)

# Set the build type.
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Choose the type of build, options are: None, Debug, Release."
    FORCE)
ENDIF()

# Set build type uppercase
STRING(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

# Determine the configure date.

EXECUTE_PROCESS(
  COMMAND date
  OUTPUT_VARIABLE CONFIG_DATE
  )
IF(CONFIG_DATE)
	string(STRIP ${CONFIG_DATE} CONFIG_DATE)
ENDIF()

#########
# End setting general properties.
#########

#########
# Compiler Options
#########

####
# Default building shared libraries.
# BUILD_SHARED_LIBS is provided by/used by
# CMake directly.
#####
OPTION(BUILD_SHARED_LIBS "Build cpoco libraries as a shared library." ON)
IF(BUILD_SHARED_LIBS)
  SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
ENDIF()

#####
# Set some default linux gcc & apple compiler options for
# debug builds.
#####
IF(CMAKE_CXX_COMPILER_IS_GNUCC OR APPLE)
  OPTION(ENABLE_COVERAGE_TESTS "Enable compiler flags needed to perform coverage tests." OFF)
  OPTION(ENABLE_CONVERSION_WARNINGS "Enable warnings for implicit conversion from 64 to 32-bit datatypes." ON)
  OPTION(ENABLE_LARGE_FILE_TESTS "Enable large file tests." OFF)

#  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

  # Debugging flags
#  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")

  # Check to see if -Wl,--no-undefined is supported.
  CHECK_CXX_LINKER_FLAG("-Wl,--no-undefined" LIBTOOL_HAS_NO_UNDEFINED)

  IF(LIBTOOL_HAS_NO_UNDEFINED)
    SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -Wl,--no-undefined")
  ENDIF()
  SET(CMAKE_REQUIRED_FLAGS "${TMP_CMAKE_REQUIRED_FLAGS}")

  # Coverage tests need to have optimization turned off.
  IF(ENABLE_COVERAGE_TESTS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    MESSAGE(STATUS "Coverage Tests: On.")
  ENDIF()

    # Warnings for 64-to-32 bit conversions.
  IF(ENABLE_CONVERSION_WARNINGS)
    CHECK_CXX_COMPILER_FLAG(-Wconversion CXX_HAS_WCONVERSION)
    CHECK_CXX_COMPILER_FLAG(-Wshorten-64-to-32 CXX_HAS_SHORTEN_64_32)

    IF(CXX_HAS_SHORTEN_64_32)
      SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wshorten-64-to-32")
    ENDIF()
    IF(CXX_HAS_WCONVERSION)
      SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wconversion")
    ENDIF()

  ENDIF(ENABLE_CONVERSION_WARNINGS)

ENDIF(CMAKE_CXX_COMPILER_IS_GNUCC OR APPLE)

ADD_DEFINITIONS()

#####
# Supress CRT Warnings.
# Only necessary for Windows
#####
IF(MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

#####
# Set the appropriate compiler/architecture for universal OSX binaries.
#####
IF(${CMAKE_SYSTEM_NAME} EQUAL "Darwin")
  SET(CMAKE_OSX_ARCHITECTURES i386;x86_64)
ENDIF(${CMAKE_SYSTEM_NAME} EQUAL "Darwin")

##########
# End compiler options
##########

##########
# End compiler options
##########

##########
# Environment checks
##########

# Include checks (probably overkill)

CHECK_INCLUDE_FILE("unistd.h"  HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("malloc.h"    HAVE_MALLOC_H)
CHECK_INCLUDE_FILE("dlfcn.h"   HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("errno.h"   HAVE_ERRNO_H)
CHECK_INCLUDE_FILE("locale.h"  HAVE_LOCAL_H)
CHECK_INCLUDE_FILE("stdint.h"  HAVE_STDINT_H)
CHECK_INCLUDE_FILE("stdio.h"   HAVE_STDIO_H)
CHECK_INCLUDE_FILE("stdlib.h"  HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("stdarg.h"    HAVE_STDARG_H)
CHECK_INCLUDE_FILE("strings.h"   HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h"   HAVE_STRING_H)
CHECK_INCLUDE_FILE("inttypes.h"  HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("stddef.h"   HAVE_STDDEF_H)

FIND_PACKAGE(Threads)
IF(CMAKE_USE_PTHREADS_INIT)
  SET(HAVE_PTHREAD ON)
ELSE()
  SET(HAVE_PTHREAD ON)
ENDIF()

##########
# End environment checks
##########

OPTION(ENABLE_MUTEX "Enable mutual exclusion." ON)
OPTION(ENABLE_PTHREAD "Enable libpthread." ON)

IF(NOT HAVE_PTHREAD AND ENABLE_PTHREAD)
    SET(ENABLE_PTHREAD OFF)
    MESSAGE(WARNING "ENABLE_PTHREAD specified, libpthread not found; disabling pthread")
ENDIF()

IF(NOT ENABLE_MUTEX)
    SET(ENABLE_PTHREAD OFF)
    MESSAGE(WARNING "ENABLE_MUTEX=off => disabling pthread")
ENDIF()

##########
# End user options
##########

##########
# Compilation sources
##########

SET(cpoco_SOURCES cpoco.c)
IF(MSVC)
SET(cpoco_SOURCES ${cpoco_SOURCES} cp_win32.c)
ENDIF()
IF(UNIX)
SET(cpoco_SOURCES ${cpoco_SOURCES} cp_unix.c)
ENDIF()
MESSAGE(STATUS "SOURCES=${cpoco_SOURCES}")

ADD_LIBRARY(libcpoco STATIC ${cpoco_SOURCES})
ADD_LIBRARY(cpoco SHARED ${cpoco_SOURCES})

INSTALL(FILES cpoco.h
	      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	      COMPONENT headers)

INSTALL(
  TARGETS cpoco
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT libraries
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT libraries
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT libraries
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

##########
# Installation stuff
##########
##
# Brute force, grab all of the dlls from the dependency directory,
# install them in the binary dir. Grab all of the .libs, put them
# in the libdir.
##

###
# Subdirectory CMakeLists.txt files should specify their own
# 'install' files.
# Including 'CPack' kicks everything off.
###
INCLUDE(InstallRequiredSystemLibraries)

###
# Create pkgconfig files.
###
# Set
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix ${CMAKE_INSTALL_PREFIX})
SET(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
SET(includedir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
SET(cmakedir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_CMAKEDIR})
SET(CC ${CMAKE_C_COMPILER})

# Create config.h file.
configure_file("${cpoco_SOURCE_DIR}/config.h.cmake.in"
               "${cpoco_BINARY_DIR}/config.h")
ADD_DEFINITIONS(-DHAVE_CONFIG_H)
INCLUDE_DIRECTORIES(${cpoco_BINARY_DIR})

################################
# Testing
################################
SET(TESTDLL cygcpt.dll)

ENABLE_TESTING()
ADD_TEST(NAME cp_test COMMAND cp_test ${TESTDLL})
ADD_EXECUTABLE(cp_test cp_test.c)
TARGET_LINK_LIBRARIES(cp_test cpoco) 

SET(cpt_SOURCES cptestlib.c)
ADD_LIBRARY(cpt MODULE ${cpt_SOURCES})

################################
# CPack inclusion must come last.
#INCLUDE(CPack)
INCLUDE(CMakeInstallation.cmake)
