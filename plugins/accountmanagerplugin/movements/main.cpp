#include <QApplication>
#include <QtCore>
#include <QDebug>
#include "movementsviewer.h"
int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	QStringList libPath ;
	libPath << a.applicationDirPath()+"/plugins";
	qDebug() << __FILE__ << QString::number(__LINE__) << " path =" << libPath ;
	a.setLibraryPaths(libPath);
	movementsViewer m;
	m.show();
	int r = a.exec();
	return r;
}
