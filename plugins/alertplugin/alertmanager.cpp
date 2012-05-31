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
#include "alertmanager.h"
#include "alertitem.h"

#include <QTextDocument>

using namespace Alert;

AlertManager::AlertManager(QObject *parent)
{
    m_type = 0;
    //ALL
    m_w = new ShowAlerts(this);
}

AlertManager::~AlertManager()
{
}

void AlertManager::showAlertsIHM()
{
    m_w->show();
}

void AlertManager::setupWidgets()
{
    m_w->setWindowTitle(trUtf8("Alert"));    
    m_gridLayout = new QGridLayout(m_w);

    m_ButtonsFrame = new QFrame(m_w);
    m_ButtonsFrame->setFrameShape(QFrame::Box);
    m_HButtonsLayout = new QHBoxLayout(m_ButtonsFrame);
    m_buttonAdd = new QPushButton(m_w);
    m_buttonDel = new QPushButton(m_w);
    m_buttonClose = new QPushButton(m_w);
    m_buttonAdd->setText(trUtf8("Add"));
    m_buttonDel->setText(trUtf8("Delete"));
    m_buttonClose->setText(trUtf8("Close"));
    m_HButtonsLayout->addWidget(m_buttonAdd);
    m_HButtonsLayout->addWidget(m_buttonDel);
    m_HButtonsLayout->addWidget(m_buttonClose);
    m_ButtonsFrame->setLayout(m_HButtonsLayout);
   
    connect(m_buttonClose,SIGNAL(pressed()),m_w,SLOT(close()));
}


void AlertManager::initializeWithType(int type)
{
    m_type = type;
    setupWidgets();
    setupIHM(type);
    showAlertsIHM();
}

void AlertManager::setupIHM(int type)
{
    
//    switch(m_type){
//        case IAlert::ALL_ALERTS :
//            setupAllAlerts();
//            break;
//        case IAlert::PATIENT_PRIMARY_PREVENTION_ALERTS :
//            setupPrimaryPreventionAlerts();
//            break;
//        case IAlert::PATIENT_SECONDARY_PREVENTION_ALERTS :
//            setupSecondaryPreventionAlerts();
//            break;
//        case IAlert::PATIENT_PRESCRIPTION_ALERTS :
//            setupPrescriptionAlerts();
//            break;
//        case IAlert::PATIENT_NOTES :
//            setupPatientNotes();
//            break;
//        default :
//            break;
//        }
}

void AlertManager::setupAllAlerts()
{
    m_tableWidget = new QTableWidget(m_w);
//    m_tableWidget->setColumnCount(IAlert::ItemsDefinition_MaxParam);
    m_gridLayout->addWidget(m_tableWidget,0,0);
    m_gridLayout->addWidget(m_ButtonsFrame,1,0);
    m_w->setLayout(m_gridLayout);
}

void AlertManager::setupPrimaryPreventionAlerts()
{
    m_tableWidget = new QTableWidget(m_w);
//    m_tableWidget->setColumnCount(IAlert::ItemsDefinition_MaxParam);
    m_gridLayout->addWidget(m_tableWidget,0,0);
    m_gridLayout->addWidget(m_ButtonsFrame,1,0);
    m_w->setLayout(m_gridLayout);    
}

void AlertManager::setupSecondaryPreventionAlerts()
{
    m_tableWidget = new QTableWidget(m_w);
//    m_tableWidget->setColumnCount(IAlert::ItemsDefinition_MaxParam);
    m_gridLayout->addWidget(m_tableWidget,0,0);
    m_gridLayout->addWidget(m_ButtonsFrame,1,0);
    m_w->setLayout(m_gridLayout);    
}

void AlertManager::setupPrescriptionAlerts()
{
    m_textEditPrescription = new QTextEdit(m_w);
    m_gridLayout->addWidget(m_textEditPrescription,0,0);
    m_gridLayout->addWidget(m_ButtonsFrame,1,0);
    m_w->setLayout(m_gridLayout); 
    const QString textToDisplay = "This medicine does'nt fit  with that patient or another message";
    const QString text = "<html><body><font size=3 color=red>"+textToDisplay+"</font></body></html>";
    QTextDocument *doc = new QTextDocument(this);
    doc->setHtml(text);
    m_textEditPrescription->setDocument(doc);
}

void AlertManager::setupPatientNotes()
{
    m_textEditNotes = new QTextEdit(m_w);
    m_gridLayout->addWidget(m_textEditNotes,0,0);
    m_gridLayout->addWidget(m_ButtonsFrame,1,0);
    m_w->setLayout(m_gridLayout);    
}


///////////////////////////////////////////
////////ITEMS
///////////////////////////////////////////


void AlertManager::setRowOfItems(QVector<QVariant> datas)
{
    int rows = m_tableWidget->rowCount();
    m_tableWidget->setRowCount(rows+1);
    
//    for (int col = IAlert::TYPE_OF_ITEM; col < IAlert::ItemsDefinition_MaxParam; ++col)
//    {
//    	  QTableWidgetItem * item = new QTableWidgetItem();
//    	  item->setData(Qt::DisplayRole,datas[col]);
//    	  m_tableWidget->setItem(rows+1,col,item);
//        }
}


