#!/bin/sh

# This script is part of FreeMedForms project : http://code.google.com/p/freemedforms
# (c) 2008 - 2009  Eric MAEKER, MD
#
# This script will prepare the source package for freemedforms only

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

cd ..

tar -czvf freemedforms.tgz --exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' --exclude 'freediams' \
--exclude 'Makef*' --exclude '*.pro.user' --exclude '*.qm' --exclude '*.db' --exclude '.*' --exclude '*.tgz' \
--exclude 'freediams*' --exclude 'doc/usermanual/freediams' --exclude 'doc/manual-di.pri'  \
--exclude 'doxygen' *

cd $SCRIPT_PATH

exit 0
