#!/bin/sh

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2011  Eric MAEKER, MD
#
# This script will prepare the source package for freediams only
# Documentations must installed by hand before
#


BUNDLE_NAME=""
SVN_REVISION=""
APP_VERSION=""
PACKPATH=""

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

showHelp()
{
echo $SCRIPT_NAME" builds FreeMedForms source package, svn braches and tags."
echo "Usage : $SCRIPT_NAME -b applicationlowcase -r 123 -sct"
echo
echo "Options :"
echo "          -b  Application name (freemedforms, freediams, freeaccount, freeicd, freetoolbox...)"
echo "          -s  Build the source package"
echo "          -s  Build the source package"
echo "          -c  Create a branch. Specify the revision number using the -r option"
echo "          -t  Create the tag. Specify the revision number using the -r option"
echo "          -r  Specify the revision number to use for the branches or tags generation"
echo "          -h  Show this help"
echo
}


cd $SCRIPT_PATH"/.."

# remove executable flags to doc files
#echo "Updating files rights : step 1"
#sudo find . -type f -exec chmod -R 666 {} \;
#echo "Updating files rights : step 2"
#sudo find . -type f -name "*.sh" -exec chmod -R 777 {} \; 

export COPYFILE_DISABLE=true

prepareFileSelection()
{
echo "**** PREPARE SOURCES PACKAGE FOR $1 ****"

# get version number of FreeDiams from the project file
APP_VERSION=`cat ./$1/$1.pro | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

FREEDIAMS_SOURCES="freediams.pro config.pri checkqtversion.pri \
README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
buildspecs/freediams_debian \
doc \
freediams \
global_resources/databases/drugs/master.db \
global_resources/databases/icd10/icd10.db \
global_resources/doc/freediams \
global_resources/package_helpers/freediams* \
global_resources/package_helpers/freemedforms.url \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/textfiles/freediams.desktop \
global_resources/textfiles/boys_surnames.csv \
global_resources/textfiles/default_user_footer.htm \
global_resources/textfiles/default_user_header.htm \
global_resources/textfiles/girls_surnames.csv \
global_resources/textfiles/listemotsfr.txt \
global_resources/textfiles/surnames.txt \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
libs/aggregation \
libs/extensionsystem \
libs/translationutils \
libs/utils \
libs/medintuxutils \
libs/medicalutils \
libs/*.pri \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/drugsbaseplugin \
plugins/drugsplugin \
plugins/emptyplugin \
plugins/listviewplugin \
plugins/printerplugin \
plugins/saverestoreplugin \
plugins/templatesplugin \
plugins/texteditorplugin \
scripts \
tests"

FREEICD_SOURCES="freeicd.pro config.pri checkqtversion.pri \
README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
doc \
freeicd \
global_resources/doc/freeicd \
global_resources/textfiles/freeicd.desktop \
global_resources/textfiles/default_user_header.htm \
global_resources/textfiles/default_user_footer.htm \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/package_helpers/freeicd* \
global_resources/package_helpers/freemedforms.url \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
global_resources/databases/icd10/icd10.db \
global_resources/sql/icd10/icd10.sql \
libs/aggregation \
libs/extensionsystem \
libs/translationutils \
libs/utils \
libs/medintuxutils \
libs/medicalutils \
libs/*.pri \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/icdplugin \
plugins/printerplugin \
plugins/texteditorplugin \
plugins/listviewplugin \
plugins/usermanagerplugin \
scripts"

FREEPAD_SOURCES="freepad.pro config.pri checkqtversion.pri \
README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
doc \
freepad \
global_resources/doc/freepad \
global_resources/textfiles/freepad.desktop \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/package_helpers/freepad* \
global_resources/package_helpers/freemedforms.url \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
libs \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/printerplugin \
plugins/texteditorplugin \
plugins/listviewplugin \
scripts"

FREETOOLBOX_SOURCES="freetoolbox.pro config.pri checkqtversion.pri \
README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
doc \
freetoolbox/free* \
freetoolbox/main.cpp \
freetoolbox/Info.plist \
freetoolbox/libs \
freetoolbox/plugins/coreplugin \
freetoolbox/plugins/drugsdb \
freetoolbox/plugins/icd10db \
freetoolbox/plugins/icdplugin \
freetoolbox/plugins/interactiondb \
freetoolbox/plugins/listviewplugin \
freetoolbox/plugins/zipcodes \
freetoolbox/plugins/*.pro \
global_resources/doc/freetoolbox \
global_resources/textfiles/freetoolbox.desktop \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/package_helpers/freetool* \
global_resources/package_helpers/freemedforms.url \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
global_resources/sql \
libs \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/icdplugin \
plugins/listviewplugin \
scripts"


FREEACCOUNT_SOURCES="freeaccount.pro config.pri checkqtversion.pri \
README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
doc \
freeaccount \
global_resources/databases/zipcodes/zipcodes.db \
global_resources/doc/freeaccount \
global_resources/textfiles/freeaccount.desktop \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/package_helpers/freeaccount* \
global_resources/package_helpers/freemedforms.url \
global_resources/profiles \
global_resources/sql/account \
global_resources/textfiles/boys_surnames.csv \
global_resources/textfiles/default_user_footer.htm \
global_resources/textfiles/default_user_header.htm \
global_resources/textfiles/girls_surnames.csv \
global_resources/textfiles/listemotsfr.txt \
global_resources/textfiles/surnames.txt \
global_resources/textfiles/pays.txt \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
libs/aggregation \
libs/extensionsystem \
libs/translationutils \
libs/utils \
libs/medintuxutils \
libs/medicalutils \
libs/*.pri \
contrib \
plugins/fmf_plugins.pri \
plugins/coreplugin \
plugins/accountbaseplugin \
plugins/accountplugin \
plugins/printerplugin \
plugins/texteditorplugin \
plugins/listviewplugin \
plugins/usermanagerplugin \
plugins/zipcodesplugin \
scripts"

FREEMEDFORMS_SOURCES="freemedforms.pro config.pri checkqtversion.pri \
README.txt COPYING.txt INSTALL \
updatetranslations.sh \
buildspecs/*.pri \
buildspecs/freemedforms_debian \
doc \
freemedforms \
global_resources/databases/drugs/master.db \
global_resources/databases/icd10/icd10.db \
global_resources/databases/zipcodes/zipcodes.db \
global_resources/doc/freemedforms \
global_resources/forms \
global_resources/package_helpers \
global_resources/pixmap/16x16 \
global_resources/pixmap/32x32 \
global_resources/pixmap/48x48 \
global_resources/pixmap/64x64 \
global_resources/pixmap/splashscreens \
global_resources/pixmap/svg/*.svg \
global_resources/pixmap/svg/*.icns \
global_resources/pixmap/svg/*.ico \
global_resources/profiles \
global_resources/sql/account \
global_resources/sql/server_config \
global_resources/textfiles/FreeMedForms.desktop \
global_resources/textfiles/boys_surnames.csv \
global_resources/textfiles/default_user_footer.htm \
global_resources/textfiles/default_user_header.htm \
global_resources/textfiles/girls_surnames.csv \
global_resources/textfiles/listemotsfr.txt \
global_resources/textfiles/surnames.txt \
global_resources/textfiles/pays.txt \
global_resources/textfiles/zipcodes.csv \
global_resources/translations/*.ts \
global_resources/translations/qt*.qm \
libs \
contrib \
plugins/plugins.pro \
plugins/fmf_plugins.pri \
plugins/accountbaseplugin \
plugins/accountplugin \
plugins/aggirplugin \
plugins/agendaplugin \
plugins/basewidgetsplugin \
plugins/categoryplugin \
plugins/coreplugin \
plugins/drugsbaseplugin \
plugins/drugsplugin \
plugins/emptyplugin \
plugins/fmfcoreplugin \
plugins/fmfmainwindowplugin \
plugins/formmanagerplugin \
plugins/icdplugin \
plugins/listviewplugin \
plugins/padtoolsplugin \
plugins/patientbaseplugin
plugins/pmhplugin \
plugins/printerplugin \
plugins/saverestoreplugin \
plugins/templatesplugin \
plugins/texteditorplugin \
plugins/usermanagerplugin \
plugins/xmlioplugin \
plugins/zipcodesplugin \
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
if [ $1 == "freetoolbox" ] ; then
    SELECTED_SOURCES=$FREETOOLBOX_SOURCES
    EXCLUSIONS="--exclude 'global_resources/forms' "
fi
if [ $1 == "freepad" ] ; then
    SELECTED_SOURCES=$FREEPAD_SOURCES
    EXCLUSIONS="--exclude 'global_resources/forms' "
fi
}


# $1 bundle name
# $2 app version
# $3 svn version
changeVersionNumbering()
{
  # modify package helpers: .ISS windows files
  echo "Not yet coded"
}

createSource()
{
# create sources tmp path
PACKPATH=$SCRIPT_PATH/$1-$APP_VERSION
if [ -e $PACKPATH ]; then
    rm -R $PACKPATH
fi
mkdir $PACKPATH

tar -cf $PACKPATH/sources.tar \
--exclude '.svn' --exclude '.cvsignore' --exclude 'qtc-gdbmacros' \
--exclude '_protected' --exclude 'build' --exclude 'bin' --exclude 'packages' \
--exclude 'rushes' --exclude 'doxygen' \
--exclude 'Makefile*' --exclude '*.pro.user*' \
--exclude 'dosages.db' --exclude 'users.db' --exclude '*.mdb' --exclude '.*' --exclude '._*' \
--exclude '*.tgz' --exclude '*.app' --exclude '*.zip' --exclude '*.a' \
--exclude '*.o' --exclude 'moc_*' --exclude 'ui_*.h' \
--exclude 'global_resources/databases/episodes' \
--exclude 'global_resources/databases/patients' \
--exclude 'global_resources/databases/templates' \
--exclude 'global_resources/databases/users' \
--exclude 'sources.tar' \
$EXCLUSIONS \
$SELECTED_SOURCES

echo "**** UNPACK SOURCES PACKAGE TO CREATED DIR ****"
tar xf $PACKPATH/sources.tar -C $PACKPATH
rm $PACKPATH/sources.tar
find $PACKPATH -type f -exec chmod -R 666 {} \;
echo "   * ADDING SVN VERSION NUMBER"
cd $PACKPATH/buildspecs
SVN=`svn info -r HEAD $SCRIPT_PATH"/.." | grep 'Changed\ Rev' | cut -b 19-`
echo "SVN_VERSION=$SVN"  > svnversion.pri
echo 'DEFINES *= "SVN_VERSION=\"\\\\\"$${SVN_VERSION}\\\\\"\""'  >> svnversion.pri
echo ""  >> svnversion.pri

echo "**** REPACK SOURCES PACKAGE FROM CREATED DIR ****"
cd $SCRIPT_PATH
tar czf ../$1fullsources-$APP_VERSION.tgz  ./$1-$APP_VERSION

echo "**** CLEANING TMP SOURCES PATH ****"
rm -R $PACKPATH

PWD=`pwd`

echo "*** Source package for $1 successfully created at"
echo $PWD/$1-$APP_VERSION
echo "SVN version: "$SVN
}

# params:
# $1: bundleName
# $2: fromSVNVersion
createBranch()
{
  echo "Create SVN branch not yet coded"
  # Manage Branch name
  SVN_FROM=""
  BRANCH_NAME=$1"_"$APP_VERSION
  if [ ! -e $SVN_REVISION ] ; then
     BRANCH_NAME=$BRANCH_NAME"-"$SVN_REVISION
     SVN_FROM="-r "$SVN_REVISION
  fi

  # go to trunk root path
  cd $SCRIPT_PATH/../..
  # create branch
  if [ ! -e branches/$BRANCH_NAME ] ; then
    echo mkdir branches/$BRANCH_NAME
    echo svn add branches
    # todo: else clear the branch ??
  fi
  for n in $SELECTED_SOURCES ; do
    echo svn copy $SVN_FROM https://freemedforms.googlecode.com/svn/trunk/$n branches/$BRANCH_NAME/$n
  done
}

# params:
# $1: bundleName
# $2: fromSVNVersion
createTag()
{
  echo "Create SVN tag not yet coded"
  # Manage Branch name
  SVN_FROM=""
  BRANCH_NAME=$1"_"$APP_VERSION
  if [ ! -e $SVN_REVISION ] ; then
     BRANCH_NAME=$BRANCH_NAME"-"$SVN_REVISION
     SVN_FROM="-r "$SVN_REVISION
  fi

  # go to trunk root path
  cd $SCRIPT_PATH/../..
  # create branch
  if [ ! -e tags/$BRANCH_NAME ] ; then
    echo mkdir tags
    echo svn add tags
    echo mkdir tags/$BRANCH_NAME
    # todo: else clear the branch ??
  fi
  for n in $SELECTED_SOURCES ; do
    echo svn copy $SVN_FROM https://freemedforms.googlecode.com/svn/trunk/$n tags/$BRANCH_NAME/$n
  done
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hb:r:sct" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    r) SVN_REVISION=$OPTARG
    ;;
    b) BUNDLE_NAME=$OPTARG
       prepareFileSelection $OPTARG
    ;;
    s) createSource $BUNDLE_NAME
    ;;
    c) createBranch $BUNDLE_NAME
    ;;
    t) createTag $BUNDLE_NAME
  esac
done



exit 0
