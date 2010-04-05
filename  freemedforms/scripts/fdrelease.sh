#!/bin/sh

# This script is part of FreeMedForms project : http://code.google.com/p/freemedforms
# (c) 2008 - 2009  Eric MAEKER, MD
#
# This script will prepare FreeDiams for internet release.
# This script path home = freemedforms/scripts
# FreeDiams path = freemedforms/freediams
# 


#####################################
# 0. retreive script path and name  #
#####################################
QMAKE_BIN=""
ARCH_TO_BUILD=""

SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

MAC_SCRIPTS_PATH=$SCRIPT_PATH
DI_PATH=$SCRIPT_PATH"../"
DIPROJECT=$DI_PATH"freediams.pro"

PACKAGES_PATH=$SCRIPT_PATH"../packages"

showHelp()
{
  echo $SCRIPT_NAME" is design for freediams releases."
  echo "Usage : $SCRIPT_NAME <options>"
  echo "Options :"
  echo "          -w  build mingw win32 binary"
  echo "          -c  build mingw crosscompilation win32 binary"
  echo "          -m  build macosx binary"
  echo "          -l  build linux binary"
  echo "          -h  show this help"
  echo "Win32 port can be build under Linux using crosscompilation"
}


while getopts ":mwlh" option
do
        case $option in
                m) ARCH_TO_BUILD="mac"; QMAKE_BIN="qmake -r -config release -Wnone"
                ;;
                w) ARCH_TO_BUILD="win32"; QMAKE_BIN="qmake-qt4 -r -config release -Wnone"
                ;;
                c) ARCH_TO_BUILD="win32"; QMAKE_BIN="qmake-qt4 -r \"CONFIG+=release crosscompil\" -Wnone"
                ;;
                l) ARCH_TO_BUILD="nux"; QMAKE_BIN="qmake-qt4 -r -config release -Wnone"
		;;
		h) showHelp
		;;
        esac
done

if [ -z "$ARCH_TO_BUILD" ] ; then
    echo "Error: you must specify building architecture (-w -m -c and/or -l options)"
    exit
fi

if [ ! -e $PACKAGES_PATH ] ; then
   mkdir $PACKAGES_PATH
fi


#####################################
# 1. make clean                     #
#####################################
echo "*** Cleaning build path..."
#cd "$DI_PATH"
#$QMAKE_BIN $QMAKE && make clean
#cd ../build
#rm -R ./.obj
#rm -R ./.moc
#rm -R ./.ui
#rm -R ./.rcc
#cd "$SCRIPT_PATH"

#####################################
# 2. build mac drugsinteractions    #
#####################################
QMAKE_SPEC=""

if [ $ARCH_TO_BUILD = "mac" ] ; then
   # build app
   echo "*** Build FreeDiams for MacOsX..."
   cd "$DI_PATH"
   QMAKE_SPEC="-spec macx-g++"
 #  $QMAKE_BIN $QMAKE_SPEC $DIPROJECT && make clean
   $QMAKE_BIN $QMAKE_SPEC $DIPROJECT && make && make install

   # link frameworks
   echo "*** Linking Frameworks..."
   cd $PACKAGES_PATH/mac/FreeDiams/
   "$MAC_SCRIPTS_PATH"/macDeploy.sh -a FreeDiams -p imageformats -p sqldrivers -p accessible

   # clean old dmg and create new one
   echo "*** Creating DMG archive for MacOsX..."
   rm *.dmg
   "$MAC_SCRIPTS_PATH"/release_dmg.sh -a FreeDiams -p ./ -s 100
   # move archive to right place
   mv FreeDiams.dmg "$PACKAGES_PATH"

   # nothing to do next so exit
   exit
fi

#####################################
# 3. build win drugsinteractions    #
#####################################
if [ $ARCH_TO_BUILD = "win32" ] ; then
   # build app
   echo "*** Cross-compiling DrugsInteractions for Win32 platform..."
   cd "$DI_PATH"
   QMAKE_SPEC="-win32 -spec win32-x-g++"
   $QMAKE_BIN $QMAKE_SPEC $DIPROJECT && make clean
   $QMAKE_BIN $QMAKE_SPEC $DIPROJECT && make && make install

   # zip Win32 dir
   cd $PACKAGES_PATH
   zip -r DrugsInteractions_Win32.zip win/drugsinteractions
   # cleaning tmp dir
   rm -R Win32
   # move archive to right place
   mv DrugsInteractions_Win32.zip "$PACKAGES_PATH"

   # send to google
   echo "*** Sending archive to Google..."
   cd "$SCRIPT_PATH"
   if [ -e googlecode_upload.py ] ; then
      rm googlecode_upload.py
   fi
   wget http://support.googlecode.com/svn/trunk/scripts/googlecode_upload.py

   echo "--> into pakages dir, type : ../scripts/googlecode_upload.py -s \"Latest DrugsInteractions - Win32 release\" -p freemedforms -u eric.maeker -l \"OpSys-Windows Type-Executable\" --config-dir=none DrugsInteractions_Win32.zip" 

   # do not exit and test if linux building is asked
fi

#####################################
# 4. build nux drugsinteractions    #
#####################################
if [ $ARCH_TO_BUILD = "nux" ] ; then
   cd "$DI_PATH"
   QMAKE_SPEC=""
   $QMAKE_BIN $QMAKE_SPEC $DIPROJECT && make clean
   $QMAKE_BIN $QMAKE_SPEC $DIPROJECT && make && make install

   # prepare dir for zipping
   cd bin
   mkdir Linux
   # add app
   cp drugsinteractions Linux
   cp *.db Linux

   # zip Linux dir
   zip -r DrugsInteractions_Linux.zip Linux
   # cleaning tmp dir
   rm -R Linux
   # move archive to right place
   mv DrugsInteractions_Linux.zip "$PACKAGES_PATH"

   # send to google
   echo "*** Sending archive to Google..."
   cd "$SCRIPT_PATH"
   if [ -e googlecode_upload.py ] ; then
      rm googlecode_upload.py
   fi
   wget http://support.googlecode.com/svn/trunk/scripts/googlecode_upload.py

   echo "--> into pakages dir, type : ../scripts/googlecode_upload.py -s \"Latest DrugsInteractions - Linux release\" -p freemedforms -u eric.maeker -l \"OpSys-Linux Type-Executable\" --config-dir=none DrugsInteractions_Linux.zip" 

   # everything is done --> can exit

fi


exit
