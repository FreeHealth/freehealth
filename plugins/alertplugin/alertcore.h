#ifndef ALERTCORE_H
#define ALERTCORE_H

#include "alertbase.h"
#include "alertmanager.h"
#include "ialert.h"

//#include <utils/log.h>
#include <QObject>
#include <QDebug>

namespace Alert {

class AlertCore : public IAlert
{
    Q_OBJECT
    private:
        AlertCore(QObject * parent = 0);
        ~AlertCore();
    public:
        static AlertCore * singleton()
        {
            if(m_singleton == NULL)
            {
                m_singleton = new AlertCore();
                qWarning() << __FILE__ << QString::number(__LINE__) << "AlertCore is  opening" ;
                }
            else
            {
            	qWarning() << __FILE__ << QString::number(__LINE__) << "AlertCore is already opened" ;
                }
            return m_singleton;
            };
         
         void initialize();
         QString setAlertUuid();
         void showIHMaccordingToType(int type = 0);
     private:
         static AlertCore * m_singleton;
         AlertBase * m_alertBase;
         AlertManager * m_alertManager;
};

}  // Alert

#endif

