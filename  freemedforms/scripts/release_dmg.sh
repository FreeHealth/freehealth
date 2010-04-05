#!/bin/sh
#-------------------------------------------------------------------------------
THIS_NAME=$0
APP_NAME=""
BUNDLE_PATH=""
DMG_SIZE=200

echoHelp()
{
  echo $THISNAME" creates a dmg file for mac deployment."
  echo "Usage: "
  echo $THIS_NAME" <options> -a "App Name for the bundle" -p /path/to/the/bundle -s sizeinMoOfDmg"
  echo $THIS_NAME" -h shows this message"
}

createDmg()
{
  FULL_BUNDLE_NAME=$BUNDLE_PATH$APP_NAME".app"

  echo "    *** DMG Release : Creating $DMG_SIZE Mo temporary dmg into this directory..."
  hdiutil create -megabytes $DMG_SIZE tmp.dmg -layout NONE -fs HFS+ -volname $APP_NAME -ov
  tmp=`hdid tmp.dmg`
  disk=`echo $tmp | cut -f 1 -d\ `

  echo "    *** DMG Release : Copying $FULL_BUNDLE_NAME to Volumes/$APP_NAME..."
  cp -R $FULL_BUNDLE_NAME "/Volumes/"$APP_NAME

#  echo "    *** DMG Release : Copying standard files..."
#  cp ../dev-readme "/Volumes/"$APP_NAME
#  cp ../GPL-2 "/Volumes/"$APP_NAME
#  cp ../GPL-3 "/Volumes/"$APP_NAME
#  cp ../LGPL-3 "/Volumes/"$APP_NAME
#  cp ../readme.txt "/Volumes/"$APP_NAME

  echo "    *** DMG Release : Cleaning dmg for undesired files..."
  target="/Volumes/"$APP_NAME
  find "${target}" | egrep "CVS" | xargs rm -rf
  find "${target}" | egrep ".svn" | xargs rm -rf

  echo "    *** DMG Release : Creating dmg file..."
  hdiutil eject "$disk"
  hdiutil convert -format UDZO tmp.dmg -o $APP_NAME.dmg
  rm tmp.dmg  
}


# analyse command line parameters
while getopts ":a:p:s:h" option
do
        case $option in
                a) APP_NAME=$OPTARG
                ;;
		p) BUNDLE_PATH=$OPTARG
		;;
		s) DMG_SIZE=$OPTARG
		;;
		h) echoHelp
		;;
#                :) echo "*** option \"$OPTARG\" sans arg"
#                ;;
#                \?) echo "*** option \"$OPTARG\" inconnue !!!"
#                ;;
#                *) echo "*** user \"$OPTARG\" !!!"
#                ;;
        esac
done

# manage command line errors
if [ -z "$APP_NAME" ] ; then
   echo "    *** DMG Release : Error: you must specify an application name with -a parameter."
   exit
fi

if [ -z "$BUNDLE_PATH" ] ; then
   echo "    *** DMG Release : Error: you must specify the path to the bundle with -p parameter."
   exit
fi

# let's go for it
createDmg

exit 0
