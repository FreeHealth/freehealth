#!/bin/sh
#/***************************************************************************
# *  The FreeMedForms project is a set of free, open source medical         *
# *  applications.                                                          *
# *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
# *  along with this program (COPYING.FREEMEDFORMS file).                   *
# *  If not, see <http://www.gnu.org/licenses/>.                            *
# ***************************************************************************/
#/***************************************************************************
# *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
# *  Contributors:                                                          *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# ***************************************************************************/

BUNDLE_NAME=""
GIT_REVISION=""
PROJECT_VERSION=""
PACKPATH=""
SOURCES_ROOT_PATH=""

# Some path definition
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi
SOURCES_ROOT_PATH=$SCRIPT_PATH"/../"

# get version number of FreeDiams from the project file
PROJECT_VERSION=`cat $SOURCES_ROOT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

showHelp()
{
echo "$SCRIPT_NAME builds FreeMedForms source package, GIT branches and tags."
echo "Project version: $PROJECT_VERSION"
echo
echo "Usage: $SCRIPT_NAME -r 123"
echo "Options:"
#echo "  -s  Build the source package"
#echo "  -c  Create a branch. Specify the revision number using the -r option"
#echo "  -t  Create the tag. Specify the revision number using the -r option"
echo "  -r  Specify the revision number to use for the branches or tags generation"
echo "  -h  Show this help"
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
global_resources/datapacks/appinstalled/account/readme.txt \
global_resources/doc/freeaccount \
global_resources/doc/freediams \
global_resources/doc/freeicd \
global_resources/doc/freemedforms \
global_resources/doc/freepad \
global_resources/doc/freetoolbox \
global_resources/forms \
global_resources/package_helpers/freeaccount* \
global_resources/package_helpers/freediams* \
global_resources/package_helpers/freeicd* \
global_resources/package_helpers/freemedforms* \
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
global_resources/sql/account \
global_resources/sql/drugdb \
global_resources/sql/druginfodb \
global_resources/sql/icd10 \
global_resources/sql/server_config \
global_resources/sql/zipcodes \
global_resources/textfiles/boys_surnames.csv \
global_resources/textfiles/default_user_footer.htm \
global_resources/textfiles/default_user_header.htm \
global_resources/textfiles/girls_surnames.csv \
global_resources/textfiles/listemotsfr.txt \
global_resources/textfiles/surnames.txt \
global_resources/textfiles/*.db \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
"

BUILDSPEC_SOURCES="\
README.txt COPYING.txt INSTALL \
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
freeaccount.pro \
freeaccount \
freediams.pro \
freediams \
freeicd.pro \
freeicd \
freemedforms.pro \
freemedforms \
freepad.pro \
freepad \
freetoolbox.pro \
freetoolbox \
"

PLUGINS_SOURCES="\
plugins/fmf_plugins.pri \
plugins/pluginjsonmetadata.xsl \
plugins/accountbaseplugin \
plugins/accountplugin \
plugins/account2plugin \
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
plugins/emptyplugin \
plugins/feedbackplugin \
plugins/fmfcoreplugin \
plugins/fmfmainwindowplugin \
plugins/formmanagerplugin \
plugins/icdplugin \
plugins/identityplugin \
plugins/listviewplugin \
plugins/padtoolsplugin \
plugins/patientbaseplugin
plugins/pmhplugin \
plugins/printerplugin \
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

createSource()
{
# create sources tmp path
PACKPATH=$SCRIPT_PATH/freemedforms-$PROJECT_VERSION
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
--exclude 'sources.tar' \
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
  sed -i "bkup" 's#__version__#'$PROJECT_VERSION'#' $f
done
rm *.*bkup
echo "   * DEFINING *.BAT FILES APP VERSION"
cd $PACKPATH/scripts
FILES=`find ./ -type f -name '*.bat'`
for f in $FILES; do
  sed -i "bkup" 's#__version__#'$PROJECT_VERSION'#' $f
done
rm *.*bkup

echo "   * DEFINING *.PLUGINSPEC FILES APP VERSION"
cd $PACKPATH
FILES=`find ./ -type f -name '*.pluginspec'`
NON_ALPHABETA_PROJECT_VERSION=`echo $PROJECT_VERSION | tr '~' '.' | tr '-' '.' | cut -d"." -f1,2,3`
for f in $FILES; do
  # compatVersion="0.6.0"
  sed -i "bkup" 's#compatVersion=\".*\"#compatVersion=\"'$NON_ALPHABETA_PROJECT_VERSION'\"#' $f
  rm $f"bkup"
  # version="0.6.0"
  sed -i "bkup" 's#version=\".*\" #version=\"'$NON_ALPHABETA_PROJECT_VERSION'\" #' $f
  rm $f"bkup"
  sed -i "bkup" 's#version=\".*\"/>#version=\"'$NON_ALPHABETA_PROJECT_VERSION'\"/>#' $f
  rm $f"bkup"
done

echo "   * ADDING LIBRARY VERSION NUMBER"
cd $PACKPATH/libs
find . -type f -name '*.pro' -exec sed -i bkup 's/# VERSION=1.0.0/!win32:{VERSION='$NON_ALPHABETA_PROJECT_VERSION'}/' {} \;
find . -type f -name '*.probkup' -exec rm {} \;

echo "   * REMOVING TEST VERSION IN FORMS"
cd $PACKPATH/global_resources/forms
find . -type f -name '*.xml' -exec sed -i bkup 's#<version>test</version>#<version>'$NON_ALPHABETA_PROJECT_VERSION'</version>#' {} \;
find . -type f -name '*.xmlbkup' -exec rm {} \;

# git version is computed in the buildspecs/githash.pri
# but the source package needs a static reference
# while source package does not include the git logs
GITHASH=`git rev-parse HEAD`
echo "   * SETTING GIT revision hash to " $GITHASH
sed -i bkup 's/GIT_HASH=.*/GIT_HASH='$GITHASH'/' $PACKPATH/buildspecs/githash.pri
rm $PACKPATH/buildspecs/githash.pribkup

echo "**** REPACK SOURCES PACKAGE FROM CREATED DIR ****"
cd $SCRIPT_PATH
tar czf ../freemedformsfullsources-$PROJECT_VERSION.tgz  ./freemedforms-$PROJECT_VERSION

echo "**** CLEANING TMP SOURCES PATH ****"
rm -R $PACKPATH

PWD=`pwd`

echo "*** Source package successfully created at `pwd`./freemedforms-$PROJECT_VERSION"
}

# params:
# $1: bundleName
# $2: fromSVNVersion
createBranch()
{
  echo "Create SVN branch not yet coded"
  # Manage Branch name
  SVN_FROM=""
  BRANCH_NAME=$1"_"$PROJECT_VERSION
  if [ ! -e $SVN_REVISION ] ; then
     BRANCH_NAME=$BRANCH_NAME"-"$SVN_REVISION
     SVN_FROM="-r "$SVN_REVISION
  fi

  # create a tmp path
  mkdir $SCRIPT_PATH/tmp/branches
  cd $SCRIPT_PATH/tmp/branches
  # create branch
  if [ ! -e branches/$BRANCH_NAME ] ; then
    echo mkdir branches/$BRANCH_NAME
    echo svn add branches
    # todo: else clear the branch ??
  fi
  for n in $SELECTED_SOURCES ; do
    echo svn copy $SVN_FROM https://freemedforms.googlecode.com/svn/trunk/$n branches/$BRANCH_NAME/$n
  done
  echo "svn commit -m \"Creating branch for "$BRANCH_NAME"\""

  # remove tmp path
  cd ../..
  echo rm -R $SCRIPT_PATH/tmp/branches
}

# params:
# $1: bundleName
# $2: fromSVNVersion
createTag()
{
  echo "Create SVN tag not yet coded"
  # Manage Branch name
  BRANCH_NAME=$1"_"$PROJECT_VERSION
  if [ ! -e $SVN_REVISION ] ; then
#     BRANCH_NAME=$BRANCH_NAME"-"$SVN_REVISION
     SVN_FROM="-r "$SVN_REVISION
  fi

  echo "svn cp $SVN_FROM https://freemedforms.googlecode.com/svn/trunk \
                         https://freemedforms.googlecode.com/svn/tags/$BRANCH_NAME \
           -m \"Tag version: $BRANCH_NAME\""

#  # create tag on the svn
#  echo "svn mkdir https://freemedforms.googlecode.com/svn/tags/"$BRANCH_NAME" -m \"Creating tag for "$BRANCH_NAME"\""

#  cd $SCRIPT_PATH

#  # checkout in a tmp dir
#  TMP_PATH=./tmp/tmp_tag_$BRANCH_NAME
#  if [ ! -e $TMP_PATH ]; then
#    mkdir $TMP_PATH
#  fi
#  svn co https://freemedforms.googlecode.com/svn/tags/$BRANCH_NAME ./$TMP_PATH
#  cd $TMP_PATH

#  for n in $SELECTED_SOURCES ; do
#    for f in `svn ls -R $SCRIPT_PATH/../$n`; do
#      FILE_PATH=`dirname $n`
#      if [ -d $n ] ; then
#        FILE_PATH=$n
#      fi

#      FULL_PATH=`dirname $FILE_PATH/$f`
#      echo "----> $FULL_PATH"

#      if [ ! -e $FULL_PATH ] && [ $FULL_PATH != "." ]; then
#        echo "svn mkdir --parents "$FULL_PATH
#        svn mkdir --parents $FULL_PATH
#      fi

#      echo "svn copy "$SVN_FROM" "$SCRIPT_PATH"/../"$FILE_PATH"/"$f" "$FILE_PATH"/"$f
#      svn copy $SVN_FROM $SCRIPT_PATH/../$FILE_PATH/$f $FILE_PATH/$f
#    done
#  done
#  echo "svn commit -m \"tag: "$BRANCH_NAME"\""
}

#########################################################################################
## Analyse options
#########################################################################################

prepareFileSelection

while getopts "hr:sct" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    r) SVN_REVISION=$OPTARG
    ;;
  esac
done

createSource

exit 0
