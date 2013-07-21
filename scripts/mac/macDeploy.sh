#!/bin/sh
# Copyright 2008 AZEVEDO Filipe MonkeyStudio script
# Adaptations by Eric MAEKER (command line parser) - 2008

### get system configuration ########################################
# as long as we can find qmake, we don't need QTDIR
QT_LIBS_PATH=`qmake -query QT_INSTALL_LIBS`
if [ ! -d $QT_LIBS_PATH/QtCore.framework ] ; then
	echo "ERROR: cannot find the Qt frameworks. Make sure Qt is installed"
	echo "and qmake is in your environment path."
	exit
fi

#### getting params ###################################################
# accepted command line
# -a application_name
# -p plugins_to_install             many times you want
APP_NAME=""
PLUGS=""

while getopts ":a:p:" option
do
        case $option in
                a) APP_NAME=$OPTARG
                ;;
		p) PLUGS=$PLUGS"  "$OPTARG     # creating plugins to install list
		;;
#                :) echo "*** option \"$OPTARG\" sans arg"
#                ;;
#                \?) echo "*** option \"$OPTARG\" inconnue !!!"
#                ;;
#                *) echo "*** user \"$OPTARG\" !!!"
#                ;;
        esac
done

# APP_NAME empty --> ask user
if [ -z "$APP_NAME" ] ; then
	echo
	echo "This script prepares a Qt application bundle for deployment. It will"
	echo "copy over the required Qt frameworks and sets the installation"
	echo "identifications. Please see the \"Deploying an Application on Qt/Mac\""
	echo "page in the Qt documentation for more information."
	echo
	echo "This script assumes you have already built the application bundle."
	echo
	echo -n "What is the name of the application? "
	read userinput
	APP_NAME=$userinput
fi

# PLUGS empty --> all plugins to install
if [ -z "$PLUGS" ] ; then
	echo
	echo "No Qt Plugin specified in command line."
        echo "Will install all Qt plugins".
        PLUGS="all"
fi

#### bundle structure ###################################################
BUNDLE="$APP_NAME.app"
APP_BIN="$BUNDLE/Contents/MacOS/`basename \"${APP_NAME}\"`"
APP_PLUGINS_PATH="$BUNDLE/Contents/plugins"
APP_QT_PLUGINS_PATH="$BUNDLE/Contents/plugins/qt"
APP_FRAMEWORKS_PATH="$BUNDLE/Contents/Frameworks"
APP_LIBS_PATH="$BUNDLE/Contents/libs"
QT_PLUGINS_PATH=`qmake -query QT_INSTALL_PLUGINS`

if [ ! -d "${BUNDLE}" ] ; then
	echo "ERROR: cannot find application bundle \"$BUNDLE\" in current directory"
	exit
fi

if [ ! -x "${APP_BIN}" ] ; then
	echo "ERROR: cannot find application \"$APP_BIN\" in bundle. Did you forget to run make?"
	exit
fi

echo "application: ${APP_NAME}"
echo "bundle:      ${BUNDLE}"
echo "libs path:   ${APP_LIBS_PATH}"
echo

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
	copyPlugins "accessible" "$APP_QT_PLUGINS_PATH"
	copyPlugins "codecs" "$APP_QT_PLUGINS_PATH"
        copyPlugins "designer" "$APP_QT_PLUGINS_PATH"
        copyPlugins "graphicssystems" "$APP_QT_PLUGINS_PATH"
	copyPlugins "iconengines" "$APP_QT_PLUGINS_PATH"
	copyPlugins "imageformats" "$APP_QT_PLUGINS_PATH"
#	copyPlugins "inputmethods" "$APP_QT_PLUGINS_PATH"
	copyPlugins "phonon_backend" "$APP_QT_PLUGINS_PATH"
	copyPlugins "script" "$APP_QT_PLUGINS_PATH"
	copyPlugins "sqldrivers" "$APP_QT_PLUGINS_PATH"
}

changeBinaryPaths()
{
	echo "Changing framework paths for `basename "${1}"`..."
	for path in $2 ; do
		name=`basename "${path}"`
		echo -n " $name"
		install_name_tool -change $path @executable_path/../Frameworks/$name "${1}"
	done
}

getSourceFramework()
{
	framework="$QT_LIBS_PATH/$1"
		
	# check for non framework, like libQtCLuScene.4.dylib
	dylib=`expr "${framework}" : '.*\(dylib\)$'`
	if [ "x${dylib}" = "x" ] ; then
		framework="$QT_LIBS_PATH/$1.framework/Versions/4/$1"
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
        for ((i=0;i<$count;i++));
        do
                plugin=${plugins[${i}]}
                relinkBinary "$plugin"
                echo
        done
}

makeInstall()
{
	echo "Running make install..."
	qmake
	if [ -e Makefile.Release ] ; then
		make -f Makefile.Release && make -f Makefile.Release install
	elif [ -e Makefile ] ; then
		make && make install
	else
		echo -n "ERROR: Makefile not found. This script requires the macx-g++ makespec"
	fi
}

relinkBinary()
{
	echo "Striping `basename \"${1}\"` binary..."
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
				
				echo
				relinkBinary "${target}" "${framework}"
			fi
		fi
	done
}

### deployement #################################################
#makeInstall "$APP_BIN"
#echo

relinkBinary "$APP_BIN"
echo

# manage Qt plugins to add to the bundle
if [[ $PLUGS = "all" ]]; then
     copyAllPlugins
else
  for p in $PLUGS
   do
     copyPlugins $p "$APP_QT_PLUGINS_PATH"
   done
fi

echo


echo

relinkPlugins "$APP_PLUGINS_PATH"
echo

relinkPlugins "$APP_LIBS_PATH"
echo

#echo "Striping `basename \"${APP_BIN}\"` binary..."
#strip "${APP_BIN}"
echo

### misc cleanup ##################################################
target="$BUNDLE/Contents"
find "${target}" | egrep "CVS" | xargs rm -rf
find "${target}" | egrep ".svn" | xargs rm -rf

### create disk image ###############################################
#echo "Creating disk image"
#imagedir="/tmp/$APP_NAME.$$"
#mkdir $imagedir
#cp -R $BUNDLE $imagedir

# TODO: copy over additional files, if any
#hdiutil create -ov -srcfolder $imagedir -format UDBZ -volname "$APP_NAME" "$APP_NAME.dmg"
#hdiutil internet-enable -yes "$APP_NAME.dmg"
#rm -rf $imagedir

echo "Done"

#### functions ###################################################

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
			fullname="$QT_LIBS_PATH/$name.framework/Versions/4/$name"
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

