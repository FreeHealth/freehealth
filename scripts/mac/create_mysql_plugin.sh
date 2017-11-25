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
# This script :
# - downloads the Qt sources
# - compiles the Qt-MySQL plugins
# - installs the plugins
# - clean the Qt sources

QT_VERSION=4.8.5
ACTUAL_PATH=`pwd`
DEFAULT_WORKING_PATH=~/Downloads
TMP_BUILD_PATH=$DEFAULT_WORKING_PATH/qt-src-tmp
MAC_SPEC=macx-clang

# Get scripts names and paths
SCRIPT_NAME=`basename $0`
if [ "`echo $0 | cut -c1`" = "/" ]; then
  SCRIPT_PATH=`dirname $0`
else
  SCRIPT_PATH=`pwd`/`echo $0 | sed -e s/$SCRIPT_NAME//`
fi

showHelp()
{
    echo $SCRIPT_NAME" builds the Qt MySQL plugins for MacOS."
    echo "Usage : $SCRIPT_NAME -w /working/path/"
    echo "Options :"
    echo "  -w  Define the working path (by default: $DEFAULT_WORKING_PATH)"
    echo "  -q  Qt version (by default: $QT_VERSION)"
    echo "  -s  spec file to use (by default: $MAC_SPEC)"
    echo "  -h  show this help"
}

# This script assumes that MySQL is installed with header files
# Get it from : http://dev.mysql.com/downloads/mysql/
# March 2014 using:
#     http://cdn.mysql.com/Downloads/MySQL-5.6/mysql-5.6.16-osx10.7-x86_64.dmg
MYSQL_SOURCES=/usr/local/mysql/include
MYSQL_LIB=/usr/local/mysql/lib
MYSQL_LIB_VERSION=18

checkMySQLInstallation()
{
    if [ ! -d "$MYSQL_SOURCES" ]; then
        echo "/!\ MySQL headers not detected. Path: $MYSQL_SOURCES"
        echo "Can not proceed. Please install MySQL on this computer"
        exit 123;
    fi

    if [ ! -d "$MYSQL_LIB" ]; then
        echo "/!\  MySQL libs not detected. Path: $MYSQL_LIB"
        echo "Can not proceed. Please install MySQL on this computer"
        exit 456;
    fi
}

# Download the Qt source to use for the MySQL plugin compilation
downloadQtSource()
{
    # URL = ftp://ftp.mirrorservice.org/sites/download.qt-project.org/official_releases/qt/5.2/5.2.1/single/qt-everywhere-opensource-src-5.2.1.tar.gz
    QT_VERSION_MAIN=`echo $QT_VERSION | cut -d"." -f1,2`

    # We are using a UK FTP mirror, you can manage your own here
    MIRROR=ftp://ftp.mirrorservice.org/sites # use "http:/" to download from the qt-project HTTP official web site
    URL=$MIRROR/download.qt-project.org/official_releases/qt/$QT_VERSION_MAIN/$QT_VERSION/single/qt-everywhere-opensource-src-$QT_VERSION.tar.gz

    echo "Downloading Qt sources:"
    echo "   mirror: $MIRROR"
    echo " full URL: $URL"
    echo "       to: $TARGZ_FILE"
    if [ ! -e $TARGZ_FILE ] ; then
        echo "curl $URL -o $TARGZ_FILE"
        STEP=`curl $URL -o $TARGZ_FILE`
        STEP=$?
        if [[ ! $STEP == 0 ]]; then
            return 123
       fi
    fi
}

# Unzip Qt source's needed files for the MySQL plugin compilation
unzipQtSource()
{
    # tar xzvf $TARGZ_FILE -C ~/Downloads/qt-src-tmp
    if [ ! -e $TMP_BUILD_PATH ] ; then
        echo "Creating temporary path : $TMP_BUILD_PATH"
        mkdir $TMP_BUILD_PATH
    fi
    echo "Unzipping Qt sources in temporary path : $TMP_BUILD_PATH"
    tar xzvf $TARGZ_FILE -C $TMP_BUILD_PATH \
        qt-everywhere-opensource-src-$QT_VERSION/qtbase/src/plugins/sqldrivers/mysql \
        qt-everywhere-opensource-src-$QT_VERSION/qtbase/src/sql/drivers/mysql \
        qt-everywhere-opensource-src-$QT_VERSION/qtbase/.qmake.conf \
        qt-everywhere-opensource-src-$QT_VERSION/qtbase/src/*.pri
        #        qt-everywhere-opensource-src-$QT_VERSION/src/plugins \
}

# Build and install Qt MySQL plugin
buildQtMySQLPlugin()
{
    PLUG_SRC=$TMP_BUILD_PATH/qt*/qtbase/src/plugins/sqldrivers/mysql
    echo "Compiling Qt MySQL plugin"
    cd $PLUG_SRC
    echo "    Moving to: `pwd` "
    echo "    qmake -spec $MAC_SPEC \"LIBS+=-L$MYSQL_LIB -lmysqlclient_r\" \"INCLUDEPATH+=$MYSQL_SOURCES\" \"CONFIG+=build_all\" "
    qmake -spec $MAC_SPEC "LIBS+=-L\"$MYSQL_LIB\" -lmysqlclient_r" "INCLUDEPATH+=\"$MYSQL_SOURCES\"" "CONFIG+=build_all"
    make
    make install

    # install_name_tool the plugin
    echo "Linking to MySQL lib"
    QT_PLUGIN_PATH=`qmake -query QT_INSTALL_PLUGINS`
    install_name_tool -change \
                        libmysqlclient.$MYSQL_LIB_VERSION.dylib \
                        $MYSQL_LIB/libmysqlclient.$MYSQL_LIB_VERSION.dylib \
                        $QT_PLUGIN_PATH/sqldrivers/libqsqlmysql.dylib
}

# Clear all downloaded, extracted and compiled files
clearFiles()
{
    rm -rf $TMP_BUILD_PATH
    rm $TARGZ_FILE
}

while getopts "w:hq:" option
do
    case $option in
        h) showHelp
            exit 0
        ;;
        q) QT_VERSION=$OPTARG;
        ;;
        w) DEFAULT_WORKING_PATH=$OPTARG;
        ;;
    esac
done

TMP_BUILD_PATH=$DEFAULT_WORKING_PATH/qt-src-tmp
TARGZ_FILE=$DEFAULT_WORKING_PATH/qt-everywhere-opensource-src-$QT_VERSION.tar.gz

checkMySQLInstallation
downloadQtSource
unzipQtSource
buildQtMySQLPlugin
clearFiles

exit 0
