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
rm -R global_resources/doc/*

tar -czvf freediams.tgz --exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' \
--exclude 'Makef*' --exclude '*.pro.user' \
--exclude 'dosages.db' --exclude 'users.db' --exclude '.*' --exclude '*.tgz' --exclude '*.zip' \
freediams.pro config.pri checkqtversion.pri update-di.txt \
README COPYING INSTALL \
buildspecs \
doc \
freediams \
global_resources \
libs \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/drugsplugin \
plugins/emptyplugin \
plugins/fdcoreplugin \
plugins/fdmainwindowplugin \
plugins/listviewplugin \
plugins/printerplugin \
texteditorplugin \
scripts \
tests \
updatetranslations.sh


cd $SCRIPT_PATH

exit 0
