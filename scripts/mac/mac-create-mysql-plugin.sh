#!/bin/sh
# This file is part of the FreeMedForms project
# (c) 2008-2010 Eric Maeker, MD
# License : New BSD

# This script :
# - downloads the Qt sources
# - compiles the Qt-MySQL plugins
# - installs the plugins
# - clean the Qt sources

QT_VERSION=4.8.1
ACTUAL_PATH=`pwd`

# This script assumes that MySQL is installed with header files
# Get it from : http://dev.mysql.com/downloads/mysql/
# Get mysql binairies (adapt mirror)
#    http://dev.mysql.com/get/Downloads/MySQL-5.5/mysql-5.5.11-osx10.6-x86.tar.gz/from/http://mirrors.ircam.fr/pub/mysql/
#    http://dev.mysql.com/get/Downloads/MySQL-5.5/mysql-5.5.11-osx10.6-x86_64.tar.gz/from/http://mirrors.ircam.fr/pub/mysql/
MYSQL_SOURCES=/usr/local/mysql/include
MYSQL_LIB=/usr/local/mysql/lib

# Download Qt sources
# curl http://get.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.6.2.tar.gz -o ~/Downloads/qt-everywhere-opensource-src-4.6.2.tar.gz -s
echo "Checking/Downloading Qt sources : ~/Downloads/qt-everywhere-opensource-src-$QT_VERSION.tar.gz"
TARGZ_FILE=~/Downloads/qt-everywhere-opensource-src-$QT_VERSION.tar.gz
if [ ! -e $TARGZ_FILE ] ; then
    echo "Download Qt sources"
    curl http://get.qt.nokia.com/qt/source/qt-everywhere-opensource-src-$QT_VERSION.tar.gz -o $TARGZ_FILE
fi

# Unzip package into a temporary dir
# tar xzvf ~/Downloads/qt-everywhere-opensource-src-$QT_VERSION.tar.gz -C ~/Downloads/qt-src-tmp
TMP_DIR=~/Downloads/qt-src-tmp
echo "Unzipping Qt sources in temporary path : $TMP_DIR"
if [ ! -e $TMP_DIR ] ; then
    echo "Creating temporary path : $TMP_DIR"
    mkdir $TMP_DIR
fi
tar xzvf $TARGZ_FILE -C $TMP_DIR qt-everywhere-opensource-src-$QT_VERSION/src/sql qt-everywhere-opensource-src-$QT_VERSION/src/plugins qt-everywhere-opensource-src-$QT_VERSION/*.pri

# Compile Qt sources
PLUG_SRC=$TMP_DIR/qt*/src/plugins/sqldrivers/mysql
echo "Compiling Qt MySQL plugin"

# cd ~/Dow*/qt*/qt*/src/plugins/sqldrivers/mysql
cd $PLUG_SRC
# qmake -spec macx-g++ LIBS+="-L\"/usr/local/mysql/lib\" -lmysqlclient" INCLUDEPATH+="\"/usr/local/mysql/include\""
qmake -spec macx-g++ LIBS+="-L\"$MYSQL_LIB\" -lmysqlclient_r" INCLUDEPATH+="\"$MYSQL_SOURCES\"" CONFIG+=build_all
make
make install

# name tool the plugin
# install_name_tool -change libmysqlclient.18.dylib /usr/local/mysql/lib/libmysqlclient.18.dylib /Users/eric/QtSDK/Desktop/Qt/474/gcc/plugins/sqldrivers/libqsqlmysql.dylib
echo "Linking to MySQL lib"
QT_PLUGIN_PATH=`qmake -query QT_INSTALL_PLUGINS`
install_name_tool -change \
                    libmysqlclient.18.dylib \
                    $MYSQL_LIB/libmysqlclient.18.dylib \
                    $QT_PLUGIN_PATH/sqldrivers/libqsqlmysql.dylib

echo "Removing temporary path"
cd $ACTUAL_PATH
rm -R $TMP_DIR

exit 0
