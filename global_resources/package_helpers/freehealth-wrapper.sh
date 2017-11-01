#!/bin/sh
#/***************************************************************************
# *  The FreeMedForms project is a set of free, open source medical         *
# *  applications.                                                          *
# *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
# *  All rights reserved.                                                   *
# *  License of this file: BSD-3 clause                                     *
# ***************************************************************************/
#/***************************************************************************
# *  Authors:                                                               *
# *  Eric MAEKER <eric.maeker@gmail.com>                                    *
# *  NAME <MAIL@ADDRESS.COM>                                                *
# ***************************************************************************/
#
#/***************************************************************************
# * This code was adapted from the QtCreator source                         *
# ***************************************************************************
# * The wrapper sets the LD_LIBRARY_PATH to the correct path when running   *
# * linux-integrated release build on a linux system.                       *
# ***************************************************************************/

BINARY="freehealth.bin"
PLUGINS_PATH="freehealth"
APPLICATION_NAME="FreeHealth"

# Check if the freehealth binary is a linux-integrated & a release build
checkEhrBuild()
{
    if `/usr/bin/$BINARY -v` | grep -q "Release (Linux Integrated)"; then
        echo "Found $APPLICATION_NAME binary"
    else
        echo "The installed $APPLICATION_NAME binary (in /usr/bin) is not a correct version. You cannot use this wrapper to run your version of $APPLICATION_NAME"
        return 1;
    fi
}

# If the binary is a link, follow it
makeAbsolute() {
    case $1 in
        /*)
            # already absolute, return it
            echo "$1"
            ;;
        *)
            # relative, prepend $2 made absolute
            echo `makeAbsolute "$2" "$PWD"`/"$1" | sed 's,/\.$,,'
            ;;
    esac
}

me=`which "$0"` # Search $PATH if necessary
if test -L "$me"; then
    # Try readlink(1)
    readlink=`type readlink 2>/dev/null` || readlink=
    if test -n "$readlink"; then
        # We have readlink(1), so we can use it. Assuming GNU readlink (for -f).
        me=`readlink -nf "$me"`
    else
        # No readlink(1), so let's try ls -l
        me=`ls -l "$me" | sed 's/^.*-> //'`
        base=`dirname "$me"`
        me=`makeAbsolute "$me" "$base"`
    fi
fi

checkEhrBuild

# Prepare paths
bindir=`dirname "$me"`
LIB_DIR="/usr/lib/freehealth-common"
PLUG_DIR="/usr/lib/$PLUGINS_PATH"

# Define the LD_LIBRARY_PATH
LD_LIBRARY_PATH=$LIB_DIR:$PLUG_DIR${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

# Launch application
exec "/usr/bin/$BINARY" ${1+"$@"}
