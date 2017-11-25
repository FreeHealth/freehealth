#!/bin/sh
#/***************************************************************************
# *  The FreeMedForms project is a set of free, open source medical         *
# *  applications.                                                          *
# *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
# *  All rights reserved.                                                   *
# *                                                                         *
# *  This program is free software: you can redistribute it and/or modify   *
# *  it under the terms of the GNU General Public License as published by   *
# *  the Free Software Foundation, either version 3 of the License, or      *
# *  (at your option) any later version.                                    *
# *                                                                         *
# *  This program is distributed in the hope that it will be useful,        *
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
# *  GNU General Public License for more details.                           *
# *                                                                         *
# *  You should have received a copy of the GNU General Public License      *
# *  along with this program.                                               *
# *  If not, see <http://www.gnu.org/licenses/>.                            *
# ***************************************************************************/
#/***************************************************************************
# *  Main developers : Eric MAEKER, <eric.maeker@gmail.com>                 *
# *  Contributors:                                                          *
# *       Jerome Pinguet <jerome@jerome.cc>                                           *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# ***************************************************************************/

BUNDLE_NAME=""
GIT_REVISION=""
PROJECT_VERSION=""
PACKPATH=""
SOURCES_ROOT_PATH=""
GPG_KEY=""
SED_INPLACE="-ibkup" # on macos change to "-i bkup"
GITHASH=`git rev-parse HEAD` 

# Some path definition
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
    SCRIPT_PATH=`dirname $0`
else
    SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi
SOURCES_ROOT_PATH=$SCRIPT_PATH"/../"

# get version number of the project
PROJECT_VERSION=`cat $SOURCES_ROOT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" | cut -d = -s -f2 | tr -d ' '`

# file naming
ZIP_FILENAME="freehealth_$PROJECT_VERSION.orig.tar.gz"
PARENT_PATH="freehealth-$PROJECT_VERSION" # root dir name in the zipfile
ZIP_PATH="source_package/"$GITHASH # Default path to store the source tar.gz file starting from the RootSourcePath

showHelp()
{
    echo "$SCRIPT_NAME builds FreeHealth source package, GIT branches and tags."
    echo "Project version: $PROJECT_VERSION"
    echo
    echo "Usage: $SCRIPT_NAME"
    echo "Options:"
    echo "  -d  Include eDRC non-free datapack in datapacks/appinstalled"
    echo "  -h  Show this help"
    echo "  -k pgpkey    Use this specific key to sign the package (indicate key long ID starting with 0x)"
    echo "  -p path to folder where the tar.gz archive will be created"
    echo
}

cd $SCRIPT_PATH"/.."

# remove executable flags to doc files
#echo "Updating files rights : step 1"
#sudo find . -type f -exec chmod -R 666 {} \;
#echo "Updating files rights : step 2"
#sudo find . -type f -name "*.sh" -exec chmod -R 777 {} \;

export COPYFILE_DISABLE=true

# prepare the source tar.gz file
prepareFileSelection()
{
echo "**** PREPARE SOURCES PACKAGE ****"

SCRIPT_SOURCE="\
scripts/mac/*.sh \
scripts/Linux/* \
scripts/source.sh \
scripts/win/* \
scripts/debug/* \
"

RESOURCES="\
global_resources/datapacks/appinstalled/defaultservers.txt \
global_resources/datapacks/appinstalled/drugs/master.db \
global_resources/datapacks/appinstalled/drugs/readme.txt \
global_resources/doc/freediams \
global_resources/doc/freedrc \
global_resources/doc/freeicd \
global_resources/doc/freehealth \
global_resources/doc/freepad \
global_resources/doc/freetoolbox \
global_resources/forms \
global_resources/package_helpers/freediams* \
global_resources/package_helpers/freedrc* \
global_resources/package_helpers/freeicd* \
global_resources/package_helpers/freehealth* \
global_resources/package_helpers/freepad* \
global_resources/package_helpers/freetool* \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/others \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/pixmap/svg/*.bmp \
global_resources/profiles \
global_resources/sql/drugdb \
global_resources/sql/druginfodb \
global_resources/sql/icd10 \
global_resources/sql/server_config \
global_resources/sql/zipcodes \
global_resources/textfiles/boys_surnames.csv \
global_resources/textfiles/default_user_footer.htm \
global_resources/textfiles/default_user_header.htm \
global_resources/textfiles/girls_surnames.csv \
global_resources/textfiles/haarcascade_frontalface_alt2.xml \
global_resources/textfiles/listemotsfr.txt \
global_resources/textfiles/surnames.txt \
global_resources/textfiles/*.db \
global_resources/textfiles/zipcodes.csv \
global_resources/textfiles/freediamstest \
global_resources/textfiles/oldprescriptionsfiles \
global_resources/textfiles/edrc \
global_resources/textfiles/prescription \
global_resources/translations/*.ts \
"

BUILDSPEC_SOURCES="\
README.md README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
buildspecs/*.in \
doc \
"

LIBS_SOURCES="\
libs/aggregation \
libs/calendar \
libs/datapackutils \
libs/extensionsystem \
libs/medicalutils \
libs/medintuxutils \
libs/translationutils \
libs/utils \
libs/*.pri \
contrib \
"

APP_SOURCES="\
freediams.pro \
freediams \
freedrc.pro \
freedrc \
freeicd.pro \
freeicd \
freehealth \
freepad.pro \
freepad \
freetoolbox.pro \
freetoolbox \
"

PLUGINS_SOURCES="\
plugins/fmf_plugins.pri \
plugins/pluginjsonmetadata.xsl \
plugins/agendaplugin \
plugins/aggirplugin \
plugins/alertplugin \
plugins/basewidgetsplugin \
plugins/categoryplugin \
plugins/coreplugin \
plugins/datapackplugin \
plugins/druginteractionsplugin \
plugins/drugsbaseplugin \
plugins/drugsplugin \
plugins/edrcplugin \
plugins/emptyplugin \
plugins/feedbackplugin \
plugins/fmfcoreplugin \
plugins/fmfmainwindowplugin \
plugins/formmanagerplugin \
plugins/icdplugin \
plugins/identityplugin \
plugins/listviewplugin \
plugins/padtoolsplugin \
plugins/patientbaseplugin \
plugins/patientimportplugin \
plugins/pmhplugin \
plugins/printerplugin \
plugins/recordimportplugin \
plugins/saverestoreplugin \
plugins/scriptplugin \
plugins/templatesplugin \
plugins/texteditorplugin \
plugins/toolsplugin \
plugins/usermanagerplugin \
plugins/webcamplugin \
plugins/xmlioplugin \
plugins/zipcodesplugin \
"

TEST_SOURCES="\
tests \
"

SELECTED_SOURCES=$SCRIPT_SOURCE$RESOURCES$BUILDSPEC_SOURCES$LIBS_SOURCES$APP_SOURCES$PLUGINS_SOURCES$TEST_SOURCES
}

includeEdrcFiles()
{
SELECTED_SOURCES=$SELECTED_SOURCES" \
global_resources/datapacks/appinstalled/edrc_ro/edrc.db \
global_resources/datapacks/appinstalled/edrc_ro/readme.txt
"
}

createSource()
{
    # create sources tmp path
    PACKPATH=$SCRIPT_PATH/$PARENT_PATH
    if [ -e $PACKPATH ]; then
        rm -R $PACKPATH
    fi
    mkdir $PACKPATH

    tar -cf $PACKPATH/sources.tar \
    --exclude '.git' --exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
    --exclude '_protected' --exclude '__nonfree__' --exclude 'nonfree' \
    --exclude 'build' --exclude 'bin' --exclude 'packages' --exclude 'zlib-1.2.3' \
    --exclude 'rushes' --exclude 'doxygen' \
    --exclude 'Makefile*' --exclude '*.pro.user*' --exclude '*bkup' --exclude '*autosave' \
    --exclude 'dosages.db' --exclude 'users.db' --exclude '*.mdb' --exclude '.*' --exclude '._*' \
    --exclude '*.app' --exclude '*.zip' --exclude '*.a' \
    --exclude '*.o' --exclude 'moc_*' --exclude 'ui_*.h' --exclude '*.dylib' \
    --exclude 'global_resources/databases' \
    --exclude 'sources.tar' --exclude '.qmake.cache' --exclude '.qmake.stash' \
    $EXCLUSIONS \
    $SELECTED_SOURCES

    echo "**** UNPACK SOURCES PACKAGE TO CREATED DIR ****"
    tar xf $PACKPATH/sources.tar -C $PACKPATH
    rm $PACKPATH/sources.tar
    find $PACKPATH -type f -exec chmod -R 666 {} \;

    echo "   * DEFINING *.ISS FILES APP VERSION"
    cd $PACKPATH/global_resources/package_helpers
    FILES=`find ./ -type f -name '*.iss'`
    for f in $FILES; do
        # Since version 5.3 there is a unicode version of Inno Setup that supports... unicode!
        # Download the  Unicode version (tested with isetup-5.5.5-unicode.exe
        # Lines using older non Unicode version of Inno Setup have been commented out
        # (ISS files are latin1, sed can generate error on utf8 system working on latin1 files)
        # (So convert the charset, sed the file, convert the charset)
        # iconv -f latin1 -t utf-8 $f > $f.utf8
        sed $SED_INPLACE 's#__version__#'$PROJECT_VERSION'#' $f
        # iconv -f utf-8 -t latin1 $f.utf8 > $f
        # rm $f.utf8
    done
    rm *.*bkup
    echo "   * DEFINING *.BAT FILES APP VERSION"
    cd $PACKPATH/scripts
    FILES=`find ./ -type f -name '*.bat'`
    for f in $FILES; do
        sed $SED_INPLACE 's#__version__#'$PROJECT_VERSION'#' $f
    done
    rm *.*bkup

    echo "   * DEFINING *.PLUGINSPEC FILES APP VERSION"
    cd $PACKPATH
    FILES=`find ./ -type f -name '*.pluginspec'`
    NON_ALPHABETA_PROJECT_VERSION=`echo $PROJECT_VERSION | tr '~' '.' | tr '-' '.' | cut -d"." -f1,2,3`
    for f in $FILES; do
        sed $SED_INPLACE 's#compatVersion=\".*\"#compatVersion=\"'$NON_ALPHABETA_PROJECT_VERSION'\"#' $f
        sed $SED_INPLACE 's#version=\".*\" #version=\"'$NON_ALPHABETA_PROJECT_VERSION'\" #' $f
        sed $SED_INPLACE 's#version=\".*\"/>#version=\"'$NON_ALPHABETA_PROJECT_VERSION'\"/>#' $f
    done

    echo "   * ADDING LIBRARY VERSION NUMBER"
    cd $PACKPATH/libs
    find . -type f -name '*.pro' -exec sed $SED_INPLACE 's/# VERSION=1.0.0/!win32 {VERSION='$NON_ALPHABETA_PROJECT_VERSION'}/' {} \;

    echo "   * REMOVING TEST VERSION IN FORMS"
    cd $PACKPATH/global_resources/forms
    find . -type f -name '*.xml' -exec sed $SED_INPLACE 's#<version>test</version>#<version>'$NON_ALPHABETA_PROJECT_VERSION'</version>#' {} \;

    # git version is computed in the buildspecs/githash.pri
    # but the source package needs a static reference
    # while source package does not include the git logs
    echo "   * SETTING GIT revision hash to " $GITHASH
    sed $SED_INPLACE 's/GIT_HASH=.*/GIT_HASH='$GITHASH'/' $PACKPATH/buildspecs/githash.pri

    # Remove all backup files
    find . -type f -name '*bkup' -exec rm {} \;
    find . -type f -name '.qmake.stash' -exec rm {} \;

    echo "**** REPACK SOURCES PACKAGE FROM CREATED DIR ****"
    cd $SCRIPT_PATH
    if [ -z "$TARGET_PATH" ]
    then
      mkdir -p ../$ZIP_PATH                                                       
      tar czf ../$ZIP_PATH/$ZIP_FILENAME  ./$PARENT_PATH 
    else
      tar czf $TARGET_PATH/$ZIP_FILENAME  ./$PARENT_PATH
    fi

    echo "**** SIGNING SOURCE FILE, KEY: $GPG_KEY ****"
    # --armor: GPG signature in ASCII Armor format: This format is more human readable and more portable.
    gpg -u $GPG_KEY --armor --output ../$ZIP_PATH/$ZIP_FILENAME.sig --detach-sig ../$ZIP_PATH/$ZIP_FILENAME

    echo "**** CLEANING TMP SOURCES PATH ****"
    rm -R $PACKPATH

    if [ -z "$TARGET_PATH" ]                                                    
    then                                                                        
      cd ..
      echo "*** Source package: `pwd`/$ZIP_PATH/$ZIP_FILENAME"
    else                                                                        
      echo "*** Source package: $TARGET_PATH/$ZIP_FILENAME"                         
    fi
}

#########################################################################################
## Analyse options
#########################################################################################

prepareFileSelection

while getopts "hdkp:" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    d) includeEdrcFiles
    ;;
    k) GPG_KEY=$OPTARG
    ;;
    p) TARGET_PATH=$OPTARG
    ;;
  esac
done

createSource

exit 0
