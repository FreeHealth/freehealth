#! /bin/sh

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

# get version number of FreeDiams from the project file
PROJECT_VERSION=`cat $SCRIPT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

# get the default spec file
sys=`uname -s`
if [ "$sys" == "Linux" ] ; then
    SPEC="linux-g++"
elif [ "$sys" == "Darwin" ] ; then
    SPEC="macx-g++"
fi

showHelp()
{
echo $SCRIPT_NAME" builds the *debug* Linux versions of the FreeMedForms applications project."
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

  qmake $1.pro -r "CONFIG+=debug" "CONFIG-=release" $BUILD_SPEC LOWERED_APPNAME=$1
  make
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

