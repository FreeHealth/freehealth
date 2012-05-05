#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include "showalertsIHM.h"

//#include <utils/log.h>

#include <QObject>
#include <QDebug>
#include <QFrame>
#include <QTableWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>

#include <QVariant>
#include <QVector>

namespace Alert {

class AlertManager : public QObject
{
	Q_OBJECT
	private:
		AlertManager(QObject * parent = 0);
		~AlertManager();
        public:
          static AlertManager * singleton()
          {
            if(m_singleton == NULL)
            {
                m_singleton = new AlertManager();
                qWarning() << __FILE__ << QString::number(__LINE__) << "AlertManager is  opening" ;
                }
            else
            {
            	qWarning() << __FILE__ << QString::number(__LINE__) << "AlertManager is already opened" ;
                }
            return m_singleton;
            };
            void initializeWithType(int type);
	private:
	    //global
	    void setupWidgets();
	    void showAlertsIHM();
	    void setupIHM(int type);
	    static AlertManager * m_singleton;	    
	    ShowAlerts * m_w;
	    int m_type;
	    //various IHM
	    void setupAllAlerts();
	    //widgets
	    //ALL
	    QGridLayout * m_gridLayout;
	    QFrame * m_ButtonsFrame;
	    QHBoxLayout * m_HButtonsLayout;
	    QPushButton * m_buttonAdd;
	    QPushButton * m_buttonDel;
	    QPushButton * m_buttonClose;
	    //ALL_ALERTS
	    QTableWidget * m_tableWidget;
	    //PATIENT_PRIMARY_PREVENTION_ALERTS
	    void setupPrimaryPreventionAlerts();
	    //PATIENT_SECONDARY_PREVENTION_ALERTS
	    void setupSecondaryPreventionAlerts();
	    //PATIENT_PRESCRIPTION_ALERTS
	    void setupPrescriptionAlerts();
	    QTextEdit * m_textEditPrescription;
	    //NOTES
	    QTextEdit * m_textEditNotes;
	    void setupPatientNotes();	    
	    //items
	    void setRowOfItems(QVector<QVariant> datas);
};

}  // Alert

#endif

