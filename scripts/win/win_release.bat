REM # This script is part of FreeMedForms project : http://www.freemedforms.com
REM # (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>
REM #
REM # This script compiles and create the installer for any FreeMedForms application
REM # 
REM # The script supposes that:
REM # - you work on the drive E:
REM # - the freemedforms-project source package is decompressed here
REM # - the most recent MySQL is installed in E:\MySQL
REM # - the Inno Setup 5 is installed on the machine, in C:\Program Files\Inno Setup 5\
REM # 
REM # After the compilation, you will find the installer in the source root dir

REM # Go to source root dir
cd ../..

REM # Create translations
cd global_resources/translations
lrelease *.ts

REM # Go to application source tree
cd ../../%1

REM # Compil application && install it
qmake.exe %1.pro -r -spec win32-g++ CONFIG+=release CONFIG-=debug_and_release
mingw32-make.exe -w
mingw32-make.exe install

REM # Copy MySQL lib into the package dir
copy e:\MySQL\bin\libmySQL.dll ..\packages\win\%1\libmySQL.dll
copy e:\MySQL\bin\libmySQL.dll ..\packages\win\%1\plugins\libmySQL.dll

REM # Copy OpenCV lib into the package dir
REM # copy e:\MySQL\bin\libmySQL.dll ..\packages\win\%1\libmySQL.dll
REM # copy e:\MySQL\bin\libmySQL.dll ..\packages\win\%1\plugins\libmySQL.dll

REM # Create the installer
C:\Progra~1\InnoSe~1\iscc.exe "e:\freemedforms-__version__/global_resources/package_helpers/%1.iss"

REM # Go to root source tree
cd ..

REM # Rename and move the setup.exe file
copy packages\win\%1\%1\setup.exe %1-__version__.exe
