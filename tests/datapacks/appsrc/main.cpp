#include "mainwindow.h"

#include <datapackutils/core.h>
#include <datapackutils/iservermanager.h>

#include <utils/global.h>

#include <QtGui/QApplication>
#include <QDesktopWidget>
#include <QDebug>

// Sur MacOsX : créé un compte FTP Anonyme
// sudo dscl . -create /Users/ftp
// sudo dscl . -create /Users/ftp NFSHomeDirectory /path/to/ftp/folder
// Effacer compte anonyme
// sudo dscl . -delete /Users/ftp

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();
    Utils::centerWidget(&w, qApp->desktop());

    return a.exec();
}
