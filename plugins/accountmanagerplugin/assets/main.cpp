#include <QApplication>
#include <QtCore>
#include <QDebug>
#include "assetsViewer.h"
int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	QStringList libPath ;
	libPath << a.applicationDirPath()+"/plugins";
	qDebug() << __FILE__ << QString::number(__LINE__) << " path =" << libPath ;
	a.setLibraryPaths(libPath);
	AssetsViewer m;
	m.show();
	int r = a.exec();
	return r;
}
