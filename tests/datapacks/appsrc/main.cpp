#include "mainwindow.h"

#include <datapackutils/servermanager.h>

#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataPack::ServerManager *manager = new DataPack::ServerManager("/tmp/");
    manager->isInternetConnexionAvailable();
    manager->addServer(QUrl("ftp://anonymous:ok@localhost:/"));
    manager->connectAndUpdate(0);

    MainWindow w;
	w.show();

    return a.exec();
}
