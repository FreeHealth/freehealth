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
# *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
# *  Contributors:                                                          *
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

# Qt Project file to build (contains absolute file path)
PROJECT_FILE=""
BUNDLE_NAME="freehealth"
PROJECT="" # lowered case of BundleName
EXTRA_PLUGINS=""

# Get scripts names and paths
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

MAC_SCRIPTS_PATH=$SCRIPT_PATH
SOURCES_PATH=$SCRIPT_PATH"../../"
PACKAGES_PATH=$SOURCES_PATH"/packages"
# get version number from the project file
VERSION=`cat $SOURCES_PATH$PROJECT/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

showHelp()
{
  echo $SCRIPT_NAME" builds FreeHealth applications into Mac bundle in release mode."
  echo "Usage : $SCRIPT_NAME -b CaseSensitiveBundle <options> -p 'with-pad with-alert with-webcam'"
  echo "Options :"
  echo " -b  Bundle name (optionnal)"
  echo " -j  jobs for the make process (see also make -j jobs)"
  echo " -s  Build from source package (you need to create the source package)"
  echo " -p  extra-plugins config (eg='with-pad with-alerts with-webcam')"
  echo " -h  show this help"
  echo "Win32 port can be build under Linux using crosscompilation"
}

buildFromSourcePackage()
{
  echo "*** Building from source package"
  SOURCE_PACKAGE=$SOURCES_PATH"freehealthfullsources-"$VERSION".tgz"
  # test source package
  if [ ! -e $SOURCE_PACKAGE ] ; then
      echo $SOURCE_PACKAGE" not found"
      exit 123;
  fi
  SOURCES_PATH=$SCRIPT_PATH"freehealth-"$VERSION"/"
#  if [ -e $SOURCES_PATH ] ; then
#    rm -Rf $SOURCES_PATH
#  fi
  TO=$SCRIPT_PATH
  echo "   * Using archive: $SOURCE_PACKAGE"
  tar xzf $SOURCE_PACKAGE -C $TO
  PACKAGES_PATH=$SOURCES_PATH"/packages/"
  PROJECT_FILE=$SOURCES_PATH/$PROJECT.pro;
  echo "   * Using project file: $PROJECT_FILE"
}

buildTranslations()
{
  echo "*** Building translations"
  cd $SOURCES_PATH
  MAKE_STEP=`lrelease ./global_resources/translations/*.ts`
  MAKE_STEP=$?
  if [ ! $MAKE_STEP = 0 ]; then
    echo "   *** Error: lrelease step wrong ***"
    exit 123
  else
    echo "   * Translations created"
  fi
}

buildApp()
{
  #####################################
  # 1. make clean                     #
  #####################################
  echo "*** Cleaning build path..."
  cd $SOURCES_PATH
  rm -R ./bin/$PROJECT/

  #rm -R build
  #rm -R bin
  #$QMAKE_BIN && make clean


  #####################################
  # 2. Build process                  #
  #####################################
  # build app
  echo "*** Building "$BUNDLE_NAME" "$VERSION
  cd $SOURCES_PATH/$PROJECT
  if [[ "$EXTRA_PLUGINS" != "" ]]; then
      PLUG_CONFIG=""
      for p in $EXTRA_PLUGINS
      do
          PLUG_CONFIG=$PLUG_CONFIG" CONFIG+=$p"
      done
      QMAKE_SPEC="-r -spec macx-g++ CONFIG+=release CONFIG-=debug_and_release CONFIG-=debug $PLUG_CONFIG"
  else
      QMAKE_SPEC="-r -spec macx-g++ CONFIG+=release CONFIG-=debug_and_release CONFIG-=debug"
  fi
  echo "   * $QMAKE_BIN $PROJECT_FILE $QMAKE_SPEC"

   MAKE_STEP=`$QMAKE_BIN $PROJECT_FILE $QMAKE_SPEC`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: qmake step wrong ***"
     exit 123
   else
     echo "   * QMake done"
   fi

   echo "   --> make -j $MAKE_JOBS"
   MAKE_STEP=`make -j $MAKE_JOBS`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: make step wrong ***"
     exit 123
   else
     echo "   * Make done"
   fi

   echo "   --> make install"
   MAKE_STEP=`make install`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: install step wrong ***"
     exit 123
   else
     echo "   * Make Install done"
   fi
}

linkQtLibs()
{
   # link frameworks
   echo "*** Linking Frameworks..."
   cd $PACKAGES_PATH/mac/$BUNDLE_NAME
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   *** Error: Installation not found ***"
     exit 123
   else
     echo "   * moving to cd $PACKAGES_PATH/mac/$BUNDLE_NAME"
   fi

   MAKE_STEP=`$MAC_SCRIPTS_PATH/macDeploy.sh -a $BUNDLE_NAME -p iconengines -p imageformats -p sqldrivers -p platforms -p printsupport`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
   echo "   *** Error: Deployement step wrong ***"
   exit 123
   fi
}

linkMySqlLib()
{
   # Deploy MySql lib
   echo "*** Linking MySQL plugin"
   ACTUAL_PATH=`pwd`
   cd $PACKAGES_PATH/mac/$BUNDLE_NAME/$BUNDLE_NAME.app
   MYSQL_PLUGIN="./Contents/plugins/qt/sqldrivers/libqsqlmysql.dylib"
   FRAMEWORK_PATH="./Contents/Frameworks"
   mysqllib=""
   for n in `otool -LX "$MYSQL_PLUGIN" | grep mysqlclient` ; do
     path=`echo $n | grep mysqlclient`
     if [ $path ] ; then
       mysqllib=$path
       echo "    $path"
     fi
     done
   #    if [ -e $mysqllib ] ; then
   #        echo "No MySql client to install"
   #    else
   echo "    Linking MySql client lib from $mysqllib to $FRAMEWORK_PATH for $BUNDLE_NAME"
   mysqllib_source=`basename $mysqllib`
   mysqllib_source=`locate $mysqllib_source`
   cp "$mysqllib_source" "$FRAMEWORK_PATH"
   name=`basename $mysqllib`
   install_name_tool -change $mysqllib @executable_path/../Frameworks/$name "$MYSQL_PLUGIN"
   #    fi
   cd $ACTUAL_PATH
}

linkOpenCVLib()
{
    # Deploy OpenCV lib
    echo "*** Linking WebCam plugin"
    ACTUAL_PATH=`pwd`
    cd $PACKAGES_PATH/mac/$BUNDLE_NAME/$BUNDLE_NAME.app
    WEBCAM_PLUGIN="./Contents/plugins/libWebcam.dylib"

    # test source package
    if [ ! -e $WEBCAM_PLUGIN ] ; then
        echo "No WebCam plugin found."
    else
        FRAMEWORK_PATH="./Contents/Frameworks"
        opencvlib=""
        for n in `otool -LX "$WEBCAM_PLUGIN" | grep libopencv` ; do
            path=`echo $n | grep libopencv`
            if [ $path ] ; then
                opencvlib=$path
                echo "    Found lib: $path"
                echo "      Linking OpenCV lib from $opencvlib to $FRAMEWORK_PATH for $BUNDLE_NAME"
                opencvlib_source=`basename $opencvlib`
                opencvlib_source=`locate $opencvlib_source`
                cp $opencvlib_source $FRAMEWORK_PATH
                name=`basename $opencvlib`
                install_name_tool -change $opencvlib @executable_path/../Frameworks/$name "$WEBCAM_PLUGIN"
            fi
        done
    fi
    cd $ACTUAL_PATH
}

createDmg()
{
   # clean old dmg and create new one
   echo "*** Creating DMG archive..."
   rm *.dmg
   MAKE_STEP=`$MAC_SCRIPTS_PATH/release_dmg.sh -a $BUNDLE_NAME -p ./ -s 150 -f $SOURCES_PATH -v $VERSION`
   MAKE_STEP=$?
   if [ ! $MAKE_STEP = 0 ]; then
     echo "   * Error: DMG creation step wrong *"
     exit 123
   fi

   # move archive to right place
   mv $BUNDLE_NAME.dmg $PACKAGES_PATH/$BUNDLE_NAME-$VERSION.dmg
 }

while getopts "j:b:p:sh" option
do
#echo "(-- option:$option  $OPTIND - '$OPTARG' --)"
    case $option in
        h) showHelp
            exit 0
        ;;
        j) MAKE_JOBS=$OPTARG;
        ;;
        b) BUNDLE_NAME=$OPTARG;
        ;;
        p) EXTRA_PLUGINS=$OPTARG;
        ;;
        s) buildFromSourcePackage
        ;;
    esac
done

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
echo "      * from source path $SOURCES_PATH"
echo

buildTranslations

# build all projects
TMP=$BUNDLE_NAME
for i in $TMP; do
    BUNDLE_NAME=$i
    PROJECT=`echo $i | tr '[A-Z]' '[a-z]'`;
    PROJECT_FILE=$SOURCES_PATH$PROJECT/$PROJECT.pro;
    echo "      * starting project: $PROJECT_FILE"
    buildApp
    linkQtLibs
    linkMySqlLib
    linkOpenCVLib
    createDmg
done

# move dmg files to script path
cp $PACKAGES_PATH/*.dmg $SCRIPT_PATH

exit 0
