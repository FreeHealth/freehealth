#include "rapidtofreeviewer.h"
#include "rapidtofreeio.h"

#include <QApplication>
#include <QDebug>

int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	//connexion
        RapidToFreeIO * rIO = new RapidToFreeIO;
        if (!rIO->initialiseBases())
        {
        	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect to databases" ;
            }
	RapidToFreeViewer * viewer = new RapidToFreeViewer;
	viewer->show();
	
	return a.exec();
}
