#include "rapidtofreeio.h"
#include <accountplugin/accountbase.h>


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
        dbRapidCompta.setHostName(host);
        dbRapidCompta.setDatabaseName(Constants::DB_CCAM);
        dbRapidCompta.setUserName(log);
        dbRapidCompta.setPassword(pass);
       if (!dbRapidCompta.isOpen() && !dbRapidCompta.open())
       {
    	    qWarning() << __FILE__ << QString::number(__LINE__) << dbRapidCompta.lastError().text() ;
    	    return false;
        }
        return true;
        }
    return false;
}
