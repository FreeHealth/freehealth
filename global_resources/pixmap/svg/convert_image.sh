###########################################################################
#  The FreeMedForms project is a set of free, open source medical         #
#  applications.                                                          #
#  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      #
#  All rights reserved.                                                   #
#                                                                         #
#  The FreeAccount plugins are free, open source FreeMedForms' plugins.   #
#  (C) 2013-now by Christian A Reiter and Eric Maeker                     #
#  (C) 2010-2012 by Pierre-Marie Desombre and Eric Maeker                 #
#  All rights reserved.                                                   #
#                                                                         #
#  This program is free software: you can redistribute it and/or modify   #
#  it under the terms of the GNU General Public License as published by   #
#  the Free Software Foundation, either version 3 of the License, or      #
#  (at your option) any later version.                                    #
#                                                                         #
#  This program is distributed in the hope that it will be useful,        #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#  GNU General Public License for more details.                           #
#                                                                         #
#  You should have received a copy of the GNU General Public License      #
#  along with this program.                            .                   #
#  If not, see <http://www.gnu.org/licenses/>.                            #
###########################################################################
#  Author: Christian A. Reiter <christian.a.reiter@gmail.com>             #
#  Contributors:                                                          #
#      NAME <MAIL@ADDRESS.COM>                                            #
###########################################################################

#!/bin/sh

SVG_DIR=.
BASE_DIR=..

SIZES=`ls -d1 $BASE_DIR/*x* |sed "s/$BASE_DIR\///"`
for DIR in $BASE_DIR/   ; do

    # remove leading $BASEDIR
    SIZES="$SIZES ${DIR##*/}"
done

print_usage() {
    echo "Converts a svg image to a png in the given geometry and moves it into the right directory."
    echo
    echo "usage: ${0##*/} <filename.svg> <geometry>"
    echo "Example: ${0##*/} SomeImage.svg 32x32"
    echo "available sizes:"
    echo "$SIZES"
    echo
}



# main



if [ "$#" = "0" -o "$1" = "--help" -o "$1" = "-h" ]; then
    print_usage
    exit 0
fi

if [ "$1" = "--all" ]; then
    for imageSize in $SIZES; do
        for svgFile in $SVG_DIR/*.svg; do
            convert $svgFile -resize $imageSize $BASE_DIR/${svgFile%.svg}.png
        done
    done
fi
if [ -f "$1" ]; then
    if [ -d "$BASE_DIR/$2" ]; then
        echo "Converting '$1' to $BASE_DIR/$2/${1%.svg}.png, resizing to $2px."
        convert "$1" -resize "$2" $BASE_DIR/$2/${1%.svg}.png
    else
        echo "directory '$BASE_DIR/$2' does not exist. Please create it."
        exit 2
    fi
else
    echo "The file '$1' does not exist."
    exit 1
fi
