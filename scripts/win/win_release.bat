REM # This script is part of FreeMedForms project : http://www.freemedforms.com
REM # (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>
REM # License: BSD 3-clause
REM #
REM # This script compiles and creates the installer for any application
REM # 
REM # The script supposes that:
REM # - freemedforms-project source package is decompressed here
REM # - Use MinGW shipped with Qt (see %PATH_TO_MINGW%)
REM # - Latest MySQL 5.5 version is installed (see %PATH_TO_MYSQL%)
REM # - most recent OpenCV must be installed in the contrib path 
REM # - Inno Setup 5 is installed on the machine (see %PATH_TO_INNOSETUP%)
REM # - unix2dos.exe is installed on the machine in the %PATH%
REM # - Win 32 OpenSSL is installed on the machine (see %PATH_TO_OPENSSL%)
REM # You can get OpenSSL from https://www.slproweb.com/products/Win32OpenSSL.html
REM # (current version as ok 08/2016 is Win32 OpenSSL v1.0.2h Light) or build it yourself.
REM # 
REM # After the compilation, you will find the installer in a folder named Output

REM # Var definition
set VERSION=__version__
set PATH_TO_MYSQL=C:\Progra~1\MySQL\MYSQLS~1.5\lib
set PATH_TO_INNOSETUP=C:\Progra~1\InnoSe~1\iscc.exe
set WORKING_DIRECTORY=%CD%
set PATH_TO_MINGW=C:\Qt\Qt5.6.1\Tools\mingw492_32
set PATH_TO_OPENSSL=C:\OPENSS~1
set PATH_TO_SYSTEM32=C:\Windows\System32

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

REM # Go to source root dir
cd ..

REM # Copy MySQL lib into package dir
copy %PATH_TO_MYSQL%\libmySQL.dll packages\win\%1\libmySQL.dll
copy %PATH_TO_MYSQL%\libmySQL.dll packages\win\%1\plugins\libmySQL.dll

REM # Copy MinGW lib into package dir
copy %PATH_TO_MINGW%\libgcc_s_dw2-1.dll packages\win\%1\
copy %PATH_TO_MINGW%\libstdc*-6.dll packages\win\%1\

REM # Copy OpenSSL lib into package dir
copy %PATH_TO_OPENSSL%\ssleay32.dll packages\win\%1\
copy %PATH_TO_OPENSSL%\libssl32.dll packages\win\%1\
copy %PATH_TO_OPENSSL%\libeay32.dll packages\win\%1\

REM # Copy msvcr120.dll into package dir
copy %PATH_TO_SYSTEM32%\msvcr120.dll packages\win\%1\

REM # Change linefeed on TXT files
unix2dos packages\win\%1\README.txt
unix2dos packages\win\%1\COPYING.txt

REM # Create the installer
copy global_resources\package_helpers\%1.iss packages\win\%1\
REM # TODO: use %1 instead of "ehr" when renaming is completed
copy %1\%1-src\ehr.ico packages\win\%1\
%PATH_TO_INNOSETUP% packages\win\%1\%1.iss"
del packages\win\%1\%1.iss

REM # Rename and move the setup.exe file
copy packages\win\%1\setup.exe %1-%VERSION%.exe

REM # Unset var
set PATH_TO_MYSQL=
set PATH_TO_OPENCV=
set PATH_TO_INNOSETUP=
set WORKING_DIRECTORY=

cd %WORKING_DIRECTORY%
