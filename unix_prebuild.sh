#!/bin/sh
# This script is part of FreeMedForms project
# Goal : preparing the build process for unices

echo " ********************************************************* "
echo " *** This script is part of the FreeMedForms project   *** "
echo " *** Creator / main author : Eric MAEKER, MD           *** "
echo " *** 2008,2009                                         *** "
echo " *** eric.maeker@free.fr                               *** "
echo " *** Licence : GPL most recent                         *** "
echo " ********************************************************* "
echo 
echo "This script prepares the build process for MacOs, Linux and *BSD."
echo
echo "Creating resources files..."
if [ ! -e ./afssaps_db/bin/resources/processed/done.csv ] ; then
   touch ./afssaps_db/bin/resources/processed/done.csv
fi

if [ ! -e ./afssaps_db/bin/resources/processed/link-class-substances.csv ] ; then
   touch ./afssaps_db/bin/resources/processed/link-class-substances.csv
fi

echo "* You can build FreeMedForms in release mode."
echo "  This mode is safer than the debug mode. "
echo "  Non stable functionnalities are inhibited."
echo "  You should not encounter compilation errors."
echo "  . For MacOs : qmake -spec macx-g++ -r -config release"
echo "  . For Linux, FreeBSD : qmake-qt4 -r -config release"
echo "  . For Linux cross-compilation : qmake -spec win32-x-g++ -win32 -r \"CONFIG+=release crosscompil\" project.pro"
echo "  Then : make"
echo
echo "* You can build non stable FreeMedForms using the debug mode"
echo "  This method is suitable for development and "
echo "  active testing of the most recent development."
echo "  If you encounter compilation errors, please contact me."
echo "  . For MacOs : qmake -spec macx-g++ -r -config debug"
echo "  . For Linux, FreeBSD : qmake-qt4 -r -config debug"
echo "  . For Linux cross-compilation : qmake -spec win32-x-g++ -win32 -r "CONFIG+=debug crosscompil" project.pro"
echo "  Then : make"
echo
echo "For release build, type : make install"
echo "Find binary in package/yourOS/project/"

exit
