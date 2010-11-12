#!/bin/sh
#-------------------------------------------------------------------------------
THIS_NAME=$0
APP_NAME=""
BUNDLE_PATH=""
FILES_PATH=""
DMG_SIZE=200

echoHelp()
{
  echo $THISNAME" creates a dmg file for mac deployment."
  echo "Usage: "
  echo $THIS_NAME" <options> -a "App Name for the bundle" -p /path/to/the/bundle/ -s sizeinMoOfDmg -f /path/to/standard/files"
  echo $THIS_NAME" -h shows this message"
}

createDmg()
{
  FULL_BUNDLE_NAME=$BUNDLE_PATH$APP_NAME".app"
  TMP="./tmp"
  DMG_FILE=$APP_NAME".dmg"

  echo "    *** DMG Release : Creating temporary package"
  rm -rf $TMP
  mkdir -p $TMP

  #  echo "    *** DMG Release : Copying $FULL_BUNDLE_NAME to $TMP..."
  cp -R $FULL_BUNDLE_NAME $TMP
  ln -s /Applications $TMP"/Applications"

  #  echo "    *** DMG Release : Copying icons to $TMP..."
  cp $FILES_PATH"/global_resources/pixmap/svg/"$APP_NAME".icns" $TMP"/.VolumeIcon.icns"
  LOWERED_APPNAME=`echo $APP_NAME | tr '[A-Z]' '[a-z]'`
  cp $FILES_PATH"/global_resources/pixmap/svg/"$LOWERED_APPNAME".icns" $TMP"/.VolumeIcon.icns"

  #  echo "    *** DMG Release : Copying folder params to $TMP..."
  tar zxvf $FILES_PATH"global_resources/package_helpers/"$LOWERED_APPNAME"_dmg.tgz" -C $TMP

#  echo "    *** DMG Release : Copying standard files..."
#  cp $FILES_PATH"/COPYING" $TMP
#  cp $FILES_PATH"/INSTALL" $TMP
#  cp $FILES_PATH"/README" $TMP

  echo "    *** DMG Release : Cleaning dmg for undesired files..."
  find "${TMP}" | egrep "CVS" | xargs rm -rf
  find "${TMP}" | egrep ".svn" | xargs rm -rf

  echo "    *** DMG Release : Cleaning dmg and adding icon"
  SetFile -c icnC $TMP/.VolumeIcon.icns
  hdiutil create -srcfolder $TMP -volname $APP_NAME -format UDRW -ov raw-$DMG_FILE
  rm -rf $TMP
  mkdir -p $TMP
  hdiutil attach raw-$DMG_FILE -mountpoint $TMP
  SetFile -a C $TMP
  hdiutil detach $TMP
  rm -rf $TMP
  rm -f $DMG_FILE
  hdiutil convert raw-$DMG_FILE -format UDZO -o $DMG_FILE
  rm -f raw-$DMG_FILE

#  echo "    *** DMG Release : Creating $DMG_SIZE Mo temporary dmg into this directory..."
#  hdiutil create -megabytes $DMG_SIZE tmp.dmg -layout NONE -fs HFS+ -volname $APP_NAME -ov
#  tmp=`hdid tmp.dmg`
#  disk=`echo $tmp | cut -f 1 -d\ `

#  echo "    *** DMG Release : Copying $FULL_BUNDLE_NAME to Volumes/$APP_NAME..."
#  cp -R $FULL_BUNDLE_NAME "/Volumes/"$APP_NAME

#  echo "    *** DMG Release : Copying standard files..."
#  ln -s /Applications "/Volumes/"$APP_NAME"/Applications"
#  cp $FILES_PATH"/global_resources/pixmap/svg/"$APP_NAME".icns" "/Volumes/"$APP_NAME"/.VolumeIcon.icns"
#  LOWERED_APPNAME=$APPNAME | tr '[A-Z]' '[a-z]'
#  cp $FILES_PATH"/global_resources/pixmap/svg/"$LOWERED_APPNAME".icns" "/Volumes/"$APP_NAME"/.VolumeIcon.icns"
#  cp ../GPL-2 "/Volumes/"$APP_NAME
#  cp ../GPL-3 "/Volumes/"$APP_NAME
#  cp ../LGPL-3 "/Volumes/"$APP_NAME
#  cp ../readme.txt "/Volumes/"$APP_NAME


  echo "    *** DMG Release : Creating dmg file..."
#  hdiutil eject "$disk"
#  hdiutil convert -format UDZO tmp.dmg -o $APP_NAME.dmg
#  rm tmp.dmg
}


# analyse command line parameters
while getopts ":a:p:s:f:h" option
do
        case $option in
                a) APP_NAME=$OPTARG
                ;;
		p) BUNDLE_PATH=$OPTARG
		;;
		s) DMG_SIZE=$OPTARG
		;;
                f) FILES_PATH=$OPTARG
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
