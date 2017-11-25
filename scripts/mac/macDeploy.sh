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

# TODO: do not process symlinks with otools

# define colors for highlighting
START_COLOR="\033[1;37m"
END_COLOR="\033[0m"

# define some Qt vars
QT_LIBS_PATH=`qmake -query QT_INSTALL_LIBS`
QT_PLUGINS_PATH=`qmake -query QT_INSTALL_PLUGINS`
QT_MAIN_VERSION=`qmake -query QT_VERSION`
QT_MAIN_VERSION=`echo $QT_MAIN_VERSION | cut -f 1 -d.`

APP_NAME=""
PLUGS=""

if [ ! -d $QT_LIBS_PATH/QtCore.framework ] ; then
    echo "ERROR: cannot find the Qt frameworks. Make sure Qt is installed"
    echo "and qmake is in your environment path."
    exit
fi

# Get scripts names and paths
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

echoVersion()
{
    echo $SCRIPT_NAME" version "$VERSION
    echo "    Qt libs detected: "$QT_LIBS_PATH
    echo "    Qt main version detected: "$QT_MAIN_VERSION
    echo "    Qt full version number: "`qmake -query QT_VERSION`
    exit 0;
}

echoHelp()
{
  echo $SCRIPT_NAME" runs Mac deployment commands on a specified FreeMedForms project bundle."
  echo "The script must run in the path of the bundle."
  echo "Usage: "
  echo "    "$SCRIPT_NAME" -a \"AppName\" -p \"plugin\" -p \"to-install\""
  echo "        -p ...  Qt plugins to install inside the bundle"
  echo "        -v ...  show version of the script"
  echo "        -h      shows this message"
  exit 0;
}

while getopts ":a:p:hv" option
do
    case $option in
        a) APP_NAME=$OPTARG
        ;;
        p) PLUGS=$PLUGS"  "$OPTARG     # creating plugins to install list
        ;;
        h) echoHelp
        ;;
        v) echoVersion
        ;;
        esac
done

# No APP_NAME -> error
if [ -z "$APP_NAME" ] ; then
    echo "Error: No application name specified"
    echo "For more help, please read $SCRIPT_NAME -h"
    exit 1;
fi

# PLUGS empty --> all plugins to install
if [ -z "$PLUGS" ] ; then
    PLUGS="all"
fi

#### bundle structure ###################################################
BUNDLE="$APP_NAME.app"
APP_BIN="$BUNDLE/Contents/MacOS/`basename \"${APP_NAME}\"`"
APP_PLUGINS_PATH="$BUNDLE/Contents/plugins"
APP_QT_PLUGINS_PATH="$BUNDLE/Contents/plugins/qt"
APP_FRAMEWORKS_PATH="$BUNDLE/Contents/Frameworks"
APP_LIBS_PATH="$BUNDLE/Contents/libs"

if [ ! -d "${BUNDLE}" ] ; then
    echo "ERROR: cannot find application bundle \"$BUNDLE\" in current directory"
    exit 2
fi

if [ ! -x "${APP_BIN}" ] ; then
    echo "ERROR: cannot find application \"$APP_BIN\" in bundle. Did you forget to run make?"
    exit 3
fi

### functions ######################################################
copyPlugins()
{
    path="$2/$1"
    if [ ! -d "${path}" ] ; then
        echo "Copying $1 plugins..."
        mkdir -p "${path}"
        cp -R "$QT_PLUGINS_PATH/${1}" "${2}"
        rm "${path}"/*_debug.*
    fi
}

copyAllPlugins()
{
    for plug in `ls $QT_PLUGINS_PATH`; do
        copyPlugins "$plug" "$APP_QT_PLUGINS_PATH"
    done
}

changeBinaryPaths()
{
    echo "Changing framework paths for `basename "${1}"`..."
    for path in $2 ; do
        name=`basename "${path}"`
        # echo -n " $name"
        install_name_tool -change $path @executable_path/../Frameworks/$name "${1}"
    done
}

getSourceFramework()
{
    framework="$QT_LIBS_PATH/$1"

    # check for non framework, like libQtCLuScene.4.dylib
    dylib=`expr "${framework}" : '.*\(dylib\)$'`
    if [ "x${dylib}" = "x" ] ; then
        framework="$QT_LIBS_PATH/$1.framework/Versions/$QT_MAIN_VERSION/$1"
    fi

    # is symlink ?
    if [ -h "${framework}" ] ; then
        symlink=`readlink "${framework}"`
        if [ $symlink[0] = '/' ] ; then
            $framework=$symlink
        else
            framework="$QT_LIBS_PATH/$symlink"
        fi
    fi

    echo "$framework"
}

getTargetFramework()
{
    echo "$APP_FRAMEWORKS_PATH/$1"
}

getBinaryDependencies()
{
    tmp=""
    for n in `otool -LX "${1}" | grep Qt` ; do
        path=`echo $n | grep Qt`
        if [ $path ] ; then
            tmp="$tmp $path"
        fi
    done

    echo $tmp
}

setId()
{
    echo "Updating $1 id..."
    install_name_tool -id @executable_path/../Frameworks/"${1}" "${2}"
}

relinkPlugins()
{
    tmp_plugins=`find "${1}" | egrep ".dylib"`
    old_ifs="$IFS"
    IFS=$'\n'
    count=0
    for plugin in $tmp_plugins ; do
        plugins[$count]="${plugin}"
        ((count++))
    done
    IFS=$old_ifs

    count=${#plugins[@]}
    for ((i=0;i<$count;i++)) ; do
        plugin=${plugins[${i}]}
        relinkBinary "$plugin"
        echo
    done
}

relinkBinary()
{
    # echo "Striping `basename \"${1}\"` binary..."
    # strip libs (-x is max allowable for shared libs)
    #strip -x "${1}"

    # set id in target
    if [ ! "x${2}" = "x" ] ; then
        setId "${2}" "${1}"
    fi

    # get dependencies
    frameworks_path=`getBinaryDependencies "$1"`

    # update framework/library paths
    changeBinaryPaths "$1" "$frameworks_path"

    # copy dependencies frameworks/libraries
    for framework_path in $frameworks_path ; do
        # get framework
        framework=`basename "${framework_path}"`

        # get filenames
        source=`getSourceFramework "$framework"`
        target=`getTargetFramework "$framework"`

        # copy file if needed
        if [ -e "${source}" ] ; then
            if [ ! -e "${target}" ] ; then
                #echo "Copying & striping `basename \"${source}\"` framework/library..."
                path=`dirname "${target}"`
                mkdir -p "${path}"
                cp -f "${source}" "${target}"

                # echo
                relinkBinary "${target}" "${framework}"
            fi
        fi
    done
}

# In FreeMedForms project all plugins are in the 'plugins' path
# We need to inform Qt about that using a qt.conf file
createQtConfFile()
{
    cd $BUNDLE"/Contents/Resources"
    echo "[Paths]" > qt.conf
    echo "Plugins=plugins/qt" >> qt.conf
}

echo $START_COLOR"MacDeploy: Starting deployment"$END_COLOR
echo "    * Application: ${APP_NAME}"
echo "    * Bundle:      ${BUNDLE}"
echo "    * Libs path:   ${APP_LIBS_PATH}"
echo "    * Qt libs: "$QT_LIBS_PATH
echo "    * Qt version: "`qmake -query QT_VERSION`
echo "    * Qt main version: "$QT_MAIN_VERSION

relinkBinary "$APP_BIN"

echo $START_COLOR"MacDeploy: Copying Qt plugins: "$PLUGS$END_COLOR
if [[ $PLUGS = "all" ]]; then
     copyAllPlugins
else
    for p in $PLUGS ; do
        copyPlugins $p "$APP_QT_PLUGINS_PATH"
    done
fi

echo $START_COLOR"MacDeploy: Linking Qt plugins: "$END_COLOR
relinkPlugins "$APP_PLUGINS_PATH"

echo $START_COLOR"MacDeploy: Linking application libs"$END_COLOR
relinkPlugins "$APP_LIBS_PATH"

createQtConfFile

#echo "Striping `basename \"${APP_BIN}\"` binary..."
#strip "${APP_BIN}"
echo

# UNUSED
copyDependencies()
{
    echo "Copying dependencies..."
    mkdir -p "${1}"
    for path in $2 ; do
        name=`basename "${path}"`
        fullname="$QT_LIBS_PATH/$name"

        # check for non framework, like libQtCLuScene.4.dylib
        dylib=`expr "${path}" : '.*\(dylib\)$'`
        if [ "x${dylib}" = "x" ] ; then
            fullname="$QT_LIBS_PATH/$name.framework/Versions/$QT_MAIN_VERSION/$name"
        fi

        # is symlink ?
        if [ -h "${fullname}" ] ; then
            symlink=`readlink "${fullname}"`
            if [ $symlink[0] = '/' ] ; then
                $fullname=$symlink
            else
                fullname="$QT_LIBS_PATH/$symlink"
            fi
        fi

        # is file exists
        if [ ! -f "${fullname}" ] ; then
            echo "ERROR: cannot find $fullname"
            exit
        fi

        echo -n "$name framework"
        target="$1/$name"
        cp -f "${fullname}" "${target}"
        # strip libs (-x is max allowable for shared libs)
        #strip -x "${target}"
    done
}
# /UNUSED

exit 0;
