#!/bin/bash
# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2012  Eric MAEKER, MD
#
# This script helps on the compilation on unices machines
#

SCRIPT_VERSION=1.0

DEBUG_BUILD_COMMANDS=0  # set to 1 to only debug commands (no commands are executed)

BUNDLE_NAME=""
PROJECT_VERSION=""
TEST=""
TRANS=""
CLEAN=""
SPEC=""
BUILD="debug"
QMAKE_CONFIG=""
MAKE_OPTS="-j4"
RUN=""
ZENITY="zenity"
SHOW_ZENITY_PROGRESS="y"  # y / n
LOG_FILE=""
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

# get version number of FreeDiams from the project file
PROJECT_VERSION=`cat $SCRIPT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

showHelp()
{
    echo $SCRIPT_NAME" builds the *debug_without_install* Linux versions of the FreeMedForms applications project."
    echo "Project version: $PROJECT_VERSION - $SCRIPT_VERSION"
    echo "Usage : $SCRIPT_NAME -txch -dri -s <qt.spec> -b <AppToBuild>"
    echo
    echo "Options :"
    echo "  -b  Application name (freemedforms, freediams, freeaccount, freeicd, freepad, freetoolbox)"
    echo "  -d  Build a debug 'no-install' version (default)"
    echo "  -r  Build a release version and install it:"
    echo "        1. Clean build path"
    echo "        2. Create translations"
    echo "        3. Compile application"
    echo "        4. Install application"
    echo "  -i  with 'linux integrated' configuration (release mode only) same as -ri"
    echo "  -j  Parallel compilation (number of jobs: equivalent to make -jX)"
    echo "  -t  Create translations"
    echo "  -x  Create a test project (in the test path)"
    echo "  -c  Make clean before compiling"
    echo "  -s  Use the specified spec file (default spec: $SPEC)"
    echo "  -R  Run application after the build"
    echo "  -h  Show this help"
    echo
}

#########################################################################################
## Build commands
#########################################################################################
createLogFile()
{
    LOG_FILE="./build_`date "+%s"`.log"
    echo "* Creating build log: $LOG_FILE"
    if [[ -f $LOG_FILE ]]; then
         rm $LOG_FILE
    fi
    touch $LOG_FILE
    echo "Log file created on: `date "+%Y.%m.%d %H:%M %N"`" > $LOG_FILE
}

detectSpec()
{
    echo "* Guessing spec file"
    if [[ $OSTYPE == linux-gnu ]]; then
      SPEC="linux-g++"
      echo "    Linux OS detected; setting qmake spec to "$SPEC
    elif [[ $OSTYPE == darwin* ]]; then
      SPEC="macx-g++"
      echo "    Mac OS detected; setting qmake spec to "$SPEC
    elif [[ $OSTYPE == freebsd ]]; then
      SPEC="linux-g++"
      echo "    FreeBSD OS detected; setting qmake spec to "$SPEC
    else
      echo "    Unknow OS"
    fi
    return 0
}

makeClean()
{
    # zenity progress feature
    echo "10"; sleep 1
    echo "# Cleaning build path" ; sleep 1
    if [[ "$CLEAN" != "" ]]; then
        echo "* Cleaning build path"
        echo "# make clean && \nrm -R $SCRIPT_PATH/bin/$BUNDLE_NAME &&\nrm -R $SCRIPT_PATH/build"; >> $LOG_FILE
        if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
            echo "# make clean && \nrm -R $SCRIPT_PATH/bin/$BUNDLE_NAME &&\nrm -R $SCRIPT_PATH/build"; sleep 5
        else
            MAKE_STEP=`make clean && rm -R $SCRIPT_PATH/bin/$BUNDLE_NAME && rm -R $SCRIPT_PATH/build >> $LOG_FILE`
            MAKE_STEP=$?
            if [[ ! $MAKE_STEP == 0 ]]; then
                return 123
            else
                echo "        Cleaned"
           fi
        fi
    fi
    return 0
}

createTranslations()
{
    # zenity progress feature
    echo "20"; sleep 1
    echo "# Creation translations" ; sleep 1
    if [ "$TRANS" != "" ]; then
        echo "* Creating translations"
            echo "# lrelease ./global_resources/translations/*.ts" >> $LOG_FILE
        if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
             echo "# lrelease ./global_resources/translations/*.ts" ; sleep 5
        else
            MAKE_STEP=`lrelease ./global_resources/translations/*.ts >> $LOG_FILE`
            MAKE_STEP=$?
            if [[ ! $MAKE_STEP == 0 ]]; then
                echo "* ERROR: lrelease step wrong ***"
                return 123
            else
                echo "        Translations created"
            fi
        fi
    fi
    return 0
}

qmakeCommand()
{
    # zenity progress feature
    echo "30"; sleep 1
    echo "# Preparing the build:\nthe qmake step" ; sleep 1
    echo "* qmake $BUNDLE_NAME.pro -r $QMAKE_CONFIG $SPEC LOWERED_APPNAME=$BUNDLE_NAME"
    echo "# qmake $BUNDLE_NAME.pro -r $QMAKE_CONFIG $SPEC LOWERED_APPNAME=$BUNDLE_NAME" >> $LOG_FILE
    if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
        echo "# qmake $BUNDLE_NAME.pro -r\n$QMAKE_CONFIG\n$SPEC\nLOWERED_APPNAME=$BUNDLE_NAME" ; sleep 5
    else
        MAKE_STEP=`qmake $BUNDLE_NAME.pro -r $QMAKE_CONFIG $SPEC LOWERED_APPNAME=$BUNDLE_NAME`
        MAKE_STEP=$?
        if [[ ! $MAKE_STEP == 0 ]]; then
            echo "* ERROR: qmake step wrong ***"
            return 123
        else
            echo "        qmake correctly done"
        fi
    fi
    return 0
}

makeCommand()
{
    # zenity progress feature
    echo "40"; sleep 1
    echo "# Building the application (make step)" ; sleep 1
    echo "# make $MAKE_OPTS"
    echo "# make $MAKE_OPTS" >> $LOG_FILE
    if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
        echo "# make $MAKE_OPTS"  ; sleep 5
    else
        MAKE_STEP=`make $MAKE_OPTS`
        MAKE_STEP=$?
        if [[ ! $MAKE_STEP == 0 ]]; then
            echo "* ERROR: make step wrong ***"
            return 123
        else
            echo "        make correctly done"
        fi
    fi
    return 0
}

makeInstallCommand()
{
    # zenity progress feature
    echo "70"; sleep 1
    echo "# Installing applications" ; sleep 1
    echo "# make install" >> $LOG_FILE
    if [[ "$BUILD" != "debug" ]]; then
        echo "* make install"
        if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
             echo "# make install"  ; sleep 5
        else
             MAKE_STEP=`make install`
             MAKE_STEP=$?
             if [[ ! $MAKE_STEP == 0 ]]; then
                 echo "* ERROR: make install step wrong ***"
                 return 123
             else
                 echo "        make install correctly done"
            fi
        fi
    fi
    return 0
}

finalMessage()
{
    # zenity progress feature
    echo "90"; sleep 1
    echo "# Compilation completed (with or without error)" ; sleep 1
    if [[ $OSTYPE == linux-gnu ]]; then
        echo "*** Start application with:"
        echo "./bin/"$BUNDLE_NAME"/"$BUNDLE_NAME"_debug --config=../global_resources/"$BUNDLE_NAME"_config.ini"
    elif [[ $OSTYPE == darwin* ]]; then
        echo "*** Start application with:"
        echo "./bin/"$BUNDLE_NAME"/"$BUNDLE_NAME"_debug.app/Contents/MacOs/"$BUNDLE_NAME"_debug --config=../../../../../global_resources/"$BUNDLE_NAME"_config.ini"
    elif [[ $OSTYPE == freebsd ]]; then
        echo "*** Start application with:"
        echo "./bin/"$BUNDLE_NAME"/"$BUNDLE_NAME"_debug --config=../../global_resources/"$BUNDLE_NAME"_config.ini"
    fi
    return 0
}

build()
{
    createLogFile
    echo "Starting build: application=$BUNDLE_NAME" >> $LOG_FILE
    # No bundle name -> Error
    if [[ "$BUNDLE_NAME" == "" ]]; then
        echo "*** ERROR: you must define the application with the -b param. See help -h."
        echo "*** ERROR: you must define the application with the -b param. See help -h." >> $LOG_FILE
        return 123;
    fi

    # Create the qmake configuration
    if [[  "$BUILD" == "debug" ]]; then
        QMAKE_CONFIG="CONFIG+=debug CONFIG-=release CONFIG+=debug_without_install"
    elif [[  "$BUILD" == "release" ]]; then
        QMAKE_CONFIG="CONFIG-=debug CONFIG+=release"
    elif [[  "$BUILD" == "linuxintegrated" ]]; then
        QMAKE_CONFIG="CONFIG-=debug CONFIG+=release CONFIG+=LINUX_INTEGRATED"
    fi

    # Check the spec
    if [ ! -e $SPEC ]; then
        SPEC="-spec "$SPEC
    fi

    MSG="DEBUG mode"
    if [[ "$BUILD" != "debug" ]]; then
        MSG="RELEASE mode"
    fi
    echo "*** Building application in $MSG: $BUNDLE_NAME ($BUILD) ***"
    if [[ "$BUILD" != "debug" ]]; then
        if [[ $OSTYPE == darwin* ]]; then
            # Switch to mac_release.sh
            if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
                echo "# cd scripts\n./mac_release.sh -b $BUNDLE_NAME\nreturn 0"
            else
                cd scripts
                echo "  **** -> switching to: scripts/mac_release.sh -b $BUNDLE_NAME"
                ./mac_release.sh -b $BUNDLE_NAME
                return $?
            fi
        fi
    fi

  if [[ "$TEST" != "" ]]; then
      echo "* Building a test application"
      cd ./tests
  fi

  if [[ ! -e $BUNDLE_NAME ]]; then
      echo "* ERROR: Application does not exist: $BUNDLE_NAME"
      echo "*** ERROR: Application does not exist." >> $LOG_FILE
      return 123
  fi

  makeClean
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: make clean." >> $LOG_FILE
      return 123;
  fi

  createTranslations
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: make translations." >> $LOG_FILE
      return 123;
  fi

  echo "# Entering $BUNDLE_NAME"; sleep 1
  cd $BUNDLE_NAME
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: entering $BUNDLE_NAME." >> $LOG_FILE
      return 123;
  fi

  qmakeCommand
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: qmake." >> $LOG_FILE
      return 123;
  fi

  makeCommand
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: make." >> $LOG_FILE
      return 123;
  fi

  makeInstallCommand
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: make install." >> $LOG_FILE
      return 123;
  fi

  echo "# Entering $SCRIPT_PATH"
  cd $SCRIPT_PATH
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: entering $SCRIPT_PATH." >> $LOG_FILE
      return 123;
  fi

  finalMessage
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: unable to display final message." >> $LOG_FILE
      return 123;
  fi
  return 0
}

launchApplication()
{
    if [[ "$RUN" == "y" ]]; then
        # Launch the debug application with the good config file
        if [[ "$BUILD" == "debug" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                $SCRIPT_PATH/bin/$BUNDLE_NAME/$BUNDLE_NAME_debug --config=../../global_resources/$BUNDLE_NAME_config.ini
            elif [[ $OSTYPE == darwin* ]]; then
                $SCRIPT_PATH/bin/$BUNDLE_NAME/$BUNDLE_NAME_debug.app/Contents/MacOs/$BUNDLE_NAME_debug --config=../../../../../global_resources/$BUNDLE_NAME_config.ini
            elif [[ $OSTYPE == freebsd ]]; then
                $SCRIPT_PATH/bin/$BUNDLE_NAME/$BUNDLE_NAME_debug --config=../../global_resources/$BUNDLE_NAME_config.ini
            fi
        elif [[ "$BUILD" == "linuxintegrated" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                $BUNDLE_NAME
            elif [[ $OSTYPE == freebsd ]]; then
                $BUNDLE_NAME
            fi
        elif [[ "$BUILD" == "release" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                $SCRIP_PATH/packages/linux/$BUNDLE_NAME/$BUNDLE_NAME
            elif [[ $OSTYPE == freebsd ]]; then
                $SCRIP_PATH/packages/freebsd/$BUNDLE_NAME/$BUNDLE_NAME
            fi
        fi
    fi
}

#########################################################################################
## Analyse options
#########################################################################################
detectSpec
while getopts "hdrRijcxtb:" option
do
  case $option in
    h) showHelp
      exit 0
    ;;
    d) BUILD="debug"
    ;;
    r) BUILD="release"; TRANS="y"
    ;;
    r) RUN="y"
    ;;
    i) BUILD="linuxintegrated"; TRANS="y"
    ;;
    j) MAKE_OPTS="-j"$OPTARG
    ;;
    x) TEST="y"
    ;;
    c) CLEAN="y"
    ;;
    s) SPEC=$OPTARG
    ;;
    t) TRANS="y"
    ;;
    b) BUNDLE_NAME=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
  esac
done

#########################################################################################
## Zenity dialogs
#########################################################################################
ZENITY_TITLE="FreeMedForms project builder assistant"
SEPARATOR=";"        # on modification, change also in secondPage() Release;
ZENITY="/usr/bin/zenity"
ZENITY_SIZED="$ZENITY --width 400  --height 300 --separator $SEPARATOR"
ZENITY_NO_SIZE="$ZENITY --width 300"
CONFIG=""
RET=""

createDefaultConfig()
{
    $CONFIG="FreeMedForms;Defaultdebugcompilation;Parallel;Notify;linux-g++"
}

zenityBuild()
{
    zenityConfigToBuildSystem

    if [[ "$SHOW_ZENITY_PROGRESS" == "n" ]]; then
        build
    else
        build > >($ZENITY --progress --auto-kill --title "Building the application" --text "Building..." --percentage=0 --auto-close)
    fi

    # Check log file for error message
    TEST=`cat $LOG_FILE`
    echo "$TEST"
    if [[ $(expr "$TEST" : ".*\*\*\* ERROR.*") -eq 0 ]]; then
        if [[ "$NOTIFY" == "y" ]]; then
            $ZENITY --notification --window-icon="info" --text="$BUNDLE_NAME build finished without error" &
        fi
        echo "* Build finished without error: $BUNDLE_NAME"
    else
        if [[ "$NOTIFY" == "y" ]]; then
            $ZENITY --notification --window-icon="warning" --text="WARNING: $BUNDLE_NAME build uncomplete" &
        fi
        echo "* Build UNCOMPLETE: $BUNDLE_NAME"
        return 123
    fi

    return 0
}

firstPage()
{
    if [[ -f ./build.conf ]]; then
        echo "* Found previous configuration"
        loadConfig

        TEXT_CONFIG=`echo $CONFIG | tr -s ";" "\012" |  sed  "s/_/ /g"`
        $ZENITY_NO_SIZE --title "$ZENITY_TITLE" --question --text "A previous build configuration was found.\n\n$TEXT_CONFIG" \
                       --ok-label "Build last configuration" --cancel-label "Load and edit the configuration"

        if [[ "$?" -eq 0 ]]; then
            echo "* Build required"
            RET=""
            zenityBuild
            exit $?
        else
            echo "* Loading previous configuration"
            RET=""
        fi
    fi

    RET=$($ZENITY_SIZED --title "$ZENITY_TITLE" --list --text "Select the project to build" \
            --radiolist --column "Select" --column "Project"  \
            `[ $(expr "$CONFIG" : ".*FreeMedForms.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeMedForms"  \
            `[ $(expr "$CONFIG" : ".*FreeDiams.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeDiams"  \
            `[ $(expr "$CONFIG" : ".*FreeAccount.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeAccount"  \
            `[ $(expr "$CONFIG" : ".*FreePad.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreePad"  \
            `[ $(expr "$CONFIG" : ".*FreePort.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreePort"  \
            `[ $(expr "$CONFIG" : ".*FreeToolBox.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeToolBox")
}

secondPage()
{
    RET=$($ZENITY_SIZED --title "$ZENITY_TITLE" --list --text "Select the build" --radiolist --column "Select" --column "Build"  \
            `[ $(expr "$CONFIG" : ".*Default_debug_compilation.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Default debug compilation (recommended)"  \
            `[ $(expr "$CONFIG" : ".*Debug_without_install.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Debug without installation"  \
            `[ $(expr "$CONFIG" : ".*Debug_with_install.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Debug with installation"  \
            `[ $(expr "$CONFIG" : ".*Release_build.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Release build"  \
            `[ $(expr "$CONFIG" : ".*Release_Linux_Integrated.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Release Linux Integrated")
}

thirdPage()
{
    RET=$($ZENITY_SIZED --title "$ZENITY_TITLE" --list --text "Select options" --checklist --column "Select" --column "Options"  \
           `[ $(expr "$CONFIG" : ".*Create_translations.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Create translations"  \
           `[ $(expr "$CONFIG" : ".*Clean_build.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Clean build path"  \
           `[ $(expr "$CONFIG" : ".*Parallel.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Parallel build" \
           `[ $(expr "$CONFIG" : ".*Notify.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Notify when done" \
           `[ $(expr "$CONFIG" : ".*Run_app.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Run application")
}

fourthPage()
{
    SPEC=`echo ${CONFIG##*;}`
    RET=$($ZENITY_NO_SIZE --title "$ZENITY_TITLE" --text "Please enter the spec file to use" --entry --entry-text "$SPEC")
}

saveConfig()
{
    echo "CONFIG=\"$CONFIG\"" | sed  "s/ /_/g" > ./build.conf
    chmod 700 ./build.conf
}

loadConfig()
{
    . ./build.conf
    echo "* Load configuration: $CONFIG"
}

zenityConfigToBuildSystem()
{
    SPEC=`echo ${CONFIG##*;}`
    # Options
    CLEAN="`[ $(expr "$CONFIG" : ".*Clean_build_path.*") -ne 0 ] && echo 'y'`"
    RUN="`[ $(expr "$CONFIG" : ".*Run_app.*") -ne 0 ] && echo 'y'`"
    TRANS="`[ $(expr "$CONFIG" : ".*Create_translations.*") -ne 0 ] && echo 'y'`"
    MAKE_OPTS="`[ $(expr "$CONFIG" : ".*Parallel.*") -ne 0 ] && echo '-j4'`"
    NOTIFY="`[ $(expr "$CONFIG" : ".*Notify.*") -ne 0 ] && echo 'y'`"
    # Build
    if [[ $(expr "$CONFIG" : ".*Default_debug_compilation.*") -ne 0 ]]; then
        BUILD="debug"
    elif [[ $(expr "$CONFIG" : ".*Debug_without_install.*") -ne 0 ]]; then
        BUILD="debug"
    elif [[ $(expr "$CONFIG" : ".*Debug_with_install.*") -ne 0 ]]; then
        BUILD="debug"
    #elif [[ $(expr "$CONFIG" : ".*Release_build.*") -ne 0 ]]; then
    #    BUILD="release"
    elif [[ $(expr "$CONFIG" : ".*Release_Linux_Integrated.*") -ne 0 ]]; then
        BUILD="linuxintegrated"
    fi
    BUNDLE_NAME=`echo ${CONFIG%%;*} | tr [:upper:] [:lower:]`

    echo "* Zenity build system"
    echo "    Bundle: $BUNDLE_NAME"
    echo "    Build: $BUILD"
    echo "    Clean: $CLEAN"
    echo "    Translations: $TRANS"
    echo "    Parallel: $MAKE_OPTS"
    echo "    Run after build: $RUN"
    echo "    Notify: $NOTIFY"
    echo "    SPEC: $SPEC"
}

startZenityDialog()
{
    firstPage
    CONF=$RET
    secondPage
    CONF=$CONF$SEPARATOR$RET
    thirdPage
    CONF=$CONF$SEPARATOR$RET
    fourthPage
    CONF=$CONF$SEPARATOR$RET

    CONFIG=$CONF
    saveConfig

    zenityBuild
    exit $?
}

#########################################################################################
## Start build process
#########################################################################################
if [[ -z $BUILD || -z $BUNDLE_NAME ]]; then
    if [[ -f $ZENITY ]]; then
        echo "* No command line params: starting zenity configuration"
        startZenityDialog
    fi
fi

build
if [[ "$?" == 0 ]]; then
    echo "* Build finished without error: $BUNDLE_NAME"
    launchApplication
else
    echo "* Build UNCOMPLETE: $BUNDLE_NAME"
fi

exit 0
