#!/bin/sh
#
# This script is part of the FreeHealth project
# License : GPLv3

echo "****  ****"
PACKDIR=`pwd`"/package"
RELATIVEPACKDIR="package"
VERSION="0.5.0~beta"
# get version of changelog with : dpkg-parsechangelog | awk '/^Version/ { print $2 }'

# Mount MacOS system
if [ -e `ls ~/Programmation`]; then
    sudo ./mountProgrammation
fi

if [ ! -e $PACKDIR ]; then
    mkdir $PACKDIR
else
    rm -Rf $PACKDIR
    mkdir $PACKDIR
fi

SOURCESDIR=$PACKDIR"/freediams-"$VERSION
SOURCESPACK=$PACKDIR"/freediams_"$VERSION".orig.tar.gz"
DEBIANMED_SOURCESPACK="freediams_sources_"$VERSION".tgz"

# Get sources from the Mac
echo "**** RETRIVING SOURCES PACKAGE ****"
sudo ~/Programmation/freemedforms/scripts/fd_sources_packager.sh
cp ~/Programmation/freemedforms/freediams_sources.tgz $SOURCESPACK

# Unpack sources
echo "**** UNPACK SOURCES ****"
rm -Rf $SOURCESDIR
mkdir $SOURCESDIR
tar zxf $SOURCESPACK -C $PACKDIR
cd $SOURCESDIR

# Get debian med files
echo "**** GETTING DEBIAN MED FILES ****"
mkdir $SOURCESDIR"/debian"
svn checkout svn://ericmaeker-guest@svn.debian.org/svn/debian-med/trunk/packages/freediams/trunk/debian $SOURCESDIR"/debian"

# Build package
debuild -k0x3FA0BBEF -S -sa --lintian-opts -i
cd ..
sudo pbuilder build *.dsc > log.txt

DEBFILES="/var/cache/pbuilder/result/freediams*"$VERSION"*.deb"
cp $DEBFILES $PACKDIR
sudo dpkg -r freediams

# Send file to server

exit 0

