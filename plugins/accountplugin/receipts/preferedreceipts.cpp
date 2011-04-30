/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "preferedreceipts.h"
#include "ui_preferedreceipts.h"
#include "choiceDialog.h"
#include "receiptsIO.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <QMessageBox>

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

PreferedReceipts::PreferedReceipts(QWidget * parent):QWidget(parent),ui(new Ui::PreferedReceiptsWidget){
    ui->setupUi(this);
    receiptsEngine receiptsIO;
    m_typeOfChoice = 0;
    choiceDialog * choice = new choiceDialog(this);
    if (choice->exec() == QDialog::Accepted)
    {
    	  m_typeOfChoice = choice->returnChoiceDialog();
    	  insertPreferedValuesIntoAccount();
        }
}

PreferedReceipts::~PreferedReceipts(){}

void PreferedReceipts::insertPreferedValuesIntoAccount(){
    receiptsEngine receiptsIO;
    QString userUuid = user()->value(Core::IUser::Uuid).toString();
    QString patientUid = patient()->data(Core::IPatient::Uid).toString();
    if (patientUid.isEmpty())
    {
    	  patientUid = "no-patient-uid";
        }
    QString patientName = patient()->data(Core::IPatient::FullName).toString();
    if (patientName.isEmpty())
    {
    	  patientName = "Patient Name";
        }
    QHash <int,QVariant> hashOfPrefValues = receiptsIO.getListOfPreferedValues(userUuid,m_typeOfChoice); 
    if (!receiptsIO.insertIntoAccount(hashOfPrefValues,userUuid))
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),trUtf8("Unable to insert datas into account")
    	                                     +__FILE__+QString::number(__LINE__),QMessageBox::Ok);
        }
}
