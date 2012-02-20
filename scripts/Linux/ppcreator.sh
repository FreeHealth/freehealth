#!/bin/sh
#
# This script is part of the FreeMedForms project
# License : GPLv3
# Create the Launchpad PPA package for one specific application

SCRIPT_VERSION=0.6.0

PACKDIR="~/ppa_"
APP_VERSION=""
APP_NAME=""
SOURCEDIR=""
SOURCEPACK_FULLPATH=""
DOWNLOAD_URL=""
DOWNLOAD_FILENAME=""
UBUNTU_RELEASE_NAME=""
PGP_KEY="0x3FA0BBEF"      # Eric's key by default, you have to know the phrase
WGET_NOPROXY=""
DEBUILD_SOURCE="-sa"
PPA_VERSION="1"

showHelp()
{
  #hb:v:k:
  SCRIPT_NAME=`basename $0`
  echo $SCRIPT_NAME" "$SCRIPT_VERSION" creates and sends package to the FreeMedForms LaunchPad PPA"
  echo
  echo "Usage: "$SCRIPT_NAME" -h -b application -v applicationVersion [-k pgpKeyToUse -p 2] "
  echo
  echo "   -h           show this help"
  echo "   -b app       Application name"
  echo "   -v version   Application version"
  echo "   -k pgpkey    Use this specific key to sign the package"
  echo "   -p ppaver    PPA subversion (pack name -> app-appservsion-natty-ppaver)"
  echo "   -s           Do not include source to package for uploading"
  echo "   -n           Do not use proxy when downloading"
  echo
}


# Create the PPA working path using the $PACKDIR variable
createWorkingDir()
{
  if [ ! -e $PACKDIR ]; then
      mkdir $PACKDIR
  fi
}

# Download source
# Uses $SOURCEPACK_FULLPATH, $PACKDIR, $APP_NAME, $APP_VERSION, $PACKDIR, $DOWNLOAD_URL, $DOWNLOAD_FILENAME
downloadAndUnpackSource()
{
  if [ ! -e $SOURCEPACK_FULLPATH ]; then
    echo "    * Downloading source package: "$DOWNLOAD_URL
    echo "    * To: "$SOURCEPACK_FULLPATH
    cd $PACKDIR
    wget $WGET_NOPROXY $DOWNLOAD_URL
    mv $DOWNLOAD_FILENAME $SOURCEPACK_FULLPATH
  fi
  if [ ! -e $SOURCEDIR ]; then
    echo "    * Unpackaging source package to: "$PACKDIR
    tar xzvf $SOURCEPACK_FULLPATH -C $PACKDIR
  fi
}

# Assuming the Debian Med files are totally updated to the correct version
# Uses $SOURCEDIR, $APP_NAME
downloadDebianMedFiles()
{
  # Get debian med files
  if [ ! -e $SOURCEDIR"/debian" ]; then
    mkdir $SOURCEDIR"/debian"
    echo "    * Downloading Debian Med files"
    svn checkout svn://svn.debian.org/svn/debian-med/trunk/packages/$APP_NAME/trunk/debian $SOURCEDIR"/debian"
    cp $SOURCEDIR"/debian/changelog" $PACKDIR"/changelog.bkup"
  fi
}

# Build source package
# Uses $PGP_KEY, $SOURCEDIR
buildSourcePackage()
{
  cd $SOURCEDIR
  echo "    * Building DSC file: debuild -k$PGP_KEY -S $DEBUILD_SOURCE --lintian-opts -i"
  debuild -k$PGP_KEY -S $DEBUILD_SOURCE --lintian-opts -i
  # sudo pbuilder build *.dsc > log.txt
}

# upload to PPA
# Uses $APP_NAME, $UBUNTU_RELEASE_NAME, $PACKDIR
uploadToPPA()
{
  echo "    * Uploading to PPA: "$UBUNTU_RELEASE_NAME
  cd $PACKDIR
  echo `pwd`
  dput ppa:freemedforms/ppa $APP_NAME"_"$APP_VERSION"-"$UBUNTU_RELEASE_NAME$PPA_VERSION"_source.changes"
}

# patch changelog
# Uses $APP_NAME, $UBUNTU_RELEASE_NAME, $PACKDIR
patchChangelog()
{
  echo "    * Patching changelog for: "$UBUNTU_RELEASE_NAME
  rm $SOURCEDIR"/debian/changelog"
  cp $PACKDIR"/changelog.bkup" $SOURCEDIR"/debian/changelog"
  
  INSERT=$APP_NAME" ("$APP_VERSION"-"$UBUNTU_RELEASE_NAME$PPA_VERSION") "$UBUNTU_RELEASE_NAME"; urgency=low"

  #sed -i "1s/.*/$REPLACE/" $SOURCEDIR"/debian/changelog"

  echo $INSERT > $SOURCEDIR"/debian/changelog"
  echo "\n  * New upstream" >> $SOURCEDIR"/debian/changelog"
  echo "\n -- Eric Maeker <eric.maeker@gmail.com>  "`date -R`"\n" >> $SOURCEDIR"/debian/changelog"
  cat $PACKDIR"/changelog.bkup" >> $SOURCEDIR"/debian/changelog"

}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hb:v:k:p:ns" option
do
  case $option in
    h) showHelp
       exit 0
    ;;
    b) APP_NAME=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
    ;;
    v) APP_VERSION=`echo "$OPTARG" | sed y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/`
    ;;
    k) PGP_KEY=$OPTARG
    ;;
    n) WGET_NOPROXY="--no-proxy"
    ;;
    s) DEBUILD_SOURCE=""
    ;;
    p) PPA_VERSION=$OPTARG
    ;;
  esac
done

if [ -e $APP_NAME ]; then
  echo "Error: you must specify an application name"
  exit 1
fi

if [ -e $APP_VERSION ]; then
  echo "Error: you must specify an application version"
  exit 2
fi

if [ -e $PGP_KEY ]; then
  echo "Error: you must specify a PGP Key to sign the package"
  exit 3
fi

# Compute variables
PACKDIR=`pwd`"/ppa_"$APP_NAME"_"$APP_VERSION
SOURCEDIR=$PACKDIR"/"$APP_NAME"-"$APP_VERSION
SOURCEPACK_FULLPATH=$PACKDIR"/"$APP_NAME"_"$APP_VERSION".orig.tar.gz"
DOWNLOAD_URL="http://freemedforms.googlecode.com/files/"$APP_NAME"fullsources-"$APP_VERSION".tgz"
DOWNLOAD_FILENAME=$APP_NAME"fullsources-"$APP_VERSION".tgz"

echo "*** Processing "$APP_NAME" "$APP_VERSION
echo "    * Source package: "$SOURCEPACK_FULLPATH
echo "    * Source path: "$SOURCEDIR

# build debhelper v8+ package
UBUNTU_RELEASE_NAME="maverick natty oneiric" #precise

createWorkingDir
downloadAndUnpackSource
downloadDebianMedFiles
UBUNTU="maverick natty oneiric"
for u in $UBUNTU
do
  UBUNTU_RELEASE_NAME=$u
  patchChangelog  
  buildSourcePackage
  uploadToPPA
done


exit 0

