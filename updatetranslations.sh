#!/bin/sh
# ****************************************************************************
#  *  The FreeMedForms project is a set of free, open source medical         *
#  *  applications.                                                          *
#  *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
#  *  All rights reserved.                                                   *
#  *                                                                         *
#  *  This program is free software: you can redistribute it and/or modify   *
#  *  it under the terms of the GNU General Public License as published by   *
#  *  the Free Software Foundation, either version 3 of the License, or      *
#  *  (at your option) any later version.                                    *
#  *                                                                         *
#  *  This program is distributed in the hope that it will be useful,        *
#  *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
#  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
#  *  GNU General Public License for more details.                           *
#  *                                                                         *
#  *  You should have received a copy of the GNU General Public License      *
#  *  along with this program.                                               *
#  *  If not, see <http://www.gnu.org/licenses/>.                            *
#  ***************************************************************************
# /***************************************************************************
#  *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
#  *  Contributors:                                                          *
#  *       Christian A. Reiter <christian.a.reiter@gmail.com                 *
#  *       NAME <MAIL@ADDRESS.COM>                                           *
#  ***************************************************************************
# this script redirects the output from the error console to stdout
# so that the whole output can be filtered via e.g. grep

# define colors for highlighting
GREEN="\033[32;40m"
NO_COLOUR="\033[0m"

#for i in $( ls plugins ); do
#    if [ -f plugins/$i/*.pro ]; then
#        echo
#        echo "$WHITE********* Updating translation for EHR plugin: $i$NO_COLOUR"
#        echo
#        lupdate plugins/$i/*plugin.pro -no-obsolete  2>&1
#    fi
#done

APPS=". freediams freedrc freetoolbox freeddimanager"
for a in $APPS; do
    for i in $( ls $a/plugins ); do
        if [ -f $a/plugins/$i/*.pro ]; then
            printf "$GREEN********* Updating translation for $a plugin: $i$NO_COLOUR\n"
            lupdate $a/plugins/$i/*.pro -no-obsolete  2>&1
        fi
    done
done

for i in $( ls libs ); do
    if [ -f libs/$i/*.pro ]; then
        printf "$GREEN********* Updating translation for libs: $i$NO_COLOUR\n"
        lupdate libs/$i/*.pro -no-obsolete  2>&1
    fi
done

exit 0;
