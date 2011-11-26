#! /bin/sh

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2011  Eric MAEKER, MD
#
# This script helps on the compilation of the Linux projects
#


SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi
TEST=""

showHelp()
{
echo $SCRIPT_NAME" builds the *debug* Linux versions of the FreeMedForms applications project."
echo "Usage : $SCRIPT_NAME -b applicationlowcase -t"
echo
echo "Options :"
echo "          -b  Application name (freemedforms, freediams, freeaccount, freeicd, freetoolbox...)"
echo "          -t  Create translations"
echo "          -x  Create a test project (in the test path)"
echo "          -h  Show this help"
echo
}

createTranslations()
{
  release global_resources/translations/*.ts
}

# $1 = lowered case bundle name
doCompilation() 
{
  echo "*** Building application : $1 ***"
  if [ ! -e $TEST ]; then
    echo "      Building a test application"
    cd ./tests/$1/
  fi
  qmake $1.pro -r -config debug LOWERED_APPNAME=$1
  make
  echo "On LINUX: Start application with: ./bin/"$1"/"$1"_debug --config=../global_resources/"$1"_config.ini"
  echo "On MACOS: Start application with: ./bin/"$1"/"$1"_debug.app/Contents/MacOs/"$1"_debug --config=../global_resources/"$1"_config.ini"
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hxtb:" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    x) TEST="y"
    ;;
    t) createTranslations
    ;;
    b) doCompilation $OPTARG
  esac
done

