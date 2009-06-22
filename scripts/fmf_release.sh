#!/bin/sh

# This script is part of FreeMedForms project : http://code.google.com/p/freemedforms
# (c) 2008 - 2009  Eric MAEKER, MD
#
# This script will prepare FreeMedForms for internet release.
# This script path home = freemedforms/scripts
# 


#####################################
# 0. retreive script path and name  #
#####################################
QMAKE="-recursive -Wnone -config release"
QMAKE_BIN=""
ARCH_TO_BUILD=""
VERSION="alpha"
#clean project before all make
DONT_CLEAN=0

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

MAC_SCRIPTS_PATH=$SCRIPT_PATH
ROOT_PATH=$SCRIPT_PATH"../"
PACKAGES_PATH=$SCRIPT_PATH"../packages"

showHelp()
{
  echo $SCRIPT_NAME" is design for FreeMedForms releases."
  echo "Usage : $SCRIPT_NAME <options>"
  echo "Options :"
  echo "          -n  do not clean before buildind (default is clean before make)"
  echo "          -w  build win32 binary"
  echo "          -m  build macosx binary"
  echo "          -l  build linux binary"
  echo "          -v <version> version number to build"
  echo "          -h  show this help"
  echo "Win32 port can be build under Linux using crosscompilation"
}


while getopts ":mwlhnv:" option
do
        case $option in
                m) ARCH_TO_BUILD="mac"; QMAKE_BIN="qmake"
                ;;
		w) ARCH_TO_BUILD="win32"; QMAKE_BIN="qmake-qt4"
		;;
		l) ARCH_TO_BUILD="nux"; QMAKE_BIN="qmake-qt4"
		;;
		h) showHelp
		;;
                v) VERSION=$OPTARG
                ;;
                n) DONT_CLEAN=1
                ;;

        esac
done

if [ -z "$ARCH_TO_BUILD" ] ; then
    echo "Error: you must specify building architecture (-w -m and/or -l options)"
    exit
fi

if [ ! -e "$PACKAGES_PATH" ] ; then
   mkdir "$PACKAGES_PATH"
fi


#####################################
# Resources Installer               #
#####################################
resourcesInstaller()
{
   FROM="$1"
   TO="$2"
   if [ ! -e "$TO" ]; then
      mkdir "$TO"
   else
      rm -R "$TO"
      mkdir "$TO"
   fi

   if [ ! -e "$TO/databases" ]; then
       mkdir "$TO/databases"
       echo "      Databases resources created"
   fi
   cp -R "$FROM/databases" "$TO"
   rm "$TO"/databases/*.zip 

   if [ ! -e "$TO/translations" ]; then
       mkdir "$TO/translations"
       echo "      Translations resources created"
   fi
   cp "$FROM"/translations/*.qm "$TO/translations"

   if [ ! -e "$TO/forms" ]; then
       mkdir "$TO/forms"
       echo "      Forms resources created"
   fi
   cp "$FROM"/forms/*.xml "$TO/forms"

   if [ ! -e "$TO/tmp" ]; then
       mkdir "$TO/tmp"
   fi

   echo "*** Cleaning copied resources..."
   target=$TO
   find "${target}" | egrep "CVS" | xargs rm -rf
   find "${target}" | egrep ".svn" | xargs rm -rf
}


#####################################
# 1. make clean                     #
#####################################
clean() {
  echo "*** Cleaning build path..."
  cd "$ROOT_PATH"
  if [ -e ./build/release/.obj ] ; then
     rm -R ./build/release/.obj
  fi

  if [ -e ./build/release/.moc ] ; then
     rm -R ./build/release/.moc
  fi

  if [ -e ./build/release/.ui ] ; then
     rm -R ./build/release/.ui
  fi

  if [ -e ./build/release/.rcc ] ; then
     rm -R ./build/release/.rcc
  fi
  cd "$SCRIPT_PATH"
}

#####################################
# 2. build mac freemedforms         #
#####################################
QMAKE_SPEC=""

if [ $ARCH_TO_BUILD = "mac" ] ; then
   # build app
   echo "*** Build FreeMedForms $VERSION for MacOsX..."
   cd "$ROOT_PATH"
   QMAKE_SPEC=" -spec macx-g++ "
   if [ $DONT_CLEAN = 0 ]; then
     clean
     cd "$ROOT_PATH"
     echo `$QMAKE_BIN $QMAKE $QMAKE_SPEC`> log.txt
     echo `make clean` > log.txt
   fi
   echo `$QMAKE_BIN $QMAKE $QMAKE_SPEC`> log.txt
   echo `make` > log.txt

   # add all resources to bundle
   echo "*** Installing FreeMedForms resources into the bundle..."
   RESOURCES_PATH="bin/FreeMedForms.app/Contents/Resources"
   resourcesInstaller ./bin "$RESOURCES_PATH"

   # remove config.ini
   find ./bin/FreeMedForms.app/ -type f -name 'config.ini' -exec rm {} \;
   #rm "$ROOT_PATH"/bin/FreeMedForms.app/Contents/Resources/config.ini

   # link frameworks
   echo "*** Linking Frameworks..."
   cd bin
   "$MAC_SCRIPTS_PATH"/macDeploy.sh -a FreeMedForms -p sqldrivers

   # clean old dmg and create new one
   echo "*** Creating DMG archive for MacOsX..."
   find ./ -type f -name '*.dmg' -exec rm {} \;
   "$MAC_SCRIPTS_PATH"/release_dmg.sh -a FreeMedForms -p ./ -s 100
   # move archive to right place
   mv FreeMedForms.dmg "$PACKAGES_PATH"/FreeMedForms-$VERSION.dmg

   # nothing to do next so exit
   exit
fi

#####################################
# 3. build win freemedforms         #
#####################################
if [ $ARCH_TO_BUILD = "win32" ] ; then
   # build app
   echo "*** Cross-compiling FreeMedForms $VERSION for Win32 platform..."
   cd "$ROOT_PATH"
   QMAKE_SPEC="-win32 -spec win32-x-g++"
   if [ $DONT_CLEAN = 0 ]; then
     clean
     cd "$ROOT_PATH"
     echo `$QMAKE_BIN $QMAKE $QMAKE_SPEC`> log.txt
     echo `make clean` > log.txt
   fi
   echo `$QMAKE_BIN $QMAKE $QMAKE_SPEC`> log.txt
   echo `make`> log.txt

   # prepare dir for zipping
   cd bin
   TMP_DIR="FreeMedForms_Win32-$VERSION"
   mkdir $TMP_DIR

   # add all resources to bundle
   echo "*** Installing FreeMedForms resources into the bundle..."
   RESOURCES_PATH=$TMP_DIR
   resourcesInstaller ./ "$RESOURCES_PATH"

   # add plugins to bundle
   if [ ! -e "$TMP_DIR/plugins" ]; then
       mkdir "$TMP_DIR/plugins"
       echo "      Plugins resources created"
   fi
   cp ./plugins/*.dll "$TMP_DIR/plugins"

   # add app
   echo "*** Preparing FreeMedForms executable and Qt libs..."
   cp freemedforms.exe $TMP_DIR
   # add Qt dlls (get them from wine)
   QT_PATH="/home/eric/.wine/drive_c/Development/Qt/4.4.3/bin"
   cp $QT_PATH/QtCore4.dll $TMP_DIR
   cp $QT_PATH/QtGui4.dll $TMP_DIR
   cp $QT_PATH/QtNetwork4.dll $TMP_DIR
   cp $QT_PATH/QtScript4.dll $TMP_DIR
   cp $QT_PATH/QtSql4.dll $TMP_DIR
   cp $QT_PATH/QtSvg4.dll $TMP_DIR
   cp $QT_PATH/QtXml4.dll $TMP_DIR
   # add mingw dll
   cp ~/.wine/drive_c/windows/system32/mingwm10.dll $TMP_DIR
   # add Qt plugins
   cp -R $QT_PATH/../plugins/sqldrivers $TMP_DIR

   find ./ -type f -name 'config.ini' -exec rm {} \;

   # zip Win32 dir
   zip -r $TMP_DIR.zip $TMP_DIR
   # cleaning tmp dir
   rm -R $TMP_DIR
   # move archive to right place
   mv $TMP_DIR.zip "$PACKAGES_PATH"

   # do not exit and test if linux building is asked
fi

#####################################
# 4. build nux freemedforms         #
#####################################
if [ $ARCH_TO_BUILD = "nux" ] ; then
   echo "*** Build FreeMedForms $VERSION for Linux..."
   cd "$ROOT_PATH"
   QMAKE_SPEC=""
   if [ $DONT_CLEAN = 0 ]; then
     clean
     cd "$ROOT_PATH"
     echo `$QMAKE_BIN $QMAKE $QMAKE_SPEC`> log.txt
     echo `make clean` > log.txt
   fi
   echo `$QMAKE_BIN $QMAKE $QMAKE_SPEC`> log.txt
   echo `make`> log.txt

   # prepare dir for zipping
   cd bin
   TMP_DIR="FreeMedForms_Linux-$VERSION"
   mkdir ./$TMP_DIR

   # add all resources to bundle
   echo "*** Installing FreeMedForms resources into the bundle..."
   RESOURCES_PATH=$TMP_DIR
   resourcesInstaller ./ $TMP_DIR

   # add plugins to bundle
   if [ ! -e "$TMP_DIR/plugins" ]; then
       mkdir "$TMP_DIR/plugins"
       echo "      Plugins resources created"
   fi
   cp ./plugins/*.so "$TMP_DIR/plugins"

   # add app
   cp ./freemedforms ./$TMP_DIR/freemedforms

   # zip Linux dir
   zip -r $TMP_DIR.zip $TMP_DIR
   # cleaning tmp dir
   rm -R $TMP_DIR
   # move archive to right place
   mv $TMP_DIR.zip "$PACKAGES_PATH"

   # everything is done --> can exit
fi


exit
