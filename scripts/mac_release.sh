#!/bin/sh
#/***************************************************************************
# *  The FreeMedForms project is a set of free, open source medical         *
# *  applications.                                                          *
# *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
# *   Contributors :                                                        *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# ***************************************************************************/


# Binary to use for the qmake step
QMAKE_BIN="qmake"
QMAKE_SPEC=""
MAKE_JOBS=4

# Scripts names and paths
SCRIPT_NAME=""
SCRIPT_PATH=""
MAC_SCRIPTS_PATH=""
SOURCES_PATH=""
PACKAGES_PATH=""

# Qt Prject file to build (contains absolute file path)
PROJECT_FILE=""
BUNDLE_NAME="FreeMedForms FreeDiams FreeAccount"
PROJECT="" # lowered case of BundleName

# Get scripts names and paths
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

MAC_SCRIPTS_PATH=$SCRIPT_PATH
SOURCES_PATH=$SCRIPT_PATH"../"
PACKAGES_PATH=$SCRIPT_PATH"../packages"
# get version number from the project file
VERSION=`cat $SOURCES_PATH$PROJECT/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

showHelp()
{
  echo $SCRIPT_NAME" builds FreeMedForms applications into Mac bundle in release mode."
  echo "Usage : $SCRIPT_NAME -b CaseSensitiveBundle <options>"
  echo "Options :"
  echo " -b  Bundle name (optionnal)"
  echo " -j  jobs for the make process (see also make -j jobs)"
  echo " -s  Build from source package (you need to create the source package)"
  echo " -h  show this help"
  echo "Win32 port can be build under Linux using crosscompilation"
}

buildFromSourcePackage()
{
  SOURCE_PACKAGE=$SOURCES_PATH"freemedformsfullsources-"$VERSION".tgz"
  # test source package
  if [ ! -e $SOURCE_PACKAGE ] ; then
      echo $SOURCE_PACKAGE" not found"
      exit 123;
  fi
  SOURCES_PATH=$SCRIPT_PATH"freemedforms-"$VERSION"/"
#  if [ -e $SOURCES_PATH ] ; then
#    rm -Rf $SOURCES_PATH
#  fi
  TO=$SCRIPT_PATH
  tar xzf $SOURCE_PACKAGE -C $TO
  PACKAGES_PATH=$SOURCES_PATH"/packages/"
  PROJECT_FILE=$SOURCES_PATH$PROJECT.pro;
}

buildTranslations()
{
  cd $SOURCES_PATH
  MAKE_STEP=`lrelease ./global_resources/translations/*.ts`
  MAKE_STEP=$?
  if [ ! $MAKE_STEP = 0 ]; then
    echo "   *** Error: lrelease step wrong ***"
    exit 123
  else
    echo "   *** Translations created"
  fi
}

buildApp()
{
  #####################################
  # 1. make clean                     #
  #####################################
  echo "*** Cleaning build path..."
  cd $SOURCES_PATH
  rm -R ./bin/$PROJECT/plugins

  #rm -R build
  #rm -R bin
  #$QMAKE_BIN && make clean


  #####################################
  # 2. Build process                  #
  #####################################
  # build app
  echo "*** Building "$BUNDLE_NAME" "$VERSION
  cd $SOURCES_PATH/$PROJECT
   QMAKE_SPEC="-r -spec macx-g++ CONFIG+=release CONFIG-=debug_and_release CONFIG-=debug LOWERED_APPNAME=$PROJECT"
   echo "   --> $QMAKE_BIN $PROJECT_FILE $QMAKE_SPEC"

   MAKE_STEP=`$QMAKE_BIN $PROJECT_FILE $QMAKE_SPEC`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: qmake step wrong ***"
     exit 123
   else
     echo "   *** QMake done"
   fi

   echo "   --> make -j $MAKE_JOBS"
   MAKE_STEP=`make -j $MAKE_JOBS`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: make step wrong ***"
     exit 123
   else
     echo "   *** Make done"
   fi

   echo "   --> make install"
   MAKE_STEP=`make install`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: install step wrong ***"
     exit 123
   else
     echo "   *** Make Install done"
   fi
}

linkQtLibs()
{
   # link frameworks
   echo "*** Linking Frameworks..."
   cd $PACKAGES_PATH/mac/$BUNDLE_NAME
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "*** Error: Installation not found ***"
     exit 123
   else
     echo "moving to cd $PACKAGES_PATH/mac/$BUNDLE_NAME"
   fi

   MAKE_STEP=`$MAC_SCRIPTS_PATH/macDeploy.sh -a $BUNDLE_NAME -p iconengines -p imageformats -p sqldrivers -p accessible`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
   echo "*** Error: Deployement step wrong ***"
   exit 123
   fi
}

linkMySqlLib()
{
   # Deploy MySql lib
   ACTUAL_PATH=`pwd`
   cd $PACKAGES_PATH/mac/$BUNDLE_NAME/$BUNDLE_NAME.app
   MYSQL_PLUGIN="./Contents/plugins/qt/sqldrivers/libqsqlmysql.dylib"
   FRAMEWORK_PATH="./Contents/Frameworks"
   mysqllib=""
   for n in `otool -LX "$MYSQL_PLUGIN" | grep mysqlclient` ; do
     path=`echo $n | grep mysqlclient`
     if [ $path ] ; then
       mysqllib=$path
       echo $path
     fi
     done
   #    if [ -e $mysqllib ] ; then
   #        echo "No MySql client to install"
   #    else
   echo "Linking MySql client lib from $mysqllib to $FRAMEWORK_PATH for $BUNDLE_NAME"
   mysqllib_source=`basename $mysqllib`
   mysqllib_source=`locate $mysqllib_source`
   cp $mysqllib_source $FRAMEWORK_PATH
   name=`basename $mysqllib`
   install_name_tool -change $mysqllib @executable_path/../Frameworks/$name "$MYSQL_PLUGIN"
   #    fi
   cd $ACTUAL_PATH
}

createDmg()
{
   # clean old dmg and create new one
   echo "*** Creating DMG archive..."
   rm *.dmg
   MAKE_STEP=`$MAC_SCRIPTS_PATH/release_dmg.sh -a $BUNDLE_NAME -p ./ -s 150 -f $MAC_SCRIPTS_PATH/../`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "*** Error: DMG creation step wrong ***"
     exit 123
   fi

   # move archive to right place
   mv $BUNDLE_NAME.dmg $PACKAGES_PATH/$BUNDLE_NAME-$VERSION.dmg
 }

while getopts "j:b:sh" option
do
#echo "(-- option:$option  $OPTIND - '$OPTARG' --)"
        case $option in
                j) MAKE_JOBS=$OPTARG;
                ;;
                b) BUNDLE_NAME=$OPTARG;
                ;;
                h) showHelp
                    exit 0
                ;;
                s) buildFromSourcePackage
        esac
done

if [ -z "$PROJECT_FILE" ] ; then
    echo "ERROR: you must specify the project file to compile."
    showHelp
    exit 123
fi
if [ -z "$BUNDLE_NAME" ] ; then
    echo "ERROR: you must specify the bundle name of the project."
    showHelp
    exit 123
fi

if [ -z $VERSION ] ; then
   echo "ERROR: No version number found."
   exit
fi

if [ ! -e $PACKAGES_PATH ] ; then
   mkdir $PACKAGES_PATH
fi

echo "*** Creating package for $BUNDLE_NAME $VERSION"
echo "      * project file $PROJECT_FILE"
echo "      * from source path $SOURCES_PATH"
echo "      * to package path $PACKAGES_PATH"

buildTranslations

# build all projects
TMP=$BUNDLE_NAME
for i in $TMP; do
  BUNDLE_NAME=$i
  PROJECT=`echo $i | tr '[A-Z]' '[a-z]'`;
  PROJECT_FILE=$SOURCES_PATH$PROJECT/$PROJECT.pro;
  buildApp
  linkQtLibs
  linkMySqlLib
  createDmg
done

# move dmg files to script path
cp $PACKAGES_PATH/*.dmg $SCRIPT_PATH

exit 0
