#!/bin/bash
# FreeHealth https://freehealth.io
#
# FreeHealth is a fork of:
# FreeMedForms
# (c) 2008 - 2015  Eric MAEKER, MD
#
# This script helps compilation on Unix derivatives
#

SCRIPT_VERSION=1.3-1406796656 # see date "+%s"
DEBUG_BUILD_COMMANDS=0    # set to 1 to only debug commands (no commands are executed)
LOWERED_BUNDLE_NAME=""    # Application to build (lower case)
CAMELCASE_BUNDLE_NAME=""  # Application to build (camel case)
PROJECT_VERSION=""        # Contains the building project version (automatically detected)
QT_VERSION=""             # Contains the default Qt version (automatically detected)
TEST=""                   # y/n. Build a "test" application
TRANS=""                  # y / n. If "y" -> process translations files
CLEAN=""                  # y / n. If "y" -> make clean
SPEC=""                   # qmake spec file to use (automatically detected)
BUILD="debug"             # Type of build: debug, release, linuxintegrated
QMAKE_CONFIG=""           #
MAKE_OPTS="-j4"           # make options (by default: multi-jobs build)
MAKE_QUAZIP=""            # 
RUN=""                    #
ZENITY="zenity"           # GUI dialog to use (kdialog/zenity)
SHOW_ZENITY_PROGRESS="y"  # y / n
LOG_FILE=""               # Log filename
GIT_PULL=""               # y / n (if "y" a git pull command will be executed)
SLEEP_TIME=0              # Sleep time for zenity progress bar
REQUESTED_PLUGINS=""      # This var will store the optional plugins requested by the user
AVAILABLE_OPTIONAL_PLUGINS="agenda alerts feedback pad pmh webcam edrc"
AVAILABLE_CAMELCASE_APPS="FreeHealth FreeDiams FreePad FreePort FreeDRC FreeDDIManager FreeICD FreeToolBox"
RUNNING_ZENITY=0          # 0 / 1 (when zenity is used => set to 1)
PROJECT_NAME="FreeHealth" # name of the project

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
    echo $SCRIPT_NAME" builds the *debug_without_install* Linux versions of the FreeHealth applications project."
    echo "Project version: $PROJECT_VERSION - $SCRIPT_VERSION"
    echo "Usage : $SCRIPT_NAME -txch -dri -s <qt.spec> -b <AppToBuild>"
    echo
    echo "Options :"
    echo "  -b  Application name in CamelCase. Values can be:"
    echo "      $AVAILABLE_CAMELCASE_APPS"
    echo "  -d  Build a debug 'no-install' version (default)"
    echo "  -r  Build a release version and install it:"
    echo "        1. Clean build path"
    echo "        2. Create translations"
    echo "        3. Compile application"
    echo "        4. Install application"
    echo "  -i  with 'linux integrated' configuration (release mode) same as -ri"
    echo "  -p  optional plugins to include (separated with a space char)"
    echo "      Select plugins within the following list:"
    echo $AVAILABLE_OPTIONAL_PLUGINS | tr " " "\n" | sed 's/^/        - /'
    echo "  -j  Parallel compilation (number of jobs: equivalent to make -jX)"
    echo "  -t  Create translations"
    echo "  -x  Create a test project (in the test path)"
    echo "  -c  Make clean before compiling"
    echo "  -s  Use the specified spec file (default spec: $SPEC)"
    echo "  -R  Run application after the build"
    echo "  -z  Don't show zenity progress bar (useful for debugging from console)"
    echo "  -l  Create a build log (nothing will be printed in the console)"
#    echo "  -X  Debug commands to console during a dry run"
    echo "  -h  Show this help"
    echo
}

#########################################################################################
## Build commands
#########################################################################################
testDependencies()
{
    # TODO: this code only works with debian distros, include other distro
    DEPENDENCIES_QT5="qttools5-dev-tools \
    libqt5svg5-dev \
    qtscript5-dev \
    libqt5scripttools5 \
    qttools5-dev \
    libquazip-dev \
    qtxmlpatterns5-dev-tools"
    DEPENDENCIES_WEBCAM=""
    if [[ "$WEBCAM" == "y" ]]; then
        DEPENDENCIES_WEBCAM="libopencv-core-dev libopencv-highgui-dev libopencv-objdetect-dev"
    fi
    DEPENDENCIES="zlib1g-dev "$DEPENDENCIES_QT5" "$DEPENDENCIES_WEBCAM
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
    LOG_FILE="$SCRIPT_PATH/build/build_`date "+%s"`.log"
    echo "Log file: "$LOG_FILE
    exec 3>&1 4>&2
    trap 'exec 2>&4 1>&3' 0 1 2 3
    exec 1>$LOG_FILE 2>&1
    # Everything below will go to the file 'log.out':
}

detectQtVersion()
{
    QT_VERSION=`qmake -query "QT_VERSION"  | cut -d . -s -f1`
    echo "* Qt information"
    echo "  Version: `qmake -query \"QT_VERSION\"`"
    echo "  Installed in `qmake -query \"QT_INSTALL_PREFIX\"`"
    echo "  Resources can be found in the following locations:"
    echo "    Documentation: `qmake -query \"QT_INSTALL_DOCS\"`"
    echo "    Header files: `qmake -query \"QT_INSTALL_HEADERS\"`"
    echo "    Libraries: `qmake -query \"QT_INSTALL_LIBS\"`"
    echo "    Binary files (executables): `qmake -query \"QT_INSTALL_BINS\"`"
    echo "    Plugins: `qmake -query \"QT_INSTALL_PLUGINS\"`"
    echo "    Data files: `qmake -query \"QT_INSTALL_DATA\"`"
    echo "    Translation files: `qmake -query \"QT_INSTALL_TRANSLATIONS\"`"
    echo "    Settings: `qmake -query \"QT_INSTALL_SETTINGS\"`"
    echo "    Examples: `qmake -query \"QT_INSTALL_EXAMPLES\"`"
    echo "    Demonstrations: `qmake -query \"QT_INSTALL_DEMOS\"`"
    echo
}

detectSpec()
{
    if [[ "$QT_VERSION" == "5" ]]; then
        SPEC=`qmake -query QMAKE_SPEC`
            if [[ "$SPEC" == "**Unknown**" ]]; then
                echo "ERROR: qmake -query couldn't detect your default mkspecs"
                exit 1
            fi
        echo "* Default qmake spec: "$SPEC
        return 0
    else
        echo "Your Qt version seems to be incorrect."
        echo "To build $PROJECT_NAME applications, use Qt 5.7 or higher"
        exit 1
    fi
}

detectGit()
{
    TEST_GIT=`git --version && [ -e ".git" ]`
    TEST_GIT=$?
    return $TEST_GIT
}

# Test if $1 is a valid plugin (registered in the AVAILABLE_OPTIONAL_PLUGINS var)
isPlugin()
{
    echo $AVAILABLE_OPTIONAL_PLUGINS | grep -F -q -w "$1";
}

# Test if $1 is a valid plugin (registered in the REQUESTED_PLUGINS var)
isRequestedPlugin()
{
    echo $REQUESTED_PLUGINS | grep -F -q -w "$1";
}

# Test if $1 is a valid application name (registered in the AVAILABLE_CAMELCASE_APPS var)
isValidCamelCaseApplication()
{
    echo $AVAILABLE_CAMELCASE_APPS | grep -F -q -w "$1";
}

# Run a specific bash command, test its result and add a message to the logfile
# $1: The command
#runCommand()
#{
#    COMMAND=$1
#    if [[ "$DEBUG_BUILD_COMMANDS" == 1 ]]; then
#        echo "    [Ok] $COMMAND"
#        echo "    [Ok] $COMMAND"
#    else
#        EXEC=$($COMMAND 2>&1)
#        if [[ ! $? == 0 ]]; then
#            echo "    [ERROR] $COMMAND"
#            echo "      "$EXEC
#            echo "    [ERROR] $COMMAND"
#            echo "      "$EXEC
#            return 123
#        else
#            echo "    [Ok] $COMMAND"
#            echo "    [Ok] $COMMAND"
#            [[ ! -z $EXEC ]] && {
#                echo $EXEC
#                echo $EXEC
#            }
#       fi
#                    echo $EXEC
#    fi
#    return 0
#}

# Update zenity progress dialog. This will also add the message to the log file & console
# $1: percent
# $2: message
updateZenityProgress()
{
    [[ $RUNNING_ZENITY == 1 ]] && {
        echo "$1"
        echo "# $2"
        sleep $SLEEP_TIME
    }
    echo "* $2"
}

gitPull()
{
    updateZenityProgress 0 "Updating your local git repo"
    if [[ "$GIT_PULL" == "y" ]]; then
        # runCommand "cd $SCRIPT_PATH; git pull"
        cd $SCRIPT_PATH
        git pull
    fi
}

makeClean()
{
    updateZenityProgress 10 "Cleaning build path"
    if [[ "$CLEAN" == "y" ]]; then
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
        # runCommand "rm -rf $SCRIPT_PATH/bin/"$LOWERED_BUNDLE_NAME"_Qt"$QT_VERSION"_"$SPEC_FOR_PATH || return 1
        # runCommand "rm -rf $SCRIPT_PATH/build/Qt"$QT_VERSION"_"$SPEC_FOR_PATH"/"$CAMELCASE_BUNDLE_NAME || return 1
        # runCommand "find . -type f -name \"Makefile*\" -delete" || return 1
        # runCommand "find . -type f -name \".qmake.cache\" -delete" || return 1
        rm -rf $SCRIPT_PATH/bin/$LOWERED_BUNDLE_NAME"_Qt"$QT_VERSION"_"$SPEC_FOR_PATH
        rm -rf $SCRIPT_PATH/build/Qt$QT_VERSION"_"$SPEC_FOR_PATH"/"$CAMELCASE_BUNDLE_NAME
        find . -type f -name "Makefile*" -exec rm {} \;
        find . -type f -name ".qmake.cache" -exec rm {} \;
        if [[ "$LOWERED_BUNDLE_NAME" == "*" ]]; then
            LOWERED_BUNDLE_NAME=""
            CAMELCASE_BUNDLE_NAME=""
        fi
    fi
    return 0
}

createTranslations()
{
    updateZenityProgress 20 "Creation translations"
    if [ "$TRANS" == "y" ]; then
        # runCommand "lrelease ./global_resources/translations/*.ts" || return 1
        lrelease ./global_resources/translations/*.ts
    fi
    return 0
}

qmakeCommand()
{
    updateZenityProgress 30 "Preparing the build: qmake"
    FILE=$SCRIPT_PATH/buildspecs/optionalplugins.pri

    # Modify optional plugins file
    for PLUG in $AVAILABLE_OPTIONAL_PLUGINS ; do
        ORIG=".*CONFIG.*with-$PLUG.*"
        SUBSTIT=""
        isRequestedPlugin $PLUG && {
            SUBSTIT="      CONFIG *= with-$PLUG"
        } || {            
            SUBSTIT="    # CONFIG *= with-$PLUG"
        }
        # FIXME: unable to send this command using runCommand()
        sed -i "s/$ORIG/$SUBSTIT/" $FILE
    done
    CHANGE=`echo $LOG_FILE | sed "s#./##"`
    # sed -i "1i\# This file was modified by $SCRIPT_NAME (see $CHANGE)\n" $FILE

    updateZenityProgress 35 "Starting: qmake"
    # runCommand "qmake $LOWERED_BUNDLE_NAME.pro -r $QMAKE_CONFIG -spec $SPEC" || return 1
    qmake $LOWERED_BUNDLE_NAME.pro -r $QMAKE_CONFIG -spec $SPEC

    return 0
}

makeCommand()
{
    updateZenityProgress 40 "Starting: make"
    # runCommand "make $MAKE_OPTS" || return 1
    make $MAKE_OPTS
    return 0
}

makeInstallCommand()
{
    if [[ "$BUILD" != "debug" ]]; then
        # runCommand "make install" || return 1
        make install
    fi
    return 0
}

finalMessage()
{
    updateZenityProgress 90 "End of compilation"
    echo
    SPEC_FOR_PATH=`echo $SPEC | sed y/+/p/`
    BINPATH=$SCRIPT_PATH"/bin/"$LOWERED_BUNDLE_NAME"_Qt"$QT_VERSION"_"$SPEC_FOR_PATH
    if [[ $OSTYPE == linux-gnu ]]; then
        echo "*** Start application with:"
        echo $BINPATH"/"$LOWERED_BUNDLE_NAME"_debug --config=../../global_resources/"$LOWERED_BUNDLE_NAME"_config.ini"
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
    if [[ "$LOWERED_BUNDLE_NAME" == "" ]]; then
        echo "*** ERROR: you must define the application with the -b param. See help -h."
        echo "*** ERROR: you must define the application with the -b param. See help -h."
        return 123;
    fi

    # Create the qmake configuration
    if [[ "$BUILD" == "debug" ]]; then
        QMAKE_CONFIG="CONFIG+=debug CONFIG-=release CONFIG+=debug_without_install"
    elif [[ "$BUILD" == "release" ]]; then
        QMAKE_CONFIG="CONFIG-=debug CONFIG+=release"
    elif [[ "$BUILD" == "linuxintegrated" ]]; then
        QMAKE_CONFIG="CONFIG-=debug CONFIG+=release CONFIG+=LINUX_INTEGRATED"
    fi

    MSG="DEBUG mode"
    if [[ "$BUILD" != "debug" ]]; then
        MSG="RELEASE mode"
    fi

    updateZenityProgress 0 "Building application in $MSG: $LOWERED_BUNDLE_NAME ($BUILD)"
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
      echo "*** ERROR: Application does not exist."
      return 123
  fi

  makeClean || return 100;

  createTranslations || return 101;

  updateZenityProgress 0 "Entering $LOWERED_BUNDLE_NAME"
  cd $LOWERED_BUNDLE_NAME || return 102;

  qmakeCommand || return 103;

  makeCommand || return 104;

  makeInstallCommand || return 105;

  updateZenityProgress 90 "Entering $SCRIPT_PATH"
  cd $SCRIPT_PATH || return 106;

  finalMessage || return 107;

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
while getopts "hdrRijlcxXtzb:p:" option
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
    z) SHOW_ZENITY_PROGRESS="n"
    ;;
    b) LOWERED_BUNDLE_NAME=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
       CAMELCASE_BUNDLE_NAME=$OPTARG
    ;;
    X) DEBUG_BUILD_COMMANDS=1
    ;;
    p) for PLUG in $OPTARG; do
           isPlugin $PLUG && {
               echo "* Adding optional plugin: " $PLUG
               REQUESTED_PLUGINS=$REQUESTED_PLUGINS" "$PLUG
           } || {
               echo "[ERROR] Requested optional plugin is not valid: " $PLUG 
           }
       done
    ;;
    l) createLogFile
  esac
done

#########################################################################################
## Zenity dialogs
#########################################################################################
ZENITY_TITLE="$PROJECT_NAME project builder assistant"
SEPARATOR=";"        # on modification, change also in secondPage() Release;
ZENITY="/usr/bin/zenity"
ZENITY_SIZED="$ZENITY --width 400  --height 400 --separator $SEPARATOR"
ZENITY_NO_SIZE="$ZENITY --width 300"
CONFIG=""
RET=""

createDefaultConfig()
{
    $CONFIG="$PROJECT_NAME;Default_debug_compilation_(recommended)"
    $CONFIG="$CONFIG;Create_translations;Parallel_build;Notify_when_done;Run_application;$SPEC"
}

zenityBuild()
{
    loadConfig
    zenityConfigToBuildSystem
    if [[ "$SHOW_ZENITY_PROGRESS" == "n" ]]; then
        build
    else
        build > >($ZENITY --progress --auto-kill \
                          --title "Building $PROJECT_NAME project's applications" \
                          --text "Building..." --percentage=0 --auto-close)
    fi

#    # Check log file for error message
#    TEST=`cat $LOG_FILE`
#    echo "$TEST"
#    if [[ $(expr "$TEST" : ".*\*\*\* ERROR.*") -eq 0 ]]; then
#        if [[ "$NOTIFY" == "y" ]]; then
#            $ZENITY --notification --window-icon="info" --text="$LOWERED_BUNDLE_NAME build finished without error" &
#        fi
#        echo "* Build finished without error: $LOWERED_BUNDLE_NAME"
#    else
#        if [[ "$NOTIFY" == "y" ]]; then
#            $ZENITY --notification --window-icon="warning" --text="WARNING: $LOWERED_BUNDLE_NAME build uncomplete" &
#        fi
#        echo "* Build UNCOMPLETE: $LOWERED_BUNDLE_NAME"
#        return 123
#    fi

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
            return $?
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
            `[ $(expr "$CONFIG" : ".*$PROJECT_NAME.*") -ne 0 ] && echo 'True' ||  echo 'False'` "$PROJECT_NAME"  \
            `[ $(expr "$CONFIG" : ".*FreeDiams.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeDiams"  \
            `[ $(expr "$CONFIG" : ".*FreeDDIManager.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeDDIManager"  \
            `[ $(expr "$CONFIG" : ".*FreePad.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreePad"  \
            `[ $(expr "$CONFIG" : ".*FreePort.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreePort"  \
            `[ $(expr "$CONFIG" : ".*FreeDRC.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeDRC"  \
            `[ $(expr "$CONFIG" : ".*FreeICD.*") -ne 0 ] && echo 'True' ||  echo 'False'` "FreeICD"  \
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
           `[ $(expr "$CONFIG" : ".*Clean_build_path.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Clean build path"  \
           `[ $(expr "$CONFIG" : ".*Parallel_build.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Parallel build" \
           `[ $(expr "$CONFIG" : ".*Notify_when_done.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Notify when done" \
           `[ $(expr "$CONFIG" : ".*Run_application.*") -ne 0 ] && echo 'True' ||  echo 'False'`  "Run application")
}

fourthPage()
{
    RET=$($ZENITY_SIZED --title "$ZENITY_TITLE" --list --text "Select optional plugins" --checklist --column "Select" --column "Plugs"  \
           `[ $(expr "$CONFIG" : ".*Agenda_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Agenda plugin"  \
           `[ $(expr "$CONFIG" : ".*Alerts_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Alerts plugin"  \
           `[ $(expr "$CONFIG" : ".*Feedback_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "Feedback plugin"  \
           `[ $(expr "$CONFIG" : ".*PadTools_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "PadTools plugin"  \
           `[ $(expr "$CONFIG" : ".*PMHx_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "PMHx plugin"  \
           `[ $(expr "$CONFIG" : ".*WebCam_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "WebCam plugin"  \
           `[ $(expr "$CONFIG" : ".*eDRC_plugin.*") -ne 0 ] && echo 'True' ||  echo 'False'` "eDRC plugin"  \
         )
}

#fifthPage()
#{
    #SPEC=`echo ${CONFIG##*;}`
#    RET=$($ZENITY_NO_SIZE --title "$ZENITY_TITLE" --text "Optional: Please enter a spec file, if you want to use one:\n(If not, just leave empty)" --entry)
#}

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
    #SPEC=`echo ${CONFIG##*;}`
    SLEEP_TIME=1
    # Options
    CLEAN="`[ $(expr "$CONFIG" : ".*Clean_build_path.*") -ne 0 ] && echo 'y' || echo 'n'`"
    RUN="`[ $(expr "$CONFIG" : ".*Run_application.*") -ne 0 ] && echo 'y' || echo 'n'`"
    TRANS="`[ $(expr "$CONFIG" : ".*Create_translations.*") -ne 0 ] && echo 'y' || echo 'n'`"
    MAKE_OPTS="`[ $(expr "$CONFIG" : ".*Parallel_build.*") -ne 0 ] && echo '-j4' || echo '-j1'`"
    NOTIFY="`[ $(expr "$CONFIG" : ".*Notify_when_done.*") -ne 0 ] && echo 'y' || echo 'n'`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*Agenda_plugin.*") -ne 0 ] && echo 'agenda '`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*Alerts_plugin.*") -ne 0 ] && echo 'alerts '`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*Feedback_plugin.*") -ne 0 ] && echo 'feedback '`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*PadTools_plugin.*") -ne 0 ] && echo 'pad '`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*PMHx_plugin.*") -ne 0 ] && echo 'pmh '`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*WebCam_plugin.*") -ne 0 ] && echo 'webcam '`"
    REQUESTED_PLUGINS=$REQUESTED_PLUGINS"`[ $(expr "$CONFIG" : ".*eDRC_plugin.*") -ne 0 ] && echo 'edrc '`"
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
    echo "    Optional Plugins: $REQUESTED_PLUGINS"
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
#    fifthPage
#    CONF=$CONF$SEPARATOR$RET

    CONFIG=$CONF
    saveConfig

    zenityBuild
    exit $?
}

#########################################################################################
## Start build process
#########################################################################################
detectQtVersion

# No bundle name provided -> error unless user requested a clear command
if [[ -z $BUILD || -z $LOWERED_BUNDLE_NAME ]]; then
    if [[ -z $CLEAN ]]; then
        if [[ -f $ZENITY ]]; then
            echo "* No command line params: starting zenity configuration"
            RUNNING_ZENITY=1
            testDependencies
            detectSpec
            startZenityDialog
        fi
    else
        # Clean only command
        makeClean
        exit 0
    fi
fi

if [[ -z $TEST ]]; then
    isValidCamelCaseApplication $CAMELCASE_BUNDLE_NAME || {
        echo "Application $CAMELCASE_BUNDLE_NAME does not exists in this project"
        echo "Application $CAMELCASE_BUNDLE_NAME does not exists in this project"
        exit 321;
    }
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
