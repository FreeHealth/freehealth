#include "alertbase.h"

#include <QSqlError>
#include <QStringList>
#include <QSqlQuery>

using namespace Alert;

AlertBase * AlertBase::m_singleton = NULL;

AlertBase::AlertBase(QObject * parent)
{
    if (!connectToAlertDatabase())
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to connect to alertbase !" ;
        }
}

AlertBase::~AlertBase()
{
    if (m_singleton)
    {
    	  m_singleton = NULL;
        }
}

bool AlertBase::connectToAlertDatabase()
{
    bool success = true;
    m_db = QSqlDatabase::addDatabase("QSQLITE","Alert");
    m_db.setDatabaseName("alerts.db");
    if (!m_db.open())
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << m_db.lastError().text() ;
    	  success = false;
        }
    else
    {
    	if (m_db.tables().size()<1)
    	{
    		  if (!setTables())
    		  {
    		  	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to set defaults values" ;
    		  	  success = false;
    		      }
    	    }
        }
    return success;
}

bool AlertBase::setTables()
{
    bool success = true;
    QHash<QString,QString> hashOfRequests;
    hashOfRequests.insert("Table alerts",
                    "CREATE TABLE IF NOT EXISTS	alerts  ("
                    "ID_Primkey  INTEGER PRIMARY KEY,"
                    "ITEM longtext  NULL,"
                    "TYPE int(10) NOT NULL,"
                    "PARENT longtext  NOT NULL,"
                    "PARENT_OR_CHILD tinyint(1) NOT NULL,"
                    "ICON BLOB  NULL,"
                    "DATE_DONE	datetime   NULL,"
                    "DATE_NEXT	datetime   NULL,"
                    "ABSTRACT	longtext  NULL,"
                    "ID_ITEM    bigint NOT NULL,"
                    "PATIENT_UID varchar(32)  NULL ,"
                    "USER_UID int(11) NULL,"
                    "RESULT	BLOB  NULL);");
                    
    hashOfRequests.insert("Table nextdate",
                    "CREATE TABLE IF NOT EXISTS	nextdate  ("
                    "ID_Primkey  INTEGER PRIMARY KEY,"
                    "ITEM longtext  NULL,"
                    "YEAR int(4) NULL,"
                    "MONTH int(2) NULL,"
                    "DAY int(2) NULL,"
                    "HOURS int(2) NULL,"
                    "MIN int(2) NULL,"
                    "USER_UID int(11) NULL);"); 
    QStringList tablesList;
    tablesList = hashOfRequests.keys();
    QSqlQuery q(m_db);    
    foreach(QString table,tablesList)
    {
    	  const QString req = hashOfRequests.value(table);
    	  if (!q.exec(req))
    	  {
    	  	  qWarning() << __FILE__ << QString::number(__LINE__) << q.lastError().text() ;
    	  	  success = false;    	  	  
    	      }
        }
    return success;
}
 

