#!/bin/sh

AUTOGENNAME=`basename ${0}` || exit $?
AUTOSRCDIR=`dirname ${0}`   || exit $?
AUTOSRCDIR=${AUTOSRCDIR}/..

set -e

echo "${AUTOGENNAME}: running \"autoreconf -i -f -I m4 ${AUTOSRCDIR}\""
autoreconf -i -f -I m4 ${AUTOSRCDIR} || exit $?

echo "${AUTOGENNAME}: removing temp files"
rm -rf ${AUTOSRCDIR}/autom4te.cache || exit $?

# end of script
