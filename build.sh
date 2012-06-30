#!/bin/bash

# This script is part of FreeMedForms project : http://www.freemedforms.com
# (c) 2008 - 2012  Eric MAEKER, MD
#
# This script helps on the compilation of the Linux projects
#

BUNDLE_NAME=""
PROJECT_VERSION=""
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi
TEST=""
TRANS=""
CLEAR=""
SPEC=""
BUILD="debug"
QMAKE_CONFIG=""
MAKE_OPTS="-j4"
RUN=""

# get version number of FreeDiams from the project file
PROJECT_VERSION=`cat $SCRIPT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`

detectSpec()
{
    echo "  *** Guessing spec file"
    if [[ $OSTYPE == linux-gnu ]]; then
      SPEC="linux-g++"
      echo "        Linux OS detected; setting qmake spec to "$SPEC
    elif [[ $OSTYPE == darwin* ]]; then
      SPEC="macx-g++"
      echo "        Mac OS detected; setting qmake spec to "$SPEC
    elif [[ $OSTYPE == freebsd ]]; then
      SPEC="linux-g++"
      echo "        FreeBSD OS detected; setting qmake spec to "$SPEC
    else
      echo "   * Unknow OS"
    fi
}

showHelp()
{
echo $SCRIPT_NAME" builds the *debug_without_install* Linux versions of the FreeMedForms applications project."
echo "Project version: "$PROJECT_VERSION
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
echo "  -c  Make clear before compiling"
echo "  -s  Use the specified spec file (default spec: $SPEC)"
echo "  -R  Run application after the build"
echo "  -h  Show this help"
echo
}

makeClear()
{
    if [[ "$CLEAR" != "" ]]; then
        echo "    *** Cleaning build path"
        MAKE_STEP=`make clean && rm -R $SCRIPT_PATH/bin/$BUNDLE_NAME && rm -R $SCRIPT_PATH/build`
        MAKE_STEP=$?
        if [[ ! $MAKE_STEP == 0 ]]; then
            echo "    *** ERROR ***"
            exit 123
        else
            echo "        Cleaned"
        fi
    fi
}

createTranslations()
{
    if [ "$TRANS" != "" ]; then
        echo "    *** Creating translations"
        MAKE_STEP=`lrelease ./global_resources/translations/*.ts`
        MAKE_STEP=$?
        if [[ ! $MAKE_STEP == 0 ]]; then
            echo "    *** ERROR: lrelease step wrong ***"
            exit 123
        else
            echo "        Translations created"
        fi
    fi
}

qmakeCommand()
{
    echo "    *** qmake $BUNDLE_NAME.pro -r $QMAKE_CONFIG $SPEC LOWERED_APPNAME=$BUNDLE_NAME"
    MAKE_STEP=`qmake $BUNDLE_NAME.pro -r $QMAKE_CONFIG $SPEC LOWERED_APPNAME=$BUNDLE_NAME`
    MAKE_STEP=$?
    if [[ ! $MAKE_STEP == 0 ]]; then
        echo "    *** ERROR: qmake step wrong ***"
        exit 123
    else
        echo "        qmake correctly done"
    fi
}

makeCommand()
{
    echo "    *** make $MAKE_OPTS"
    MAKE_STEP=`make $MAKE_OPTS`
    MAKE_STEP=$?
    if [[ ! $MAKE_STEP == 0 ]]; then
        echo "    *** ERROR: make step wrong ***"
        exit 123
    else
        echo "        make correctly done"
    fi
}

makeInstallCommand()
{
    if [[ "$BUILD" != "debug" ]]; then
        echo "    *** make install"
        MAKE_STEP=`make install`
        MAKE_STEP=$?
        if [[ ! $MAKE_STEP == 0 ]]; then
            echo "    *** ERROR: make install step wrong ***"
            exit 123
        else
            echo "        make install correctly done"
        fi
    fi
}

finalMessage()
{
    if [[ $OSTYPE == linux-gnu ]]; then
        echo "*** Start application with:"
        echo "./bin/"$BUNDLE_NAME"/"$BUNDLE_NAME"_debug --config=../global_resources/"$BUNDLE_NAME"_config.ini"
    elif [[ $OSTYPE == darwin* ]]; then
        echo "*** Start application with:"
        echo "./bin/"$BUNDLE_NAME"/"$BUNDLE_NAME"_debug.app/Contents/MacOs/"$BUNDLE_NAME"_debug --config=../../../../../global_resources/"$BUNDLE_NAME"_config.ini"
    elif [[ $OSTYPE == freebsd ]]; then
        echo "*** Start application with:"
        echo "./bin/"$BUNDLE_NAME"/"$BUNDLE_NAME"_debug --config=../global_resources/"$BUNDLE_NAME"_config.ini"
    fi
}

build()
{
    if [[ "$BUNDLE_NAME" == "" ]]; then
    echo "*** WARNING: you must define the application with the -b param. See help -h."
    exit 1;
    fi

    MSG="DEBUG mode"
    if [[ "$BUILD" != "debug" ]]; then
      MSG="RELEASE mode"
    fi
    echo "*** Building application in $MSG: $BUNDLE_NAME ($BUILD) ***"
    if [[ "$BUILD" != "debug" ]]; then
        if [[ $OSTYPE == darwin* ]]; then
            # Switch to mac_release.sh
            cd scripts
            echo "  **** -> switching to: scripts/mac_release.sh -b $BUNDLE_NAME"
            ./mac_release.sh -b $BUNDLE_NAME
            return 0
        fi
    fi


  if [ "$TEST" != "" ]; then
    echo "    *** Building a test application"
    cd ./tests
  fi

  if [[ ! -e $BUNDLE_NAME ]]; then
      echo "    *** ERROR: Application does not exist: $BUNDLE_NAME"
      exit 1
  fi

  makeClear
  createTranslations
  echo "    *** cd $BUNDLE_NAME"
  cd $BUNDLE_NAME
  qmakeCommand
  makeCommand
  makeInstallCommand
  echo "    *** cd $SCRIPT_PATH"
  cd $SCRIPT_PATH
  finalMessage
}

launchApplication()
{
    if [[ "$RUN" == "y" ]]; then
        # Launch the debug application with the good config file
        if [[ "$BUILD" == "debug" ]]; then
            if [[ $OSTYPE == linux-gnu ]]; then
                $SCRIPT_PATH/bin/$BUNDLE_NAME/$BUNDLE_NAME_debug --config=../global_resources/$BUNDLE_NAME_config.ini
            elif [[ $OSTYPE == darwin* ]]; then
                $SCRIPT_PATH/bin/$BUNDLE_NAME/$BUNDLE_NAME_debug.app/Contents/MacOs/$BUNDLE_NAME_debug --config=../../../../../global_resources/$BUNDLE_NAME_config.ini
            elif [[ $OSTYPE == freebsd ]]; then
                $SCRIPT_PATH/bin/$BUNDLE_NAME/$BUNDLE_NAME_debug --config=../global_resources/$BUNDLE_NAME_config.ini
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
    c) CLEAR="y"
    ;;
    s) SPEC=$OPTARG
    ;;
    t) TRANS="y"
    ;;
    b) BUNDLE_NAME=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
  esac
done

if [[  "$BUILD" == "debug" ]]; then
    QMAKE_CONFIG="CONFIG+=debug CONFIG-=release CONFIG+=debug_without_install"
elif [[  "$BUILD" == "release" ]]; then
    QMAKE_CONFIG="CONFIG-=debug CONFIG+=release"
elif [[  "$BUILD" == "linuxintegrated" ]]; then
    QMAKE_CONFIG="CONFIG-=debug CONFIG+=release CONFIG+=LINUX_INTEGRATED"
fi

if [ ! -e $SPEC ]; then
  SPEC="-spec "$SPEC
fi

build
launchApplication
exit 0
