#!/bin/sh
#
# This script is part of the FreeMedForms project
# License : GPLv3
# Create the Launchpad PPA package for one specific application
# FreeMedForms launchpad team: https://launchpad.net/~freemedforms
# In order to use this script, you need to have the following packages installed:
# subversion svn-buildpackage

SCRIPT_VERSION=0.9.0

PACKDIR="~/ppa_"
APP_VERSION=""
APP_NAME=""
SOURCEDIR=""
SOURCEPACK_FULLPATH=""
DOWNLOAD_URL=""
DOWNLOAD_FILENAME=""
UBUNTU_RELEASE_NAME=""
PGP_KEY="0x75D4AE85B9520933"      # Long ID of Eric Maeker new OpenPGP key (default key)
WGET_NOPROXY=""
DEBUILD_SOURCE="-sa"
PPA_VERSION="1"
DPUT_ARGS=""
SERIES="precise trusty utopic"
DEBUILD_OPTIONS=""

showHelp()
{
  #hb:v:k:
  SCRIPT_NAME=`basename $0`
  echo $SCRIPT_NAME" "$SCRIPT_VERSION" creates and sends package to the FreeMedForms LaunchPad PPA"
  echo "Usage: "$SCRIPT_NAME" -h -b application -v applicationVersion [-k pgpKeyToUse -p 2] -r \"oneiric maverick\" "
  echo
  echo "   -h           show this help"
  echo "   -b app       Application name"
  echo "   -v version   Application version"
  echo "   -k pgpkey    Use this specific key to sign the package (indicate key long ID starting with 0x)"
  echo "   -p ppaver    PPA subversion (pack name -> app-appservsion-natty-ppaver)"
  echo "   -s           Do not include source to package for uploading"
  echo "   -n           Do not use proxy when downloading"
  echo "   -r           Specify the Series to build (precise trusty utopic)"
  echo "   -f           Use dput -f to upload packages"
  echo "   -d           Don't execute dpkg-checkbuilddeps (like debuild -d)"
  echo
}


# Create the PPA working path using the $PACKDIR variable
createWorkingDir()
{
  echo "    * Creating working path: $PACKDIR"
  if [ ! -e $PACKDIR ]; then
      mkdir -p $PACKDIR
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
    STEP=`svn checkout svn://svn.debian.org/svn/debian-med/trunk/packages/$APP_NAME/trunk/debian $SOURCEDIR"/debian"`
    STEP=$?
    if [ ! $STEP = 0 ]; then
      echo "   *** Error: Unable to checkout Debian Med SVN ***"
      exit 123
    else
      echo "      Done"
    fi
    cp $SOURCEDIR"/debian/changelog" $PACKDIR"/changelog.bkup"
    echo 'cp $SOURCEDIR"/debian/control" $PACKDIR"/control.bkup"'
  fi
}

# Build source package
# Uses $PGP_KEY, $SOURCEDIR
buildSourcePackage()
{
  cd $SOURCEDIR
  echo "    * Building DSC file: debuild -k$PGP_KEY -S $DEBUILD_SOURCE --lintian-opts -i $DEBUILD_OPTIONS"
  debuild -k$PGP_KEY -S $DEBUILD_SOURCE --lintian-opts -i $DEBUILD_OPTIONS
  # sudo pbuilder build *.dsc > log.txt
}

# upload to PPA
# Uses $APP_NAME, $UBUNTU_RELEASE_NAME, $PACKDIR
uploadToPPA()
{
  echo "    * Uploading to PPA: "$UBUNTU_RELEASE_NAME
  cd $PACKDIR"/build-area"
  echo `pwd`
  if [ "$APP_NAME" = "libquazip" ]; then
    echo "      ppa:freemedforms/libquazip"
    dput $DPUT_ARGS ppa:freemedforms/libquazip $APP_NAME"_"$APP_VERSION"-"$UBUNTU_RELEASE_NAME$PPA_VERSION"_source.changes"
  else
    echo "      ppa:freemedforms/ppa"
    dput $DPUT_ARGS ppa:freemedforms/ppa $APP_NAME"_"$APP_VERSION"-"$UBUNTU_RELEASE_NAME$PPA_VERSION"_source.changes"
  fi
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

# For all distribs, change the debhelper dependency to 8.0 instead of the latest one
changeToDebHelper8()
{
  rm $SOURCEDIR"/debian/control"
  cp $PACKDIR"/control.bkup" $SOURCEDIR"/debian/control"
  if [ "$UBUNTU_RELEASE_NAME" = "precise" ]; then
    echo "9" > $SOURCEDIR"/debian/compat"
    return 0;
  fi
  echo "    * Patching debhelper dependency to 8.0, ubuntu "$UBUNTU_RELEASE_NAME
  sed -i "s/debhelper (>= 9)/debhelper (>= 8)/" $SOURCEDIR"/debian/control"
  rm $SOURCEDIR"/debian/compat"
  echo "8" > $SOURCEDIR"/debian/compat"
}

# According to the currently building ubuntu distro,
# adapt the debian/{control,rules} to use the correct version 
# of package dependencies (Build, Require...).
checkDependenciesVersion()
{
  CONTROL_FILE=$SOURCEDIR"/debian/control"
  RULES_FILE=$SOURCEDIR"/debian/rules"

  # Manage QuaZip (experimetal)
  if [ "$UBUNTU_RELEASE_NAME" = "precise" ]; then
    echo "    * Patching debian/{control,rules} for libquazip"
    sed -i "s/libopencv-objdetect-dev (>= 2.3),/libopencv-objdetect-dev (>= 2.3)/" $CONTROL_FILE
    sed -i "/libquazip0-dev (>= 0.4.4)/d" $CONTROL_FILE
    sed -i "s/\"CONFIG+=dontbuildquazip\"//" $RULES_FILE
  fi

  # //OLD// Manage OpenCV lib: precise/quantal=2.3 ; raring/saucy/trusty=2.4
  #if [ "$UBUNTU_RELEASE_NAME" = "precise" ]||[ "$UBUNTU_RELEASE_NAME" = "quantal" ]; then
  #  echo "    * Patching debian/{control,rules} for opencv"
  #  sed -i "s/libopencv-core2.3 (>= 2.3)/libopencv-core2.4 (>= 2.4)/" $CONTROL_FILE
  #  sed -i "s/libopencv-highgui2.3 (>= 2.3)/libopencv-highgui2.4 (>= 2.4)/" $CONTROL_FILE
  #  sed -i "s/libopencv-objdetect2.3 (>= 2.3)/libopencv-objdetect2.4 (>= 2.4)/" $CONTROL_FILE
  #fi
}

# prepare source package using svn-buildpackage
svnBuildPackage()
{
  cd $PACKDIR
  echo "    * Fetching svn debian files"
  echo "      from Debian Med"
   svn checkout svn://svn.debian.org/svn/debian-med/trunk/packages/$APP_NAME/ ./
  #echo "       from FreeMedForms project"
  #svn checkout https://freemedforms.googlecode.com/svn/trunk/buildspecs/debian/freemedforms-project ./
  cp "./trunk/debian/changelog" $PACKDIR"/changelog.bkup"
  cp "./trunk/debian/control" $PACKDIR"/control.bkup"
  SOURCEDIR=$PACKDIR"/trunk"
  changeToDebHelper8
  patchChangelog
  checkDependenciesVersion
  cd $PACKDIR"/trunk"
  echo "    * Building DSC file: svn-buildpackage --svn-download-orig -k$PGP_KEY -S $DEBUILD_SOURCE --svn-ignore"
  svn-buildpackage --svn-download-orig -k$PGP_KEY -S $DEBUILD_SOURCE --svn-ignore $DEBUILD_OPTIONS
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hb:v:k:p:r:nsd" option
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
    r) SERIES=$OPTARG
    ;;
    f) DPUT_ARGS="-f"
    ;;
    d) DEBUILD_OPTIONS=$DEBUILD_OPTIONS" -d"
    ;;
  esac
done

if [ ! -n "$APP_NAME" ]; then
  echo "Error: you must specify an application name"
  exit 1
fi

if [ ! -n "$APP_VERSION" ]; then
  echo "Error: you must specify an application version"
  exit 2
fi

if [ -e $PGP_KEY ]; then
  echo "Error: you must specify an OpenPGP key to sign the package"
  exit 3
fi

for u in $SERIES
do
  # Compute variables
  UBUNTU_RELEASE_NAME=$u
  PACKDIR=`pwd`"/ppa_"$APP_NAME"_"$APP_VERSION"/"$u
  SOURCEDIR=$PACKDIR"/"$APP_NAME"-"$APP_VERSION
  SOURCEPACK_FULLPATH=$PACKDIR"/"$APP_NAME"_"$APP_VERSION".orig.tar.gz"

  echo "*** Processing "$APP_NAME" "$APP_VERSION
  echo "    * Source package: "$SOURCEPACK_FULLPATH
  echo "    * Source path: "$SOURCEDIR
  createWorkingDir
  svnBuildPackage
  uploadToPPA
done

exit 0
