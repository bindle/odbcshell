#
#   ODBC Shell
#   Copyright (C) 2011 Bindle Binaries <syzdek@bindlebinaries.com>.
#
#   @BINDLE_BINARIES_BSD_LICENSE_START@
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of Bindle Binaries nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BINDLE BINARIES BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
#
#   @BINDLE_BINARIES_BSD_LICENSE_END@
#
#   configure.in - automate compiling on a unix platform
#

AC_PREREQ([2.65])

AC_COPYRIGHT([Copyright (c) 2011 Bindle Binaries <syzdek@bindlebinaries.com>.])
AC_REVISION(0.1)
AC_INIT([ODBC Shell],[0.2],[syzdek@bindlebinaries.com],[odbcshell])

AC_SUBST([RELEASE_MONTH],["June 2011"])

# Version Info:
#    Current    -- the current version number of this API
#    Revision   -- the revision of the implementation of the API version
#    Age        -- How many seqential past API versions is supported by
#                  this implementation
# Format => Current:Revision:Age
AC_SUBST([LIB_VERSION_CURRENT],  [0])
AC_SUBST([LIB_VERSION_REVISION], [0])
AC_SUBST([LIB_VERSION_AGE],      [0])
AC_SUBST([LIB_VERSION_INFO],     [${LIB_VERSION_CURRENT}:${LIB_VERSION_REVISION}:${LIB_VERSION_AGE}])
AC_SUBST([LIB_RELEASE_INFO],     [${LIB_VERSION_CURRENT}]) # used by Win32 DLLs

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([build-aux/autogen.sh])
#AC_CONFIG_LIBOBJ_DIR(compat)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# determines host type
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# configures for automake
AM_INIT_AUTOMAKE(gnu std-options subdir-objects)

# Compilers
AC_PROG_CC
#AC_PROG_OBJC
#AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_USE_SYSTEM_EXTENSIONS # replaces AC_AIX AC_GNU_SOURCE AC_MINIX
AC_C_BIGENDIAN

# binary locations
AC_PROG_INSTALL
AC_PATH_PROG([SHELL],   sh bash,        [AC_MSG_ERROR([missing "sh"])], [$PATH])
AC_CHECK_TOOLS(AR,      ar gar,         [AC_MSG_ERROR([missing binutil "ar"])])
AC_CHECK_TOOLS(LD,      ld gld,         [AC_MSG_ERROR([missing binutil "ld"])])
AC_CHECK_TOOLS(NM,      nm gnm,         [AC_MSG_ERROR([missing binutil "nm"])])
AC_CHECK_TOOLS(RANLIB,  ranlib granlib, [AC_MSG_ERROR([missing binutil "ranlib"])])
AC_CHECK_TOOLS(STRIP,   strip gstrip,   [AC_MSG_ERROR([missing binutil "strip"])])

# shell programs
AC_PROG_AWK
AC_PROG_EGREP
AC_PROG_FGREP
AM_PROG_LEX
AC_PROG_LN_S

# GNU Libtool Support
LT_INIT(dlopen disable-fast-install win32-dll)

# custom configure options
AC_ODBCSHELL_ENABLE_WARNINGS

# Geek Humor check
AC_CHECK_LIB([aundry],[clean socks],,[AC_MSG_NOTICE([No clean socks found.])])

# enables getopt_long if header and functions were found
AC_CHECK_HEADERS([getopt.h], [AC_DEFINE_UNQUOTED(USE_GETOPT_LONG, 1, [Use GNU getopt_long])])

# checks for environement variable functions
AC_SEARCH_LIBS([getenv],   ,,AC_MSG_ERROR([ODBC Shell requires a C library with getenv().]))
AC_SEARCH_LIBS([setenv],   ,,AC_MSG_ERROR([ODBC Shell requires a C library with setenv().]))
AC_SEARCH_LIBS([unsetenv], ,,AC_MSG_ERROR([ODBC Shell requires a C library with unsetenv().]))

# check for iODBC
have_iodbc=yes
AC_CHECK_HEADERS([sql.h]                                   ,,[have_iodbc=no])
AC_CHECK_HEADERS([sqlext.h]                                ,,[have_iodbc=no])
AC_CHECK_HEADERS([sqltypes.h]                              ,,[have_iodbc=no])
AC_CHECK_HEADERS([sqlucode.h]                              ,,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_HEADERS([iodbcext.h]                              ,,)
AC_CHECK_TYPES([SQLCHAR],              ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLHDBC],              ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLHENV],              ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLHSTMT],             ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLHWND],              ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLINTEGER],           ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLRETURN],            ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLSMALLINT],          ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLTCHAR],             ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLUINTEGER],          ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLULEN],              ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_CHECK_TYPES([SQLUSMALLINT],         ,[have_iodbc=no],[[#include <sqlext.h>]])
AC_SEARCH_LIBS([SQLAllocHandle],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLCloseCursor],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLDataSources],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLDisconnect],           [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLDescribeCol],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLDriverConnectW],       [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLExecute ],             [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLFreeHandle],           [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLFetchScroll],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLGetData],              [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLGetDiagRec],           [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLGetFunctions],         [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLGetInfo],              [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLGetTypeInfo],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLMoreResults],          [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLNumResultCols],        [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLPrepare],              [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLRowCount],             [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLSetConnectOption],     [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLSetEnvAttr],           [iodbc odbc],,[have_iodbc=no])
AC_SEARCH_LIBS([SQLTables ],              [iodbc odbc],,[have_iodbc=no])
if test "x${have_iodbc}" == "xno";then
   AC_MSG_ERROR([ODBC Shell requires iODBC or unixODBC.])
fi

# check for GNU readline
have_readline=yes
AC_CHECK_LIB([curses], [tgetnum])
AC_CHECK_HEADERS([readline/readline.h]      ,,[have_readline=no])
AC_CHECK_HEADERS([readline/history.h]       ,,[have_readline=no])
AC_SEARCH_LIBS([add_history],     [readline],,[have_readline=no], [-lcurses])
AC_SEARCH_LIBS([read_history],    [readline],,[have_readline=no], [-lcurses])
AC_SEARCH_LIBS([write_history],   [readline],,[have_readline=no], [-lcurses])
AC_SEARCH_LIBS([readline],        [readline],,[have_readline=no], [-lcurses])
if test "x${have_readline}" == "xno";then
   AC_MSG_ERROR([ODBC Shell requires GNU Readline library.])
fi

# determine PACKAGE_VERSION via Git
AC_DMS_GIT_PACKAGE_VERSION

# Creates outputs
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

# Show local config
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   ODBC Shell])
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   Please send suggestions to:   $PACKAGE_BUGREPORT])
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   run 'make all'])
AC_MSG_NOTICE([ ])

# end of configure.ac
