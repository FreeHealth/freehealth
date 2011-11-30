#include "mainwindow.h"

#include <datapackutils/servermanager.h>

#include <QtGui/QApplication>

// Sur MacOsX : créé un compte FTP Anonyme
// sudo dscl . -create /Users/ftp
// sudo dscl . -create /Users/ftp NFSHomeDirectory /path/to/ftp/folder
// Effacer compte anonyme
// sudo dscl . -delete /Users/ftp

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataPack::ServerManager *manager = new DataPack::ServerManager("/tmp/");
    manager->isInternetConnexionAvailable();
#ifdef Q_OS_MAC
    manager->addServer(QUrl("file://Users/eric/Public/datapacks"));
#elif
    manager->addServer(QUrl("ftp://192.168.0.12/"));
#endif
    manager->connectAndUpdate(0);

    MainWindow w;
	w.show();

    return a.exec();
}
