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
#   @configure_input@
#
#   Makefile.am - automate compiling on a unix platform
#

# Sub directories
SUBDIRS					=
DIST_SUBDIRS				=

# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 @AM_CXXFLAGS@
AM_OBJCFLAGS				= -O2 @AM_OBJCFLAGS@
AM_CPPFLAGS				= -O2 -UPMARK -DHAVE_CONFIG_H=1 -I$(top_builddir)/include -I$(top_srcdir)/include -DSYSCONFDIR=\"$(sysconfdir)\"
ACLOCAL_AMFLAGS				= -I m4 -W all -W error
AM_MAKEINFOFLAGS                        = --no-split
DEFS					=


# automake targets
check_PROGRAMS				=
doc_DATA				=
include_HEADERS				=
lib_LTLIBRARIES				=
man_MANS				=
info_TEXINFOS				= doc/odbcshell.texi
noinst_LTLIBRARIES			=
noinst_HEADERS				=
noinst_PROGRAMS				= src/odbc-example
bin_PROGRAMS				= src/odbcshell


# macros for doc/readme.info
doc_odbcshell_TEXINFOS			= doc/copyright.texi \
                                          doc/odbcshell.texi


# macros for src/odbc-example
src_odbc_example_DEPENDENCIES		= Makefile
src_odbc_example_SOURCES		= src/odbc-example.c


# macros for src/odbcshell
src_odbcshell_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_odbcshell_CPPFLAGS			= -DPROGRAM_NAME="\"odbcshell\"" $(AM_CPPFLAGS)
src_odbcshell_SOURCES			= $(noinst_HEADERS) \
					  src/odbcshell.c \
					  src/odbcshell.h \
					  src/odbcshell-cli.c \
					  src/odbcshell-cli.h \
					  src/odbcshell-commands.c \
					  src/odbcshell-commands.h \
					  src/odbcshell-exec.c \
					  src/odbcshell-exec.h \
					  src/odbcshell-odbc.c \
					  src/odbcshell-odbc.h \
					  src/odbcshell-options.c \
					  src/odbcshell-options.h \
					  src/odbcshell-parse.c \
					  src/odbcshell-parse.h \
					  src/odbcshell-print.c \
					  src/odbcshell-print.h \
					  src/odbcshell-profile.c \
					  src/odbcshell-profile.h \
					  src/odbcshell-script.c \
					  src/odbcshell-script.h \
					  src/odbcshell-signal.c \
					  src/odbcshell-signal.h \
					  src/odbcshell-variables.c \
					  src/odbcshell-variables.h


# substitution routine
do_subst = sed \
	-e 's,[@]SHELL[@],@SHELL@,g' \
	-e 's,[@]PACKAGE_VERSION[@],@PACKAGE_VERSION@,g' \
	-e 's![@]PACKAGE_NAME[@]!@PACKAGE_NAME@!g' \
	-e 's,[@]PACKAGE_TARNAME[@],@PACKAGE_TARNAME@,g' \
	-e 's,[@]PACKAGE_BUGREPORT[@],@PACKAGE_BUGREPORT@,g' \
	-e 's,[@]RELEASE_MONTH[@],@RELEASE_MONTH@,g' \
	-e 's,[@]INSTALL[@],$(INSTALL),g' \
	-e 's,[@]INSTALL_PROGRAM[@],$(INSTALL_PROGRAM),g' \
	-e 's,[@]INSTALL_SCRIPT[@],$(INSTALL_SCRIPT),g' \
	-e 's,[@]INSTALL_DATA[@],$(INSTALL_DATA),g' \
	-e 's,[@]LIB_VERSION_CURRENT[@],@LIB_VERSION_CURRENT@,g' \
	-e 's,[@]LIB_VERSION_REVISION[@],@LIB_VERSION_REVISION@,g' \
	-e 's,[@]LIB_VERSION_AGE[@],@LIB_VERSION_AGE@,g' \
	-e 's,[@]LIB_VERSION_INFO[@],@LIB_VERSION_INFO@,g' \
	-e 's,[@]LIB_RELEASE_INFO[@],@LIB_RELEASE_INFO@,g' \
	-e 's,[@]prefix[@],$(prefix),g' \
	-e 's,[@]datadir[@],$(datadir),g' \
	-e 's,[@]sbindir[@],$(sbindir),g' \
	-e 's,[@]sysconfdir[@],$(sysconfdir),g' \
	-e 's,[@]localstatedir[@],$(localstatedir),g'
do_subst_fn = \
		echo "do_subst < ${@}.in > ${@}"; \
		mkdir -p `dirname ${@}` || exit 1; \
		${do_subst} < $(srcdir)/${@}.in > ${@} || exit 1; \
		chmod 0755 ${@}
do_subst_dt = \
		echo "do_subst < ${@}.in > ${@}"; \
		mkdir -p `dirname ${@}` || exit 1; \
		${do_subst} < $(srcdir)/${@}.in > ${@} || exit 1; \
		chmod 0644 ${@}


# lists
BUILT_SOURCES				=
TESTS					=
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= build-aux/autogen.sh \
					  build-aux/git-package-version \
					  build-aux/git-package-version.sh
CLEANFILES				= \
					  $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings


# custom targets
.PHONY: autogen update gitclean build-aux/git-package-version

build-aux/git-package-version:
	$(top_srcdir)/build-aux/git-package-version.sh $(top_srcdir)

autogen:
	$(top_srcdir)/build-aux/autogen.sh
	test -x ./config.status && { ./config.status --recheck || exit $?; } || exit 0

update:
	git --git-dir=$(top_srcdir)/.git pull 2> /dev/null || exit 0
	$(MAKE) -f Makefile autogen

gitclean:
	$(MAKE) -f Makefile distclean
	rm -f  $(top_srcdir)/INSTALL
	rm -f  $(top_srcdir)/Makefile.in
	rm -f  $(top_srcdir)/aclocal.m4
	rm -f  $(top_srcdir)/config.h.in
	rm -f  $(top_srcdir)/configure
	rm -f  $(top_srcdir)/m4/*
	rm -f  $(top_srcdir)/build-aux/c* $(top_srcdir)/build-aux/d*
	rm -f  $(top_srcdir)/build-aux/i* $(top_srcdir)/build-aux/l*
	rm -f  $(top_srcdir)/build-aux/m*
	rm -f  $(top_srcdir)/build-aux/git-package-version


# local targets
install-exec-local:

install-data-local:

uninstall-local:

clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache
	rm -f  $(srcdir)/GTAGS $(srcdir)/GRTAGS $(srcdir)/GPATH

# end of automake file
