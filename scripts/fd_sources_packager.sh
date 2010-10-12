#!/bin/sh

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2010  Eric MAEKER, MD
#
# This script will prepare the source package for freediams only

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

cd $SCRIPT_PATH"/.."

echo "**** REMOVING DOCUMENTATIONS ****"
rm -R global_resources/doc/freediams/fr/html/*
rm -R global_resources/doc/freediams/en/html/*


# remove executable flags to files
#echo "Updating files rights : step 1"
#sudo find . -type f -exec chmod -R 666 {} \;
#echo "Updating files rights : step 2"
#sudo find . -type f -name "*.sh" -exec chmod -R 777 {} \; 

export COPYFILE_DISABLE=true

# get version number of FreeDiams from the project file
VERSION=`cat ./freediams/freediams.pro | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`
# create sources tmp path
PACKPATH=$SCRIPT_PATH"/freediams-"$VERSION
if [ -e $PACKPATH ]; then
    rm -R $PACKPATH
fi
mkdir $PACKPATH

echo "**** PREPARE SOURCES PACKAGE ****"
tar -cf $PACKPATH/sources.tar \
--exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' --exclude 'rushes' \
--exclude 'Makefile*' --exclude '*.pro.user*' \
--exclude 'dosages.db' --exclude 'users.db' --exclude '.*' --exclude '._*' \
--exclude '*.tgz' --exclude '*.app' --exclude '*.zip' \
--exclude 'global_resources/forms' \
--exclude 'global_resources/databases/episodes' \
--exclude 'global_resources/databases/patients' \
--exclude 'global_resources/databases/templates' \
--exclude 'global_resources/databases/users' \
--exclude 'sources.tar' \
freediams.pro config.pri checkqtversion.pri \
README COPYING INSTALL \
updatetranslations.sh \
buildspecs \
doc \
freediams \
global_resources/databases/drugs/drugs*.db \
global_resources/databases/drugs/iam*.db \
global_resources/doc \
global_resources/textfiles/freediams.desktop \
global_resources/pixmap \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
libs \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/drugsbaseplugin \
plugins/drugsplugin \
plugins/emptyplugin \
plugins/fdcoreplugin \
plugins/fdmainwindowplugin \
plugins/listviewplugin \
plugins/printerplugin \
plugins/saverestoreplugin \
plugins/templatesplugin \
plugins/texteditorplugin \
scripts \
tests

echo "**** UNPACK SOURCES PACKAGE TO CREATED DIR ****"
tar xf $PACKPATH/sources.tar -C $PACKPATH
rm $PACKPATH/sources.tar

echo "**** REPACK SOURCES PACKAGE FROM CREATED DIR ****"
cd $SCRIPT_PATH
tar czf "../freediamsfullsources-"$VERSION".tgz"  "./freediams-"$VERSION

exit 0
