#!/bin/bash
#
# This script is part of the FreeMedForms project
#
# License: GNU General Public License version 3 or later
#
# This script will download and "manually" install FreeMedForms 0.9.4
# from FreeMedForms PPA for Ubuntu 14.04 Trusty.
#
# If a .deb file is already present in the download folder, it will
# not be downloaded again.
#
# This script can also uninstall all FreeMedForms packages.
#
# It was written for users who don't have easy access to internet and
# cannot use the usual PPA installation process.
#
# You should give execution mode to file before use:
# sudo chmod +x script_name.sh
#
# Troubleshooting
# If script doesn't download anything, maybe the ppa version number
# has changed: modify trusty1 to trusty2 or trusty3 etc in deb_packages array.
#
# TO DO
# Modify to allow download and installation for both Precise and Trusty.
# Modify to allow download and installation of future FMF versions.
#
# Written by
# Jerome Pinguet <jerome@jerome.cc>
# GPG key 0x14B7E62420E51038
# for the FreeMedForms project
#


DIR="freemedforms-project-0.9.4-trusty1"
PPA_URL="https://launchpad.net/~freemedforms/+archive/ubuntu/ppa/+files/"

# list of deb packages to be downloaded and installed
# the order is important for dpkg dependencies!

deb_packages=( \
    "freemedforms-theme_0.9.4-trusty1_all.deb" \
    "freemedforms-i18n_0.9.4-trusty1_all.deb" \
    "freemedforms-common-resources_0.9.4-trusty1_all.deb" \
    "freemedforms-emr-doc-en_0.9.4-trusty1_all.deb" \
    "freemedforms-emr-doc-fr_0.9.4-trusty1_all.deb" \
    "freemedforms-emr-resources_0.9.4-trusty1_all.deb" \
    "freemedforms-freedata_0.9.4-trusty1_all.deb" \
    "freemedforms-libs_0.9.4-trusty1_amd64.deb" \
    "freemedforms-libs_0.9.4-trusty1_i386.deb" \
    "freemedforms-emr_0.9.4-trusty1_amd64.deb" \
    "freemedforms-emr_0.9.4-trusty1_i386.deb"
    )

# list of packages to be removed
# order is important for dpkg dependencies!

packages=( \
    "freemedforms-emr" \
    "freemedforms-libs" \
    "freemedforms-freedata" \
    "freemedforms-emr-resources" \
    "freemedforms-emr-doc-en" \
    "freemedforms-emr-doc-fr" \
    "freemedforms-common-resources" \
    "freemedforms-theme" \
    "freemedforms-i18n" \
    )

SUBSTRING32="i386"
SUBSTRING64="amd64"

makedir()
{
if [ ! -d "$DIR" ];then
    mkdir $DIR
fi
}

download()
{
for i in "${deb_packages[@]}"
do
    FILE=$DIR"/"$i
    if [ -f $FILE ]; then
        echo "File "$i" already present in directory "$DIR", skipping download."
        continue
    else
        echo "Downloading package "$i"\n"
        wget -P $DIR $PPA_URL$i
    fi
done
}

showHelp()
{
    SCRIPT_NAME=`basename $0`
    echo $SCRIPT_NAME" can download all FreeMedForms EMR (not FreeDiams)" \
    "packages from PPA once and manually install 32 or 64 bit version with dpkg -i"
    echo "It can also remove all package using the -u flag."
    echo "Enjoy!"
    echo
    echo "Usage: sudo "$SCRIPT_NAME "-i version"
    echo
    echo "    -h            print this help"
    echo "    -i version    install 32 or 64 bit version"
    echo "    -u            uninstall all packages (both 32  and 64)"
    echo
    echo "Example: sudo "$SCRIPT_NAME" -i 32    (installs the 32 bit version)"
    echo
}

uninstall()
{
echo "Uninstalling all FreeMedForms 0.9.4 packages."
for i in "${packages[@]}"
do
        echo "Removing package "$i"\n"
        sudo dpkg --remove $i
done
echo "All FreeMedForms 0.9.4 packages are now uninstalled."
}


install32()
{
echo "Entering installation of FreeMedForms 0.9.4 32 bit"
for i in "${deb_packages[@]}"
do
    if [[ "$i" =~ "$SUBSTRING64" ]]; then
        continue
    else
        echo "Installing package "$i"\n"
        sudo dpkg -install $DIR"/"$i
    fi
done
echo "Installed FreeMedForms 0.9.4 32 bit version"
}


install64()
{
echo "Entering installation of FreeMedForms 0.9.4 64 bit"
for i in "${deb_packages[@]}"
do
    if [[ "$i" =~ "$SUBSTRING32" ]];then
        continue
    else
        echo "Installing package "$i"\n"
        sudo dpkg --install $DIR"/"$i
    fi
done
echo "Installed FreeMedForms 0.9.4 64 bit version"
}

while getopts "hi:u" option
do
    case $option in
    h) showHelp
       exit 0
    ;;
    i) INSTALL_VERSION=$OPTARG 
    ;;
    u) uninstall
       exit 0
    ;;
    esac
done

makedir

download

if [ "$INSTALL_VERSION" = "32" ]; then
    install32
    exit 0
elif [ "$INSTALL_VERSION" = "64" ]; then
    install64
    exit 0
else
    echo "Error with version, not 32 or 64. Try again. Thanks."
    exit 0
fi

exit 0
