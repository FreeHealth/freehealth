#include "movementsviewer.h"
#include <QApplication>

int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	movementsViewer m;
	m.show();
	int r = a.exec();
	return r;
}
