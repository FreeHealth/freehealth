#include "preventCore.h"
#include "connexion.h"

#include <QApplication>

int main (int argc, char * argv[])
{
	QApplication a(argc,argv);
	Common::Connexion * c = new Common::Connexion;
	if (!c->connect())
	{
		  qWarning() << __FILE__ << QString::number(__LINE__) << "cannot connect to database" ;
	    }
	PreventCore *p = new PreventCore;
	p->run();
	
	return a.exec();
}
