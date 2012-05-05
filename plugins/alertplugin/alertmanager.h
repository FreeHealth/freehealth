/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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

