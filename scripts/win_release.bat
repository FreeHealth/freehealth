cd ..
cd global_resources/translations
lrelease *.ts
cd ../../%1
qmake.exe %1.pro -r -spec win32-g++ CONFIG+=release LOWERED_APPNAME=%1 CONFIG-=debug_and_release
mingw32-make.exe -w
mingw32-make.exe install
