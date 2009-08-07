#!/bin/sh

# This script is part of FreeMedForms project : http://code.google.com/p/freemedforms
# (c) 2008 - 2009  Eric MAEKER, MD
#
# This script will prepare the source package for freediams only

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

cd ..
rm -R global_resources/doc/freediams/html

tar -czvf freediams.tgz --exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' --exclude 'Makef*' --exclude '*.pro.user' \
--exclude '*.qm' --exclude 'xmlio*' --exclude '*.db' --exclude '.*' --exclude '*.tgz' \
--exclude 'usertoolkit' --exclude 'pluginsmanager' \
freediams.pro config.pri install.pri checkqtversion.pri update-di.txt \
freediams plugins/formwidgets/mfDrugsWidget \
global_resources/databases/drugs global_resources/doc/freediams global_resources/pixmap global_resources/translations \
libs \
doc/usermanual/freediams doc/usermanual/richtexteditor doc/manual-di.pri doc/api doc/api_Doxyfile

cd $SCRIPT_PATH

exit 0
