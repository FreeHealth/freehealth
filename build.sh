#!/bin/bash
# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2014  Eric MAEKER, MD
#
# This script helps on the compilation on unices machines
#

SCRIPT_VERSION=1.2-1406739164 # see date "+%s"

DEBUG_BUILD_COMMANDS=0  # set to 1 to only debug commands (no commands are executed)

LOWERED_BUNDLE_NAME=""
CAMELCASE_BUNDLE_NAME=""
PROJECT_VERSION=""
QT_VERSION=""
TEST=""
TRANS=""
CLEAN=""
WEBCAM=""
ALERT=""
SPEC=""
BUILD="debug"
QMAKE_CONFIG=""
MAKE_OPTS="-j4"
MAKE_QUAZIP=""
RUN=""
ZENITY="zenity"
SHOW_ZENITY_PROGRESS="y"  # y / n
LOG_FILE=""
GIT_PULL=""
SLEEP_TIME=0
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
    echo "  -b  Application name in CamelCase. Values can be:"
    echo "      FreeMedForms, FreeDiams, FreeDDIManager, FreeToolBox"
    echo "      FreeICD, FreeDRC, FreePad..."
    echo "  -d  Build a debug 'no-install' version (default)"
    echo "  -r  Build a release version and install it:"
    echo "        1. Clean build path"
    echo "        2. Create translations"
    echo "        3. Compile application"
    echo "        4. Install application"
    echo "  -i  with 'linux integrated' configuration (release mode only) same as -ri"
    echo "  -w  build the webcam plugin (requires libopencv-dev)"
    echo "  -a  build the alert plugin"
    echo "  -j  Parallel compilation (number of jobs: equivalent to make -jX)"
    echo "  -t  Create translations"
    echo "  -x  Create a test project (in the test path)"
    echo "  -c  Make clean before compiling"
    echo "  -s  Use the specified spec file (default spec: $SPEC)"
    echo "  -R  Run application after the build"
    echo "  -X  Debug commands to console during a dry run"
    echo "  -h  Show this help"
    echo
}

#########################################################################################
## Build commands
#########################################################################################
testDependencies()
{
    # TODO: this code only works with debian distros, include other distro
    # DEPENDENCIES_QT5="qt5-default qt5-qmake"
    DEPENDENCIES_QT4="libqt4-dev libxext-dev" # libquazip-dev not in wheezy
    DEPENDENCIES_WEBCAM=""
    if [[ "$WEBCAM" == "y" ]]; then
        DEPENDENCIES_WEBCAM="libopencv-core-dev libopencv-highgui-dev libopencv-objdetect-dev"
    fi
    DEPENDENCIES="zlib1g-dev "$DEPENDENCIES_QT4" "$DEPENDENCIES_WEBCAM
    for d in $DEPENDENCIES ; do
        echo "* Checking dependencies $d"
        if [[ -z `dpkg -l | grep $d` ]] ; then
            echo "Please install $d"
            echo "    sudo apt-get install $d"
            exit 1;
        fi
    done
    if [[ -z `dpkg -l | grep libquazip0-dev` ]] ; then
        MAKE_QUAZIP="y"
    fi
}

createLogFile()
{
    LOG_FILE="./build_`date "+%s"`.log"
    echo "* Creating build log: $LOG_FILE"
    rm -rf $LOG_FILE
    touch $LOG_FILE
    echo "Log file created on: `date "+%Y.%m.%d %H:%M %N"`" > $LOG_FILE
}

detectQtVersion()
{
    QT_VERSION=`qmake -query "QT_VERSION"  | cut -d . -s -f1`
    echo >> $LOG_FILE
    echo "* Qt information" >> $LOG_FILE
    echo "  Version: `qmake -query \"QT_VERSION\"`" >> $LOG_FILE
    echo "  Installed in `qmake -query \"QT_INSTALL_PREFIX\"`" >> $LOG_FILE
    echo "  Resources can be found in the following locations:" >> $LOG_FILE
    echo "    Documentation: `qmake -query \"QT_INSTALL_DOCS\"`" >> $LOG_FILE
    echo "    Header files: `qmake -query \"QT_INSTALL_HEADERS\"`" >> $LOG_FILE
    echo "    Libraries: `qmake -query \"QT_INSTALL_LIBS\"`" >> $LOG_FILE
    echo "    Binary files (executables): `qmake -query \"QT_INSTALL_BINS\"`" >> $LOG_FILE
    echo "    Plugins: `qmake -query \"QT_INSTALL_PLUGINS\"`" >> $LOG_FILE
    echo "    Data files: `qmake -query \"QT_INSTALL_DATA\"`" >> $LOG_FILE
    echo "    Translation files: `qmake -query \"QT_INSTALL_TRANSLATIONS\"`" >> $LOG_FILE
    echo "    Settings: `qmake -query \"QT_INSTALL_SETTINGS\"`" >> $LOG_FILE
    echo "    Examples: `qmake -query \"QT_INSTALL_EXAMPLES\"`" >> $LOG_FILE
    echo "    Demonstrations: `qmake -query \"QT_INSTALL_DEMOS\"`" >> $LOG_FILE
    echo >> $LOG_FILE
}

detectSpec()
{
    echo "* Guessing spec file"
    SPEC_PATH=`qmake -query "QT_INSTALL_DATA"`
    SPEC_PATH=$SPEC_PATH"/mkspecs"
    SPEC=`ls -al $SPEC_PATH/default | sed "s/^.*default -> //"`
    if [[ -z $SPEC ]]; then
        echo "ERROR: no default spec detected"
    fi
    echo "Default qmake spec: "$SPEC
    echo "* Default qmake spec: "$SPEC >> $LOG_FILE
    return 0
}

detectGit()
{
    TEST_GIT=`git --version && [ -e ".git" ]`
    TEST_GIT=$?
    return $TEST_GIT
}

# Run a specific bash command, test its result and add a message to the logfile
# $1: The command
runCommand()
{
    COMMAND=$1
    if [[ "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
        echo "# $COMMAND" >> $LOG_FILE
        echo "# $COMMAND"
    else
        EXEC=`$COMMAND`
        EXEC=$?
        if [[ ! $EXEC == 0 ]]; then
            echo "[ERROR]: # $COMMAND" >> $LOG_FILE
            return 123
        else
            echo "[Ok]: $COMMAND" >> $LOG_FILE
       fi
    fi
}

gitPull()
{
    # zenity progress feature
    echo "0"; sleep $SLEEP_TIME
    echo "# Updating your local source copy" ; sleep $SLEEP_TIME
    if [[ "$GIT_PULL" == "y" ]]; then
        echo "* Updating your local git repo" >> $LOG_FILE
        runCommand "cd $SCRIPT_PATH; git pull"
    fi
}

makeClean()
{
    # zenity progress feature
    echo "10"; sleep $SLEEP_TIME
    echo "# Cleaning build path" ; sleep $SLEEP_TIME
    if [[ "$CLEAN" == "y" ]]; then
        echo "* Cleaning build path" >> $LOG_FILE
        SPEC_FOR_PATH=`echo $SPEC | sed y/+/p/`
        if [[ -z $LOWERED_BUNDLE_NAME ]]; then
            LOWERED_BUNDLE_NAME="*"
            CAMELCASE_BUNDLE_NAME="*"
        fi
        # Remove all (path/files)
        #   bin/freeddimanager_Qt4_linux-gpp-64
        #   build/Qt4_linux-gpp/FreeDDIManager
        #   Makefile
        #   .qmake.cache
        runCommand "rm -rf $SCRIPT_PATH/bin/"$LOWERED_BUNDLE_NAME"_Qt"$QT_VERSION"_"$SPEC_FOR_PATH
        runCommand "rm -rf $SCRIPT_PATH/build/Qt"$QT_VERSION"_"$SPEC_FOR_PATH"/"$CAMELCASE_BUNDLE_NAME
        runCommand "find . -type f -name \"Makefile*\" -delete"
        runCommand "find . -type f -name \".qmake.cache\" -delete"
        echo >> $LOG_FILE
        if [[ "$LOWERED_BUNDLE_NAME" == "*" ]]; then
            LOWERED_BUNDLE_NAME=""
            CAMELCASE_BUNDLE_NAME=""
        fi
    fi
    return 0
}

createTranslations()
{
    # zenity progress feature
    echo "20"; sleep $SLEEP_TIME
    echo "# Creation translations" ; sleep $SLEEP_TIME
    if [ "$TRANS" == "y" ]; then
        echo "* Creating translations" >> $LOG_FILE
        runCommand "lrelease ./global_resources/translations/*.ts"
    fi
    return 0
}

qmakeCommand()
{
    # TODO: update this part using the buildspec/optionalplugins.pri manage libquazip0-dev build
    # zenity progress feature
    echo "30"; sleep $SLEEP_TIME
    echo -e "# Preparing the build:\nrunning qmake" ; sleep $SLEEP_TIME
    EXTRAPLUGS=""
    if [[ "$WEBCAM" == "y" ]]; then
        EXTRAPLUGS="with-webcam"
    fi    
    if [[ "$ALERT" == "y" ]]; then
        EXTRAPLUGS="$EXTRAPLUGS with-alerts"
    fi
    if [[ "$EXTRAPLUGS" != "" ]]; then
        QMAKE_CONFIG="$QMAKE_CONFIG CONFIG+=\"$EXTRAPLUGS\""
    fi
    runCommand "qmake $LOWERED_BUNDLE_NAME.pro -r $QMAKE_CONFIG -spec $SPEC"
    return 0
}

makeCommand()
{
    # zenity progress feature
    echo "40"; sleep $SLEEP_TIME
    echo "# Building the application (make step)" ; sleep $SLEEP_TIME
    runCommand "make $MAKE_OPTS"
    return 0
}

makeInstallCommand()
{
    # zenity progress feature
    echo "70"; sleep $SLEEP_TIME
    echo "# Installing applications" ; sleep $SLEEP_TIME
    if [[ "$BUILD" != "debug" ]]; then
        echo "# make install" >> $LOG_FILE
        runCommand "make install"
    fi
    return 0
}

finalMessage()
{
    # zenity progress feature
    echo "90"; sleep $SLEEP_TIME
    echo "# Compilation completed (with or without error)" ; sleep $SLEEP_TIME
    SPEC_FOR_PATH=`echo $SPEC | sed y/+/p/`
    BINPATH=$SCRIPT_PATH"/bin/"$LOWERED_BUNDLE_NAME"_Qt"$QT_VERSION"_"$SPEC_FOR_PATH
    if [[ $OSTYPE == linux-gnu ]]; then
        echo "*** Start application with:"
        echo $BINPATH"/"$LOWERED_BUNDLE_NAME"_debug --config=../global_resources/"$LOWERED_BUNDLE_NAME"_config.ini"
    elif [[ $OSTYPE == darwin* ]]; then
        echo "*** Start application with:"
        echo $BINPATH"/"$LOWERED_BUNDLE_NAME"/"$LOWERED_BUNDLE_NAME"_debug.app/Contents/MacOs/"$LOWERED_BUNDLE_NAME"_debug --config=../../../../../global_resources/"$LOWERED_BUNDLE_NAME"_config.ini"
    elif [[ $OSTYPE == freebsd ]]; then
        echo "*** Start application with:"
        echo $BINPATH"/"$LOWERED_BUNDLE_NAME"/"$LOWERED_BUNDLE_NAME"_debug --config=../../global_resources/"$LOWERED_BUNDLE_NAME"_config.ini"
    fi
    return 0
}

build()
{
    echo "Starting build: application=$LOWERED_BUNDLE_NAME" >> $LOG_FILE
    # No bundle name -> Error
    if [[ "$LOWERED_BUNDLE_NAME" == "" ]]; then
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

    MSG="DEBUG mode"
    if [[ "$BUILD" != "debug" ]]; then
        MSG="RELEASE mode"
    fi
    echo "*** Building application in $MSG: $LOWERED_BUNDLE_NAME ($BUILD) ***"
    if [[ "$BUILD" != "debug" ]]; then
        if [[ $OSTYPE == darwin* ]]; then
            # Switch to mac_release.sh
            if [[  "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
                echo "# cd scripts\n./mac_release.sh -b $LOWERED_BUNDLE_NAME\nreturn 0"
            else
                cd scripts
                echo "  **** -> switching to: scripts/mac_release.sh -b $LOWERED_BUNDLE_NAME"
                ./mac_release.sh -b $LOWERED_BUNDLE_NAME
                return $?
            fi
        fi
    fi

  if [[ "$TEST" != "" ]]; then
      echo "* Building a test application"
      cd ./tests
  fi

  if [[ ! -e $LOWERED_BUNDLE_NAME ]]; then
      echo "* ERROR: Application does not exist: $LOWERED_BUNDLE_NAME"
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

  echo "# Entering $LOWERED_BUNDLE_NAME"; sleep $SLEEP_TIME
  cd $LOWERED_BUNDLE_NAME
  if [[ "$?" != 0 ]]; then
      echo "*** ERROR: entering $LOWERED_BUNDLE_NAME." >> $LOG_FILE
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
        echo "* Launch the application"

        SPEC_FOR_PATH=`echo $SPEC | sed y/+/p/`
        BINPATH=$SCRIPT_PATH"/bin/"$LOWERED_BUNDLE_NAME"_Qt"$QT_VERSION"_"$SPEC_FOR_PATH

        if [[ "$BUILD" == "debug" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (debug_without_install, linux)"
                $BINPATH"/"$LOWERED_BUNDLE_NAME_debug --config=../../global_resources/$LOWERED_BUNDLE_NAME_config.ini
            elif [[ $OSTYPE == darwin* ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (debug_without_install, MacOs)"
                $BINPATH/$LOWERED_BUNDLE_NAME_debug.app/Contents/MacOs/$LOWERED_BUNDLE_NAME_debug --config=../../../../../global_resources/$LOWERED_BUNDLE_NAME_config.ini
            elif [[ $OSTYPE == freebsd ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (debug_without_install, freebsd)"
                $BINPATH/$LOWERED_BUNDLE_NAME_debug --config=../../global_resources/$LOWERED_BUNDLE_NAME_config.ini
            fi
        elif [[ "$BUILD" == "linuxintegrated" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (release, linux integrated)"
               $LOWERED_BUNDLE_NAME
            elif [[ $OSTYPE == freebsd ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (release, freebsd, linux integrated)"
                $LOWERED_BUNDLE_NAME
            fi
        elif [[ "$BUILD" == "release" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (release, linux , bundled)"
                $SCRIP_PATH/packages/linux/$LOWERED_BUNDLE_NAME/$LOWERED_BUNDLE_NAME
            elif [[ $OSTYPE == freebsd ]]; then
                echo "    Launch $LOWERED_BUNDLE_NAME (release, freebsd, bundled)"
                $SCRIP_PATH/packages/freebsd/$LOWERED_BUNDLE_NAME/$LOWERED_BUNDLE_NAME
            fi
        fi
    fi
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hdrRijcxXtwab:" option
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
    w) WEBCAM="y"
    ;;
    a) ALERT="y"
    ;;
    b) LOWERED_BUNDLE_NAME=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
       CAMELCASE_BUNDLE_NAME=$OPTARG
    ;;
    X) DEBUG_BUILD_COMMANDS=1
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
    $CONFIG="FreeMedForms;Default_debug_compilation_(recommended)"
    $CONFIG="$CONFIG;Create_translations;Parallel_build;Notify_when_done;Run_application;$SPEC"
}

zenityBuild()
{
    loadConfig
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
            $ZENITY --notification --window-icon="info" --text="$LOWERED_BUNDLE_NAME build finished without error" &
        fi
        echo "* Build finished without error: $LOWERED_BUNDLE_NAME"
    else
        if [[ "$NOTIFY" == "y" ]]; then
            $ZENITY --notification --window-icon="warning" --text="WARNING: $LOWERED_BUNDLE_NAME build uncomplete" &
        fi
        echo "* Build UNCOMPLETE: $LOWERED_BUNDLE_NAME"
        return 123
    fi

    return 0
}

gitPage()
{
    echo "DETECTING GIT "`detectGit`
    if [[ `detectGit` -eq 0 ]]; then
        echo "* Git detected"
        $ZENITY_NO_SIZE --title "$ZENITY_TITLE" --question \
                        --text "A revision control (git clone) was found.\nDo you want to update your local working copy of the source ?" \
                        --ok-label "Yes, update" --cancel-label "No, compile now"
        if [[ "$?" -eq 0 ]]; then
            echo "* git pull"
            RET=""
            GIT_PULL="y"
            gitPull
            exit $?
        else
            echo "* Don't update source copy"
            RET=""
        fi        
    else
        echo "* Git not detected"
    fi
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
            `[ $(expr "$CONFIG" : ".*FreeDDIManager.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeDDIManager"  \
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
           `[ $(expr "$CONFIG" : ".*Build_WebCam_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Build WebCam plugin"  \
           `[ $(expr "$CONFIG" : ".*Build_Alert_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Build Alert plugin"  \
           `[ $(expr "$CONFIG" : ".*Create_translations.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Create translations"  \
           `[ $(expr "$CONFIG" : ".*Clean_build_path.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Clean build path"  \
           `[ $(expr "$CONFIG" : ".*Parallel_build.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Parallel build" \
           `[ $(expr "$CONFIG" : ".*Notify_when_done.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Notify when done" \
           `[ $(expr "$CONFIG" : ".*Run_application.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Run application")
}

fourthPage()
{
    SPEC=`echo ${CONFIG##*;}`
    RET=$($ZENITY_NO_SIZE --title "$ZENITY_TITLE" --text "Optional: Please enter a spec file, if you want to use one:\n(If not, just leave empty)" --entry --entry-text "$SPEC")
}

saveConfig()
{
    echo "CONFIG=\"$CONFIG\"" | sed  "s/ /_/g" > ./build.conf
    chmod 700 ./build.conf
}

loadConfig()
{
    if [[ -e ./build.conf ]]; then
        . ./build.conf
        echo "* Previous configuration loaded"
    else
        createDefaultConfig
        echo "* Using default configuration"
    fi
}

zenityConfigToBuildSystem()
{
    echo "* CONFIG: $CONFIG"
    SPEC=`echo ${CONFIG##*;}`
    SLEEP_TIME=1
    # Options
    CLEAN="`[ $(expr "$CONFIG" : ".*Clean_build_path.*") -ne 0 ] && echo 'y' || echo 'n'`"
    WEBCAM="`[ $(expr "$CONFIG" : ".*Build_WebCam_plugin.*") -ne 0 ] && echo 'y' || echo 'n'`"
    ALERT="`[ $(expr "$CONFIG" : ".*Build_Alert_plugin.*") -ne 0 ] && echo 'y' || echo 'n'`"
    RUN="`[ $(expr "$CONFIG" : ".*Run_application.*") -ne 0 ] && echo 'y' || echo 'n'`"
    TRANS="`[ $(expr "$CONFIG" : ".*Create_translations.*") -ne 0 ] && echo 'y' || echo 'n'`"
    MAKE_OPTS="`[ $(expr "$CONFIG" : ".*Parallel_build.*") -ne 0 ] && echo '-j4' || echo '-j1'`"
    NOTIFY="`[ $(expr "$CONFIG" : ".*Notify_when_done.*") -ne 0 ] && echo 'y' || echo 'n'`"
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
    CAMELCASE_BUNDLE_NAME=`echo ${CONFIG%%;*}`
    LOWERED_BUNDLE_NAME=`echo $CAMELCASE_BUNDLE_NAME | tr [:upper:] [:lower:]`

    echo "* Zenity build system"
    echo "    Bundle: $CAMELCASE_BUNDLE_NAME ($LOWERED_BUNDLE_NAME) ; $BUILD"
    echo "    Clean: $CLEAN"
    echo "    Translations: $TRANS"
    echo "    Parallel: $MAKE_OPTS"
    echo "    Run after build: $RUN"
    echo "    Notify: $NOTIFY"
    echo "    Spec file: $SPEC"
    EXTRA_PLUGS=""
    if [[ "$WEBCAM" == "y" ]]; then
        EXTRA_PLUGS="WebCam plugin; "
    fi    
    if [[ "$ALERT" == "y" ]]; then
        EXTRA_PLUGS=$EXTRA_PLUGS"Alert plugin; "
    fi
    if [[ "$EXTRA_PLUGS" != "" ]]; then
        echo "    Extra-plugins: $EXTRA_PLUGS"
    fi
}

startZenityDialog()
{
    gitPage
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
createLogFile
detectQtVersion

if [[ -z $BUILD || -z $LOWERED_BUNDLE_NAME ]]; then
    if [[ -z $CLEAN ]]; then
        if [[ -f $ZENITY ]]; then
            echo "* No command line params: starting zenity configuration"
            startZenityDialog
        fi
    else
        # Clean only command
        makeClean
        exit 0
    fi
fi

# Run the build commands
testDependencies
detectSpec
build
if [[ "$?" == 0 ]]; then
    echo "* Build finished without error: $LOWERED_BUNDLE_NAME"
    launchApplication
else
    echo "* Build UNCOMPLETE: $LOWERED_BUNDLE_NAME"
fi

exit 0
