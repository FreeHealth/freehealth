#!/bin/sh
#
# This script is part of the FreeMedForms project
# License : GPLv3
# Create the Launchpad PPA package for one specific application

SCRIPT_VERSION=0.9.0

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
DPUT_ARGS=""
SERIES="precise quantal raring"

showHelp()
{
  #hb:v:k:
  SCRIPT_NAME=`basename $0`
  echo $SCRIPT_NAME" "$SCRIPT_VERSION" creates and sends package to the FreeMedForms LaunchPad PPA"
  echo
  echo "Usage: "$SCRIPT_NAME" -h -b application -v applicationVersion [-k pgpKeyToUse -p 2] -r \"oneiric maverick\" "
  echo
  echo "   -h           show this help"
  echo "   -b app       Application name"
  echo "   -v version   Application version"
  echo "   -k pgpkey    Use this specific key to sign the package"
  echo "   -p ppaver    PPA subversion (pack name -> app-appservsion-natty-ppaver)"
  echo "   -s           Do not include source to package for uploading"
  echo "   -n           Do not use proxy when downloading"
  echo "   -r           Specify the Series to build (oneiric maverick natty lucid)"
  echo "   -f           Use dput -f to upload packages"
  echo
}


# Create the PPA working path using the $PACKDIR variable
createWorkingDir()
{
  if [ ! -e $PACKDIR ]; then
      mkdir $PACKDIR
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
    echo 'cp $SOURCEDIR"/debian/control" $PACKDIR"/control.bkup"'
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
# adapt the debian/{control,rules} to use the internal libquazip or
# the system lib.
checkQuazip()
{
  if [ "$UBUNTU_RELEASE_NAME" = "precise" ]; then
    echo "    * Patching debian/{control,rules} for libquazip"
    # Build internal libquazip
    CONTROL_FILE=$SOURCEDIR"/debian/control"
    RULES_FILE=$SOURCEDIR"/debian/rules"
    # Remove build dependency in the control file
    sed -i "s/Build-Depends: libquazip0-dev (>= 0.4.4)/# Removed libquazip0 build dependency/" $CONTROL_FILE
    # Update rules
    sed -i "s/\"CONFIG+=dontbuildquazip\"//" $RULES_FILE
    return 0;
  fi
  # Use system lib: keep control|rules unchanged
}

# prepare source package using svn-buildpackage
svnBuildPackage()
{
  cd $PACKDIR
  echo "    * Fecthing svn debian files"
  echo "      from Debian Med"
   svn checkout svn://svn.debian.org/svn/debian-med/trunk/packages/$APP_NAME/ ./
  #echo "       from FreeMedForms project"
  #svn checkout https://freemedforms.googlecode.com/svn/trunk/buildspecs/debian/freemedforms-project ./
  cp "./trunk/debian/changelog" $PACKDIR"/changelog.bkup"
  cp "./trunk/debian/control" $PACKDIR"/control.bkup"
  SOURCEDIR=$PACKDIR"/trunk"
  changeToDebHelper8
  patchChangelog
  checkQuazip
  cd $PACKDIR"/trunk"
  echo "    * Building DSC file: svn-buildpackage --svn-download-orig -k$PGP_KEY -S $DEBUILD_SOURCE --svn-ignore"
  svn-buildpackage --svn-download-orig -k$PGP_KEY -S $DEBUILD_SOURCE --svn-ignore
}

#########################################################################################
## Analyse options
#########################################################################################
while getopts "hb:v:k:p:r:ns" option
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
  echo "Error: you must specify a PGP Key to sign the package"
  exit 3
fi

# Compute variables
PACKDIR=`pwd`"/ppa_"$APP_NAME"_"$APP_VERSION
SOURCEDIR=$PACKDIR"/"$APP_NAME"-"$APP_VERSION
SOURCEPACK_FULLPATH=$PACKDIR"/"$APP_NAME"_"$APP_VERSION".orig.tar.gz"

echo "*** Processing "$APP_NAME" "$APP_VERSION
echo "    * Source package: "$SOURCEPACK_FULLPATH
echo "    * Source path: "$SOURCEDIR

createWorkingDir
for u in $SERIES
do
  UBUNTU_RELEASE_NAME=$u
  svnBuildPackage
  uploadToPPA
done


exit 0
