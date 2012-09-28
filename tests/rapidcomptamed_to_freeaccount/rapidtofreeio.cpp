#include "rapidtofreeio.h"
#include <../../plugins/accountbaseplugin/accountbase.h>


RapidToFreeIO::RapidToFreeIO(QObject * parent)
{
}

RapidToFreeIO::~RapidToFreeIO(){}

bool RapidToFreeIO::initialiseBases()
{
    	AccountBase::instance()->initialize();
    	if (!connectToRapidComptamed())
    	{
    		  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to connect to rapidcomptamed" ;
    	    }
}

bool RapidToFreeIO::connectToRapidComptamed()
{
    QSqlDatabase dbRapidCompta;
    dbRapidCompta = QSqlDatabase::addDatabase("QMYSQL", "RapidComptamed");
    ConnectionDialog * c = new ConnectionDialog(this);
    if (c->exec()==QDialog::Accepted)
    {    	      
        dbRapidCompta.setHostName(c->host());
        dbRapidCompta.setDatabaseName("RapidComptamed");
        dbRapidCompta.setUserName(c->login());
        dbRapidCompta.setPassword(c->password());
       if (!dbRapidCompta.isOpen() && !dbRapidCompta.open())
       {
    	    qWarning() << __FILE__ << QString::number(__LINE__) << dbRapidCompta.lastError().text() ;
    	    return false;
        }
        return true;
        }
    return false;
}
