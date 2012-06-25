#!/bin/bash

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2012  Eric MAEKER, MD
#
# This script helps on the compilation of the Linux projects
#

BUNDLE_NAME=""
PROJECT_VERSION=""
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi
TEST=""
CLEAR=""
SPEC=""
QMAKE_CONFIG="CONFIG+=debug CONFIG-=release CONFIG+=debug_without_install"

# get version number of FreeDiams from the project file
PROJECT_VERSION=`cat $SCRIPT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

if [[ $OSTYPE == linux-gnu ]]; then
  SPEC="linux-g++"
  echo "Linux OS detected; setting qmake spec to "$SPEC
elif [[ $OSTYPE == darwin* ]]; then
  SPEC="macx-g++"
  echo "Mac OS detected; setting qmake spec to "$SPEC
elif [[ $OSTYPE == freebsd ]]; then
  SPEC="linux-g++"
  echo "FreeBSD OS detected; setting qmake spec to "$SPEC
else
  echo "   * Unknow OS"
fi

showHelp()
{
echo $SCRIPT_NAME" builds the *debug_without_install* Linux versions of the FreeMedForms applications project."
echo "Project version: "$PROJECT_VERSION
echo "Usage : $SCRIPT_NAME -txch -s <qt.spec> -b <AppToBuild>"
echo
echo "Options :"
echo "  -b  Application name (freemedforms, freediams, freeaccount, freeicd, freepad, freetoolbox)"
echo "  -t  Create translations"
echo "  -x  Create a test project (in the test path)"
echo "  -c  Make clear before compiling"
echo "  -s  Use the specified spec file (default spec: $SPEC)"
echo "  -h  Show this help"
echo
}

createTranslations()
{
  lrelease global_resources/translations/*.ts
}

# $1 = lowered case bundle name
doCompilation() 
{
  echo "*** Building application : $1 ***"
  if [ ! -e $TEST ]; then
    echo "    *** Building a test application"
    cd ./tests/$1/
  fi
  if [ ! -e $CLEAR ]; then
    echo "    *** Make clear"
    make clear
    rm -R $SCRIPT_PATH/bin/$1
  fi
  BUILD_SPEC=""
  if [ ! -e $SPEC ]; then
    BUILD_SPEC="-spec "$SPEC
  fi
  echo "    ** qmake $1/$1.pro -r $QMAKE_CONFIG $BUILD_SPEC LOWERED_APPNAME=$1"

  cd $1
  qmake $1.pro -r $QMAKE_CONFIG $BUILD_SPEC LOWERED_APPNAME=$1
  make
  cd ..
  echo "On LINUX: Start application with: ./bin/"$1"/"$1"_debug --config=../global_resources/"$1"_config.ini"
  echo "On MACOS: Start application with: ./bin/"$1"/"$1"_debug.app/Contents/MacOs/"$1"_debug --config=../../../../../global_resources/"$1"_config.ini"
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hcxtb:" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    x) TEST="y"
    ;;
    c) CLEAR="y"
    ;;
    s) SPEC=$OPTARG
    ;;
    t) createTranslations
    ;;
    b) BUNDLE_NAME=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
       doCompilation $BUNDLE_NAME
  esac
done

