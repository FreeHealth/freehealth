cd ..
cd glo*/tr*
lrelease *.ts
cd ../..
qmake.exe E:/%1-%2/%1.pro -r -spec win32-g++ CONFIG+=release LOWERED_APPNAME=%1 CONFIG-=debug_and_release
mingw32-make.exe -w -j1
mingw32-make.exe install
