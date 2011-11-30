#include "mainwindow.h"

#include <datapackutils/core.h>
#include <datapackutils/iservermanager.h>

#include <QtGui/QApplication>

// Sur MacOsX : créé un compte FTP Anonyme
// sudo dscl . -create /Users/ftp
// sudo dscl . -create /Users/ftp NFSHomeDirectory /path/to/ftp/folder
// Effacer compte anonyme
// sudo dscl . -delete /Users/ftp

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataPack::Core *core = DataPack::Core::instance();
    core->isInternetConnexionAvailable();

#ifdef Q_OS_MAC
    core->serverManager()->addServer("file://Users/eric/Public/datapacks");
#elif
    core->serverManager()->addServer("ftp://localhost/");
#endif
    core->serverManager()->connectAndUpdate(0);

    MainWindow w;
    w.show();

    return a.exec();
}
