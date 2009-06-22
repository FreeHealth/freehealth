#!/bin/dash
#  FreeMedForms, (c) Eric Maeker, 2008
#  License of this script is BSD-modified
#  This script prepare binaries for linux and windows, zip all needed files for each

PATH_TO_BIN="./Binaries"
RETURN_FROM_BIN=".."

PATH_TO_APP_BIN="./bin/"

PATH_TO_SAMPLE="./forms/"

echo "----------------------------------------------------------------------"
echo "|| Building ZIP archive of FreeMedForms (Linux and Win32 platform ) ||"
echo "----------------------------------------------------------------------"
echo " "
cd ..

# Modifie les *.pro pour enlever les warns et mettre en release
sed -i 's#CONFIG              += qt warn_on thread x11 windows debug#CONFIG              += qt warn_off thread x11 windows release#' config.pri 

# Cleaning project
if [ ! -e Makefile ] ; then make distclean; fi


# Compile and Strip Linux binary  (binary is located in FreeMedForms/bin named=freemedforms)
echo "***********************************"
echo "*****  Building Linux binary  *****"
echo `qmake-qt4 -Wnone -recursive && make`
echo `make clean`
echo "********  Stripping binary  *******"
# strip -s $PATH_TO_APP_BIN/freemedforms


# Create a temporary dir
echo "******  Creating Zipped file  *****"
if [ ! -e $PATH_TO_BIN/linux ] ; then mkdir $PATH_TO_BIN/linux ; fi
cp -R $PATH_TO_APP_BIN* $PATH_TO_BIN/linux
echo `rm -rf $PATH_TO_BIN/linux/freemedforms.exe`
echo `rm -rf $PATH_TO_BIN/linux/tmp/*`
echo `rm -rf $PATH_TO_BIN/linux/translations/*.ts`
echo `find ./$PATH_TO_BIN/linux/* -type f -name "*~" -exec rm -f {} \;`
echo `rm -rf $PATH_TO_BIN/linux/config.ini`
echo `rm -rf $PATH_TO_BIN/linux/plugins/*.dll`

# Zip all files
cd $PATH_TO_BIN/linux/
find ./* | grep -v 'svn' > ../includefiles
zip -rq $RETURN_FROM_BIN/../FreeMedForms_Linux.zip . -i@../includefiles
rm ../includefiles
cd $RETURN_FROM_BIN/..
echo "***********************************"

echo " "

# Compile and Strip Windows binary
# Go to src dir
echo "***********************************"
echo "**  Building Windows 32 binary  ***"
echo `qmake-qt4 -Wnone -recursive -spec win32-x-g++ -win32 && make`
echo `make clean`

# Create a temporary dir
echo "******  Creating Zipped file  *****"
if [ ! -e $PATH_TO_BIN/win32 ] ; then mkdir $PATH_TO_BIN/win32 ; fi
cp -R $PATH_TO_APP_BIN* $PATH_TO_BIN/win32
echo `rm -rf $PATH_TO_BIN/win32/freemedforms`
echo `rm -rf $PATH_TO_BIN/win32/tmp/*`
echo `rm -rf $PATH_TO_BIN/win32/translations/*.ts`
echo `find ./$PATH_TO_BIN/win32/* -type f -name "*~" -exec rm -f {} \;`
echo `rm -rf $PATH_TO_BIN/win32/config.ini`
echo `rm -rf $PATH_TO_BIN/win32/plugins/*.so`


# Zip all files
cd $PATH_TO_BIN/win32/
find ./* | grep -v 'svn' > ../includefiles
zip -rq $RETURN_FROM_BIN/../FreeMedForms_Win32.zip . -i@../includefiles
rm ../includefiles
cd $RETURN_FROM_BIN/..
echo "***********************************"


# Modifie les *.pro pour remettre les warns et mettre en release
sed -i 's#CONFIG              += qt warn_off thread x11 windows release#CONFIG              += qt warn_on thread x11 windows debug#' config.pri 

cd scripts
exit 0
