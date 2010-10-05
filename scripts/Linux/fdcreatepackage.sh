#!/bin/sh
#
# This script is part of the FreeMedForms project
# License : GPLv3
# Self usage script for creation of the Debian packages from my Mac
# Using VirtualBox sharedFolder


PACKDIR=`pwd`"/package"
RELATIVEPACKDIR="package"
VERSION="0.5.0beta"

# Mount MacOS system
if [ -e `ls ~/Programmation`]; then
    sudo ./mountProgrammation
fi

if [ ! -e $PACKDIR ]; then
    mkdir $PACKDIR
#else
#    rm -Rf $PACKDIR
#    mkdir $PACKDIR
fi

SOURCESDIR=$PACKDIR"/freediams-"$VERSION
SOURCESPACK=$PACKDIR"/freediams_"$VERSION".orig.tar.gz"
DEBIANMED_SOURCESPACK="freediams_sources_"$VERSION".tgz"

# Get sources from the Mac
#sudo ~/Programmation/freemedforms/scripts/fd_sources_packager.sh
#cp ~/Programmation/freemedforms/freediams_sources.tgz $SOURCESPACK

# Unpack sources
#rm -Rf $SOURCESDIR
#mkdir $SOURCESDIR
#tar zxvf $SOURCESPACK -C $SOURCESDIR
#cd $SOURCESDIR

# Get debian med files
#mkdir $SOURCESDIR"/debian"
#svn checkout svn://ericmaeker-guest@svn.debian.org/svn/debian-med/trunk/packages/freediams/trunk/debian $SOURCESDIR"/debian"

# Build package
#debuild -k0x3FA0BBEF -S -sa --lintian-opts -i
#cd ..
#sudo pbuilder build *.dsc > log.txt

DEBFILES="/var/cache/pbuilder/result/freediams*"$VERSION"*.deb"
cp $DEBFILES $PACKDIR
sudo dpkg -r freediams

# Create a tar.gz file for Debian-Med Team
mv $SOURCESPACK $PACKDIR"/"$DEBIANMED_SOURCESPACK
cd package
tar czvf $PACKDIR"/freediamsfullsources_"$VERSION".tgz" \
freediams*.dsc \
freediams*.build \
freediams*.changes \
$DEBIANMED_SOURCESPACK

# Send file to server

exit 0

