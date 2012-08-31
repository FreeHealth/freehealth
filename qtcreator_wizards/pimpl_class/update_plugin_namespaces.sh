#!/bin/sh
# ***************************************************************************
# *  The FreeMedForms project is a set of free, open source medical         *
# *  applications.                                                          *
# *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
# *  All rights reserved.                                                   *
# *                                                                         *
# *  This program is free software: you can redistribute it and/or modify   *
# *  it under the terms of the GNU General Public License as published by   *
# *  the Free Software Foundation, either version 3 of the License, or      *
# *  (at your option) any later version.                                    *
# *                                                                         *
# *  This program is distributed in the hope that it will be useful,        *
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
# *  GNU General Public License for more details.                           *
# *                                                                         *
# *  You should have received a copy of the GNU General Public License      *
# *  along with this program (COPYING.FREEMEDFORMS file).                   *
# *  If not, see <http://www.gnu.org/licenses/>.                            *
# ***************************************************************************
# ***************************************************************************
# *   Main developers: Christian A. Reiter <christian.a.reiter@gmail.com>   *
# *   Contributors:                                                         *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# *       NAME <MAIL@ADDRESS.COM>                                           *
# ***************************************************************************

# this script updates the list of available plugins and makes it available
# in the combobox of the PIMPL wizard

workdir=$PWD
cd ../../plugins

# clear combochoices.txt file
echo "generated file by $0\nPlease copy the following line into the combochoices attribute of the PluginNamespace field in wizard.xml" > "$workdir/combochoices.txt"

for plugin in *; do
    if [ -d "$plugin" -a -f "$plugin/$plugin.h" ]; then
        echo Plugin: \"$plugin\"
        echo Namespace: \"$namespace\"
        namespace=`grep "^namespace.*{.*$" $plugin/$plugin.h | sed "s/Internal.*//" | cut -f2 -d' '`
        echo
        echo -n "$namespace," >> "$workdir/combochoices.txt"
    fi
done
echo "File written successfully."