/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "preferredreceipts.h"
#include "receiptsmanager.h"
#include "ui_preferredreceipts.h"
#include "choicedialog.h"
#include "receiptsio.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <accountbaseplugin/constants.h>

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

using namespace Trans::ConstantTranslations;

enum { WarnDebugMessage = false };

static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

PreferredReceipts::PreferredReceipts(QWidget * parent) :
    QWidget(parent),
    ui(new Ui::PreferredReceiptsWidget)
{
    setObjectName("PreferredReceipts");
    ui->setupUi(this);
    ui->resultLabel->setText("");
    m_typeOfChoice = 0;
    QString userUuid = user()->uuid();
    ReceiptsManager manager;
    if (manager.isMedintuxArg())
    {
        setAttribute(Qt::WA_DeleteOnClose);
    }
    QStringList listOfActs;
    listOfActs = manager.getPreferentialActFromThesaurus(userUuid).keys();
    if (listOfActs.size()>0)
    {
          m_preferredAct = listOfActs[0];
        }
    else
    {
        m_preferredAct = "";
        }

    QTimer::singleShot(100,this,SLOT(showChoiceDialog())) ;

}

PreferredReceipts::~PreferredReceipts(){}

void PreferredReceipts::insertpreferredValuesIntoAccount(){
    QString currency = tr("euros");
    receiptsEngine receiptsIO;
    QString userUuid = user()->uuid();
    QString patientUid = patient()->uuid();
    ReceiptsManager manager;
    if (patientUid.isEmpty())
    {
          patientUid = "no-patient-uid";
          qWarning() << __FILE__ << QString::number(__LINE__) << " no-patient-uid";
        }
    QString patientName = patient()->data(Core::IPatient::FullName).toString();
    if (WarnDebugMessage)
    {
          qDebug() << __FILE__ << QString::number(__LINE__) << " patientName = "<< patientName;
        }
    if (patientName.isEmpty())
    {
          patientName = "Patient Name";
          qWarning() << __FILE__ << QString::number(__LINE__) << " Patient Name";
        }
    //medintux
    if (manager.isMedintuxArg()||manager.isFreaccountIndependant())
    {
        patientName = manager.getFullName();
        }
    QList<double> listOfPercentages;
    listOfPercentages = m_choiceAndPercentagesHash.values();
    QStringList listOfValues;
    QString listOfValuesStr;
    for (int i = 0; i < listOfPercentages.size(); i += 1)
    {
        int typeOfChoice = m_choiceAndPercentagesHash.key(listOfPercentages[i]);
        QHash <int,QVariant> hashOfPrefValues = receiptsIO.getListOfpreferredValues(userUuid,
                                                                                   patientUid,
                                                                                   patientName,
                                                                                   typeOfChoice);

        double preferredValue = hashOfPrefValues.value(Constants::ACCOUNT_CHEQUEAMOUNT+typeOfChoice-1).toDouble();
        if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " preferredValue =" << QString::number(preferredValue) ;
        QVariant debtor = m_choiceAndDebtorHash.value(typeOfChoice);
        if (preferredValue == -1.13)//means does not exist
        {
              qWarning() << __FILE__ << QString::number(__LINE__) << "no preferred value" ;
              Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("You should create a preferredValue like this:\n"
                                                                         "Take a value in thesaurus list with a RIGHT clic,\n"
                                                                         "if you don't have one, save the next value"
                                                                         " in thesaurus and chose it in the same way"));
              return;
            }

        if (m_percent!=100.00)
        {
            hashOfPrefValues.insert(Constants::ACCOUNT_CHEQUEAMOUNT+typeOfChoice-1,preferredValue*listOfPercentages[i]/100.00);
            hashOfPrefValues.insert(Constants::ACCOUNT_INSURANCE_ID,debtor);
            hashOfPrefValues.insert(Constants::ACCOUNT_DUEBY,debtor);
            }

       if (!receiptsIO.insertIntoAccount(hashOfPrefValues,userUuid)) {
           Utils::warningMessageBox(tkTr(Trans::Constants::ERROR), tr("Unable to insert data into account"));
       }
       else{
           listOfValues << QString::number(preferredValue*listOfPercentages[i]/100.00);
           }
        }
    listOfValuesStr = listOfValues.join("+") ;

    const QString resultText = tr("The value ")
                               +m_preferredAct
                               +": "
                               +listOfValuesStr
                               +" "+currency
                               +tr(" has been inserted.");
    const QString resultTextHtml = "<html><body><font size = 4 color = green><bold>"+resultText+"<bold></font>"
                                   "</body></html>";
    ui->resultLabel->setText(resultTextHtml);
}

void PreferredReceipts::showChoiceDialog(){
    ChoiceDialog * choice = new ChoiceDialog(this,false,false,m_preferredAct);
    if (choice->exec() == QDialog::Accepted)
    {
          m_typeOfChoice = choice->returnChoiceDialog();
          //m_percentagesList = choice->listOfPercentValues();
          QStandardItemModel * model = choice->getChoicePercentageDebtorSiteDistruleModel();
          for (int i = 0; i < model->rowCount(); i += 1)
          {
            int TYPE = choice->TYPE_OF_CHOICE;
            int PERCENTAGE = choice->PERCENTAGE;
            int DEBTOR = choice->DEBTOR;
            if (WarnDebugMessage)
              qDebug() << __FILE__ << QString::number(__LINE__) << " TYPE =" <<  QString::number(TYPE);
            int typeOfChoice = model->data(model->index(i,TYPE),Qt::DisplayRole).toInt();
            double percentage = model->data(model->index(i,PERCENTAGE),Qt::DisplayRole).toDouble();
            QVariant debtor = model->data(model->index(i,DEBTOR),Qt::DisplayRole);
                m_choiceAndPercentagesHash.insertMulti(typeOfChoice,percentage);
                m_choiceAndDebtorHash.insertMulti(typeOfChoice,debtor);
              }
          insertpreferredValuesIntoAccount();
          delete model;
          delete choice;
        }
    ReceiptsManager manager;
    if (manager.isMedintuxArg()||manager.isFreaccountIndependant())
    {
        QApplication::closeAllWindows();
    }
}



/*    TYPE_OF_CHOICE = 0,
      PERCENTAGE,
      DEBTOR,
      SITE,
      DISTRULES,*/
