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

# define colors for highlighting
START_COLOR="\033[1;37m"
END_COLOR="\033[0m"

APP_NAME=""
BUNDLE_PATH=""
FILES_PATH=""
VERSION="No_Version"
DMG_SIZE=200

# Get scripts names and paths
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

echoHelp()
{
  echo $SCRIPT_NAME" creates a dmg file for mac deployment."
  echo "Usage: "
  echo $SCRIPT_NAME" <options> -a \"AppName\" -p /path/to/the/bundle -s 100 -f /path/to/standard/files"
  echo "    -p ...  path to the application bundle to be stored in the DMG"
  echo "    -s ...  size of the DMG (Mo)"
  echo "    -f ...  path to source files"
  echo "    -v ...  packaging version"
  echo "    -h      shows this message"
}

createDmg()
{
  FULL_BUNDLE_NAME=$BUNDLE_PATH"/"$APP_NAME".app"
  TMP=$SCRIPT_PATH"/tmp"
  DMG_FILE=$APP_NAME".dmg"

  echo $START_COLOR"*** DMG Release : Creating temporary package: "$END_COLOR
  echo "    $TMP"
  if [ -e $TMP ]; then
      echo "$TMP exist";
      rm -rf $TMP
  fi
  mkdir -p $TMP

  echo $START_COLOR"*** DMG Release : Copying $FULL_BUNDLE_NAME"
  cp -aR $FULL_BUNDLE_NAME $TMP
  ln -s /Applications $TMP"/Applications"

  echo $START_COLOR"*** DMG Release : Copying icon"$END_COLOR
  cp $FILES_PATH"/global_resources/pixmap/svg/"$APP_NAME".icns" $TMP"/.VolumeIcon.icns"
  LOWERED_APPNAME=`echo $APP_NAME | tr '[A-Z]' '[a-z]'`
  cp $FILES_PATH"/global_resources/pixmap/svg/"$LOWERED_APPNAME".icns" $TMP"/.VolumeIcon.icns"

  echo $START_COLOR"*** DMG Release : Copying folder params"$END_COLOR
  tar zxvf $FILES_PATH"/global_resources/package_helpers/"$LOWERED_APPNAME"_dmg.tgz" -C $TMP

  echo $START_COLOR"*** DMG Release : Copying standard files"$END_COLOR
  cp $FILES_PATH"/COPYING.txt" $TMP
  cp $FILES_PATH"/README.txt" $TMP
  touch $TMP"/"$VERSION

  echo $START_COLOR"*** DMG Release : Cleaning undesired files"$END_COLOR
  find "${TMP}" | egrep "CVS" | xargs rm -rf
  find "${TMP}" | egrep ".svn" | xargs rm -rf
  find "${TMP}" | egrep ".git" | xargs rm -rf

  echo $START_COLOR"*** DMG Release : Creating DMG file"$END_COLOR
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

#  echo "*** DMG Release : Creating $DMG_SIZE Mo temporary dmg into this directory..."
#  hdiutil create -megabytes $DMG_SIZE tmp.dmg -layout NONE -fs HFS+ -volname $APP_NAME -ov
#  tmp=`hdid tmp.dmg`
#  disk=`echo $tmp | cut -f 1 -d\ `

#  echo "*** DMG Release : Copying $FULL_BUNDLE_NAME to Volumes/$APP_NAME..."
#  cp -R $FULL_BUNDLE_NAME "/Volumes/"$APP_NAME

#  echo "*** DMG Release : Copying standard files..."
#  ln -s /Applications "/Volumes/"$APP_NAME"/Applications"
#  cp $FILES_PATH"/global_resources/pixmap/svg/"$APP_NAME".icns" "/Volumes/"$APP_NAME"/.VolumeIcon.icns"
#  LOWERED_APPNAME=$APPNAME | tr '[A-Z]' '[a-z]'
#  cp $FILES_PATH"/global_resources/pixmap/svg/"$LOWERED_APPNAME".icns" "/Volumes/"$APP_NAME"/.VolumeIcon.icns"
#  cp ../GPL-2 "/Volumes/"$APP_NAME
#  cp ../GPL-3 "/Volumes/"$APP_NAME
#  cp ../LGPL-3 "/Volumes/"$APP_NAME
#  cp ../readme.txt "/Volumes/"$APP_NAME


  # echo "*** DMG Release : Creating dmg file..."
#  hdiutil eject "$disk"
#  hdiutil convert -format UDZO tmp.dmg -o $APP_NAME.dmg
#  rm tmp.dmg
}

# analyse command line parameters
while getopts ":a:p:s:f:v:h" option
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
        f) VERSION=$OPTARG
        ;;
        h) echoHelp
        ;;
    esac
done

# manage command line errors
if [ -z "$APP_NAME" ] ; then
   echo "*** DMG Release : Error: you must specify an application name with -a parameter."
   exit
fi

if [ -z "$BUNDLE_PATH" ] ; then
   echo "*** DMG Release : Error: you must specify the path to the bundle with -p parameter."
   exit
fi

# let's go for it
createDmg

exit 0
