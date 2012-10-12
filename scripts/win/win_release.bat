REM # This script is part of FreeMedForms project : http://www.freemedforms.com
REM # (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>
REM # License: BSD 3-clause
REM #
REM # This script compiles and create the installer for any FreeMedForms application
REM # 
REM # The script supposes that:
REM # - the freemedforms-project source package is decompressed here
REM # - the most recent MySQL is installed (see %PATH_TO_MYSQL%)
REM # - the most recent OpenCV is installed (see %PATH_TO_OPENCV%)
REM # - the Inno Setup 5 is installed on the machine (see %PATH_TO_INNOSETUP%)
REM # 
REM # After the compilation, you will find the installer in the source root dir

REM # Var definition
@echo off set PATH_TO_MYSQL=E:\MySQL\bin
@echo off set PATH_TO_OPENCV=E:\opencv\bin
@echo off set PATH_TO_INNOSETUP=C:\Progra~1\InnoSe~1\iscc.exe
@echo off set WORKING_DIRECTORY=%CD%

REM # Go to source root dir
cd ../..

REM # Create translations
cd global_resources/translations
lrelease *.ts
cd ../..

REM # Go to application source tree
cd %1

REM # Compil application && install it
qmake.exe %1.pro -r -spec win32-g++ CONFIG+=release CONFIG-=debug_and_release
mingw32-make.exe -w
mingw32-make.exe install

REM # Copy MySQL lib into the package dir
copy %PATH_TO_MYSQL%\libmySQL.dll ..\packages\win\%1\libmySQL.dll
copy %PATH_TO_MYSQL%\libmySQL.dll ..\packages\win\%1\plugins\libmySQL.dll

REM # Copy OpenCV lib into the package dir
REM # copy %PATH_TO_OPENCV%\libopencv_core.2.4.dll ..\packages\win\%1\
REM # copy %PATH_TO_OPENCV%\libopencv_highgui.2.4.dll ..\packages\win\%1\
REM # copy %PATH_TO_OPENCV%\libopencv_objdetect.2.4.dll ..\packages\win\%1\

REM # Create the installer
%PATH_TO_INNOSETUP% "%WORKING_DIRECTORY%/../../global_resources/package_helpers/%1.iss"

REM # Go to root source tree
cd ..

REM # Rename and move the setup.exe file
copy packages\win\%1\%1\setup.exe %1-__version__.exe

REM # Unset var
@echo off set PATH_TO_MYSQL=
@echo off set PATH_TO_OPENCV=
@echo off set PATH_TO_INNOSETUP=
@echo off set WORKING_DIRECTORY=

cd %WORKING_DIRECTORY%
