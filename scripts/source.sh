#!/bin/sh

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2010  Eric MAEKER, MD
#
# This script will prepare the source package for freediams only
# Documentations must installed by hand before
#

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

cd $SCRIPT_PATH"/.."

# remove executable flags to doc files
#echo "Updating files rights : step 1"
#sudo find . -type f -exec chmod -R 666 {} \;
#echo "Updating files rights : step 2"
#sudo find . -type f -name "*.sh" -exec chmod -R 777 {} \; 

export COPYFILE_DISABLE=true

# get version number of FreeDiams from the project file
VERSION=`cat ./$1/$1.pro | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`
# create sources tmp path
PACKPATH=$SCRIPT_PATH/$1-$VERSION
if [ -e $PACKPATH ]; then
    rm -R $PACKPATH
fi
mkdir $PACKPATH

echo "**** PREPARE SOURCES PACKAGE ****"

FREEDIAMS_SOURCES="freediams.pro config.pri checkqtversion.pri \
README COPYING INSTALL \
updatetranslations.sh \
buildspecs \
doc \
freediams \
global_resources/databases/drugs/drugs*.db \
global_resources/databases/drugs/iam*.db \
global_resources/doc/freediams \
global_resources/textfiles/freediams.desktop \
global_resources/pixmap \
global_resources/package_helpers \
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
tests"

FREEICD_SOURCES="freeicd.pro config.pri checkqtversion.pri \
README COPYING INSTALL \
updatetranslations.sh \
buildspecs \
doc \
freeicd \
global_resources/doc/freeicd \
global_resources/textfiles/freeicd.desktop \
global_resources/pixmap \
global_resources/package_helpers \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
global_resources/sql/icd10.sql \
libs \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/icdplugin \
plugins/printerplugin \
plugins/texteditorplugin \
plugins/listviewplugin \
plugins/usermanagerplugin \
scripts"

FREEACCOUNT_SOURCES="freeaccount.pro config.pri checkqtversion.pri \
README COPYING INSTALL \
updatetranslations.sh \
buildspecs \
doc \
freeaccount \
global_resources/doc/freeaccount \
global_resources/textfiles/freeaccount.desktop \
global_resources/pixmap \
global_resources/package_helpers \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
libs \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/accountbaseplugin \
plugins/printerplugin \
plugins/texteditorplugin \
plugins/listviewplugin \
plugins/usermanagerplugin \
scripts"

FREEMEDFORMS_SOURCES="freemedforms.pro config.pri checkqtversion.pri \
README COPYING INSTALL \
updatetranslations.sh \
buildspecs \
doc \
freemedforms \
global_resources/databases/drugs/drugs*.db \
global_resources/databases/drugs/iam*.db \
global_resources/doc/freemedforms \
global_resources/textfiles/freemedforms.desktop \
global_resources/pixmap \
global_resources/package_helpers \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
global_resources/sql/icd10.sql \
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
tests"

SELECTED_SOURCES="";
EXCLUSIONS="";
if [ $1 == "freediams" ] ; then
    SELECTED_SOURCES=$FREEDIAMS_SOURCES
    EXCLUSIONS="--exclude 'global_resources/forms' "
fi
if [ $1 == "freeicd" ] ; then
    SELECTED_SOURCES=$FREEICD_SOURCES
    EXCLUSIONS="--exclude 'global_resources/forms' "
fi
if [ $1 == "freemedforms" ] ; then
    SELECTED_SOURCES=$FREEMEDFORMS_SOURCES
fi
if [ $1 == "freeaccount" ] ; then
    SELECTED_SOURCES=$FREEACCOUNT_SOURCES
fi


tar -cf $PACKPATH/sources.tar \
--exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' --exclude 'rushes' \
--exclude 'Makefile*' --exclude '*.pro.user*' \
--exclude 'dosages.db' --exclude 'users.db' --exclude '.*' --exclude '._*' \
--exclude '*.tgz' --exclude '*.app' --exclude '*.zip' \
--exclude '*.o' --exclude 'moc_*' --exclude 'ui_*.h' \
--exclude 'global_resources/databases/episodes' \
--exclude 'global_resources/databases/patients' \
--exclude 'global_resources/databases/templates' \
--exclude 'global_resources/databases/users' \
--exclude 'global_resources/databases/icd10' \
--exclude 'sources.tar' \
$EXCLUSIONS \
$SELECTED_SOURCES


echo "**** UNPACK SOURCES PACKAGE TO CREATED DIR ****"
tar xf $PACKPATH/sources.tar -C $PACKPATH
rm $PACKPATH/sources.tar
find $PACKPATH -type f -exec chmod -R 666 {} \;

echo "**** REPACK SOURCES PACKAGE FROM CREATED DIR ****"
cd $SCRIPT_PATH
tar czf ../$1fullsources-$VERSION.tgz  ./$1-$VERSION

echo "**** CLEANING TMP SOURCES PATH ****"
rm -R $PACKPATH

exit 0
