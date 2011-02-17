#include <QApplication>
#include <QtCore>
#include <QDebug>
#include "ledgerviewer.h"
int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	QStringList libPath ;
	libPath << a.applicationDirPath()+"/plugins";
	qDebug() << __FILE__ << QString::number(__LINE__) << " path =" << libPath ;
	a.setLibraryPaths(libPath);
	LedgerViewer m(0);
	m.show();
	int r = a.exec();
	return r;
}

