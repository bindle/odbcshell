# New ports collection makefile for:   odbcshell
# Date created:        31 March 2001
# Whom:                David M. Syzdek <syzdek@bindlebinaries.com>
#
# $FreeBSD$
#

PORTNAME=      odbcshell
PORTVERSION=   0.0.247.gbad10b6
CATEGORIES=    databases
MASTER_SITES=  http://scm.bindlebinaries.com/pub/software/odbcshell/

MAINTAINER=    syzdek@bindlebinaries.com
COMMENT=       ODBC command line client and script interpreter.

RUN_DEPENDS=
LIB_DEPENDS=   odbc.1:${PORTSDIR}/databases/unixODBC

GNU_CONFIGURE=  yes
USE_GMAKE=      yes
USE_AUTOTOOLS=  libtool
USE_LDCONFIG=   yes
CONFIGURE_ARGS= CPPFLAGS=-I${LOCALBASE}/include LDFLAGS=-L${LOCALBASE}/lib


PLIST_FILES=   bin/odbcshell

.include <bsd.port.mk>
