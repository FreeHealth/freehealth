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
# rm -R global_resources/doc/fr/*
# rm -R global_resources/doc/en/*

SOURCE_DIRS="buildspecs \
doc \
freediams \
global_resources/doc \
global_resources/pixmap \
libs \
contrib \
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

SOURCE_FILES="\
README COPYING INSTALL \
freediams.pro config.pri checkqtversion.pri \
updatetranslations.sh \
global_resources/databases/drugs/drugs*.db \
global_resources/databases/drugs/iam*.db \
global_resources/textfiles/freediams.desktop \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
plugins/fmf_plugins.pri \
"


FILES_TO_ARCHIVE=""

appendDirFiles() {
    echo "appendDirFiles "$1
    for FILE in $1/*
    do
        FILES_TO_ARCHIVE+=$FILE" "
    done
}

appendDir() {
    echo "Append Dir "$1
    appendDirFiles $1
    if [ -e $1"/*/" ]; then
        for CHILD in `ls -d $1/*/`;
        do
            appendDir $CHILD
        done;
    fi;
}

# Add files
#for SOURCE in $SOURCE_FILES
#do
#   FILES_TO_ARCHIVE+=$SOURCE" "
#done

# Add dirs
#for SOURCE in $SOURCE_DIRS
#do
#    appendDir $SOURCE
#done

#echo $FILES_TO_ARCHIVE > text.txt


#tar -cvf freediams.tgz \
#--exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
#--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' --exclude 'rushes' \
#--exclude 'Makefile*' --exclude '*.pro.user*' \
#--exclude 'dosages.db' --exclude 'users.db' --exclude '\._.*' \
#--exclude '*.tgz' --exclude '*.app' --exclude '*.zip' \
#$FILES_TO_ARCHIVE

export COPYFILE_DISABLE=true

tar -czvf freediams_sources.tgz \
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
tests \


cd $SCRIPT_PATH

exit 0
