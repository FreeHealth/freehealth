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
#
# This script will run the cppcheck program on all the freemedforms code.
# The output files are stored in
#    freemedforms.git/build/codecheck
#
# Script version: 0.1 - 01 July 2013

# Some path definition
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi
SOURCES_ROOT_PATH=$SCRIPT_PATH"/../"

# Get version of the project
PROJECT_VERSION=`cat $SOURCES_ROOT_PATH/buildspecs/projectversion.pri | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '`
GIT_REV=`git rev-parse HEAD`
PROJECTS="freemedforms freediams freedrc"

# Prepare some vars
OUTPUT_PATH="$SOURCES_ROOT_PATH/build/codecheck/$PROJECT_VERSION-$GIT_REV"
CPPCHECK="/usr/bin/cppcheck"
CPPOPTIONS=""
CPPINCLUDES=""
CPPDIRSTOSCAN=""
CPPOUTPUT=""

showHelp()
{
    echo "FreeMedForms Project version: $PROJECT_VERSION"
    echo "$SCRIPT_NAME runs some static code checking of the FreeMedForms source code."
    echo "This script needs the cppcheck application to be installed in $CPPCHECK"
    echo "All code analysis output is stored in"
    echo "    freemedforms.git/build/codecheck/{APPLICATION}/{VERSION}-{GIT_REVISION}"
    echo "Usage: $SCRIPT_NAME -h"
    echo "Options:"
    echo "  -b  Code to test "
    echo "        freemedforms for the FreeMedForms EMR application specific code"
    echo "        freediams    for the FreeDiams application specific code"
    echo "        freedrc      for the FreeDrc application specific code"
    echo "        plugins      for the shared plugins code"
    echo "        libs         for the shared libs code"
    echo "  -h  Show this help"
    echo ""
    # Test if cppcheck is installed on the computer
    if [ ! -f $CPPCHECK ]; then
      echo "ERROR: $CPPCHECK not available. Please install cppcheck";
    fi
    cd $SCRIPT_PATH
}

# This function uses the global following vars to proceed
# CPPOPTIONS
# CPPINCLUDES
# CPPDIRSTOSCAN
# CPPOUTPUT
runCppCheck()
{
    echo "** Running code checking on " $CPPDIRSTOSCAN
    CHECKS="all" # "style performance portability information unusedFunction missingInclude"
    for CHECK in $CHECKS ; do
        echo "$CPPCHECK $CPPOPTIONS --enable=$CHECK $CPPINCLUDES $CPPDIRSTOSCAN 2> $CPPOUTPUT/$CHECK.txt"
        $CPPCHECK $CPPOPTIONS --enable=$CHECK $CPPINCLUDES $CPPDIRSTOSCAN 2> $CPPOUTPUT/$CHECK.txt
    done
}

# first arg is the project to screen 'libs' or 'plugins'
runCppCheckPerSubDir()
{
    SUB=`ls -d $1/*/`
    for SUBPROJECT in $SUB ; do
        # Create output path
        if [ ! -z `mkdir -p "$OUTPUT_PATH/$SUBPROJECT"` ]; then
            echo "ERROR: unable to create path: $OUTPUT_PATH/$SUBPROJECT"
            exit 124;
        fi

        # Prepare cppcheck command line
        CPPOPTIONS="--enable=all -D_linux -D_X86_ -DQ_DECL_IMPORT="
        if [[ "$PRO" == "libs" ]]; then
            CPPINCLUDES="-I $SOURCES_ROOT_PATH/libs"
            CPPOPTIONS=$CPPOPTIONS" --suppress=unusedFunction"
        elif [[ "$PRO" = "plugins" ]]; then
            CPPOPTIONS=$CPPOPTIONS" --suppress=unusedFunction"
            CPPINCLUDES="-I $SOURCES_ROOT_PATH/libs -I $SOURCES_ROOT_PATH/contrib -I $SOURCES_ROOT_PATH/plugins"
        else
            CPPINCLUDES="-I $SOURCES_ROOT_PATH/libs -I $SOURCES_ROOT_PATH/contrib -I $SOURCES_ROOT_PATH/plugins"
        fi
        CPPDIRSTOSCAN=$SUBPROJECT
        CPPOUTPUT="$OUTPUT_PATH/$SUBPROJECT"
        runCppCheck
    done
}

startCppCheck()
{
#        QT_HEADERS=`qmake -query QT_INSTALL_HEADERS`
#        INCLUDES=$INCLUDES" -I $QT_HEADERS"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtCore"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtGui"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtNetwork"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtScript"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtScriptTools"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtSql"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtUiTools"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtXml"
#        INCLUDES=$INCLUDES" -I $QT_HEADERS/QtXmlPatterns"
#        INCLUDES=$INCLUDES" -I ../libs -I ../contrib -I ../plugins -I plugins"
    cd $SOURCES_ROOT_PATH


    for PRO in $PROJECTS ; do
        echo "** Checking project: "$PRO
        if [[ "$PRO" = "libs" || "$PRO" = "plugins" ]]; then
            runCppCheckPerSubDir $PRO
        else
            CPPOPTIONS="--enable=all -D_linux -D_X86_ -DQ_DECL_IMPORT="
            CPPINCLUDES="-I $SOURCES_ROOT_PATH/libs -I $SOURCES_ROOT_PATH/contrib -I $SOURCES_ROOT_PATH/plugins -I plugins"
            CPPDIRSTOSCAN="plugins $PRO-src"
            CPPOUTPUT="$OUTPUT_PATH/$PRO"
            if [ ! -z `mkdir -p $OUTPUT_PATH/$PRO` ]; then                      
                echo "ERROR: unable to create folder: $PRO in path: $OUTPUT_PATH"                           
                exit 124;                                                       
            fi
            runCppCheck
        fi
    done

}

# Check args
while getopts "hb:" option
do
  case $option in
    h) showHelp
       exit 0
    ;;
    b) PROJECTS=$OPTARG
    ;;
  esac
done

# Test if cppcheck is installed on the computer
if [ ! -f $CPPCHECK ]; then
  echo "ERROR: $CPPCHECK not available. Please install cppcheck";
  echo "ERROR: type ./$SCRIPT_NAME -h for help";
  exit 123;
fi

# Create output path
if [ ! -z `mkdir -p $OUTPUT_PATH` ]; then
    echo "ERROR: unable to create path: $OUTPUT_PATH"
    exit 124;
fi

startCppCheck

exit 0
