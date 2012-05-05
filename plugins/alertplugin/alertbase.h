#ifndef ALERTBASE_H
#define ALERTBASE_H

//#include <utils/log.h>
#include <QSqlDatabase>
#include <QObject>
#include <QDebug>

namespace Alert {

class AlertBase:public QObject
{
	Q_OBJECT
	private:
		AlertBase(QObject * parent = 0);
		~AlertBase();
        public:
            static AlertBase * singleton()
            {
                if (m_singleton == NULL)
                {
                	  m_singleton = new AlertBase();
                	  qWarning() << __FILE__ << QString::number(__LINE__) << "AlertBase is created" ;
                    }
                else
                {
                	qWarning() << __FILE__ << QString::number(__LINE__) << "AlertBase is already opened" ;
                    }
                return m_singleton;
            }
            QSqlDatabase m_db;
            
	private:
	    static AlertBase * m_singleton;
	    bool connectToAlertDatabase();
	    bool setTables();
};

}  // Alert

#endif

