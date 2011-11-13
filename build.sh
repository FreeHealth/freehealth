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

showHelp()
{
echo $SCRIPT_NAME" builds the *debug* Linux versions of the FreeMedForms applications project."
echo "Usage : $SCRIPT_NAME -b applicationlowcase -t"
echo
echo "Options :"
echo "          -b  Application name (freemedforms, freediams, freeaccount, freeicd, freetoolbox...)"
echo "          -t  Create translations"
echo "          -h  Show this help"
echo
}

createTranslations()
{
  echo release global_resources/translations/*.ts
}

# $1 = lowered case bundle name
doCompilation() 
{
  echo "*** Building application : $1 ***"
  echo qmake $1.pro -r -config debug LOWERED_APPNAME=$1
  echo make
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "htb:" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    t) createTranslations
    ;;
    b) doCompilation $OPTARG
  esac
done

