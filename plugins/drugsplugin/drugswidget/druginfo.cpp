/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
#include "druginfo.h"
#include "druginfo_p.h"

/**
 * \class DrugInfo
 * \brief Show a dialog with drugs information and interactions found.
 * The dialog uses the
 * current drugs view drugsmodel as current drugs model.
 * \remark This dialog was intended to allow user to send debugging data but
 * it was never used in production.
 * \sa Utils::MessageSender
*/

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

//#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/idruginteraction.h>
#include <drugsbaseplugin/interactionmanager.h>

#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/constants.h>

#include <utils/global.h>

#include <QApplication>

using namespace DrugsWidget::Internal;
using namespace DrugsWidget::Constants;

static inline DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }
//static inline DrugsDB::InteractionManager &interactionManager() {return DrugsDB::DrugBaseCore::instance().interactionManager();}

DrugInfo::DrugInfo(const QVariant &drugUid,  QWidget *parent)
    : QDialog (parent), d(0)
{
    d = new DrugInfoPrivate(this);
    setDrug(drugUid);
}

DrugInfoPrivate::DrugInfoPrivate(QDialog *parent)
    : QObject(parent), m_Parent(parent)
{
    setupUi(parent);
    m_INNSent = false;
    m_InteractSent = false;
    parent->setWindowTitle(qApp->applicationName());
    listWidgetInteractions->setStyleSheet("QListWidget#listWidgetInteractions:item { padding: 5px; }");

    // no tests abilities for instance
    Core::ISettings *s = Core::ICore::instance()->settings();
    if ((s->value(S_USER).isValid() &&
           (!s->value(S_USER).toString().isEmpty()))) {
        //           qWarning() << qApp->property("user") << qApp->property("password");
        groupTestINN->show();
        groupTestIAM->hide();
    } else {
        groupTestINN->hide();
        groupTestIAM->hide();
    }

    connect(butSendINN, SIGNAL(clicked()), this, SLOT(on_butSendINN_clicked()));
    connect(butIAMSend, SIGNAL(clicked()), this, SLOT(on_butIAMSend_clicked()));
    connect(listWidgetInteractions, SIGNAL(itemSelectionChanged()),
             this, SLOT(on_listWidgetInteractions_itemSelectionChanged()));
}


bool DrugInfoPrivate::checkSent()
{
    bool sendingMessage = false;
    //     if (((chkAllInteractionsFound->isChecked()) ||
    //               (chkAllInteractionsOK->isChecked()) ||
    //               (chkAllIAMTextsOK->isChecked()) ||
    //               (chkAllCATTextsOK->isChecked()) ||
    //               (!InteractMessage->toPlainText().isEmpty())) &&
    //               (!m_InteractSent))
    //     {
    //          if (QMessageBox::warning(this, qApp->applicationName(),
    //                                     tr("Interactions Information will be lost.\nDo you want to send them?"),
    //                                     QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes)
    //          {
    //               on_butIAMSend_clicked();
    //               sendingMessage = true;
    //          }
    //     }


    if (((rbINNOk->isChecked()) ||
         (rbINNWrong->isChecked()) ||
         (!INNMessage->toPlainText().isEmpty())) &&
         (!m_INNSent)) {
        bool yes = Utils::yesNoMessageBox(tr("INN Information will be lost.\nDo you want to send them?"),
                                          tr("INN Information will be lost.\nDo you want to send them?"));
        if (yes) {
            on_butIAMSend_clicked();
            sendingMessage = true;
        }
    }
    return sendingMessage;
}

void DrugInfo::accept()
{
    if (d->checkSent())
        connect(&d->m_Sender, SIGNAL(sent()) , SLOT(done()));
    else QDialog::accept();
}

void DrugInfo::reject()
{
    if (d->checkSent())
        connect(&d->m_Sender, SIGNAL(sent()) , SLOT(done()));
    else QDialog::reject();
}


void DrugInfo::setDrug(const QVariant &drugUid)
{
    using namespace DrugsDB::Constants;
    d->m_DrugUid = drugUid;
    // manage drugs information
    d->drugName->setText(drugModel()->drugData(d->m_DrugUid, Drug::Denomination).toString());
    d->knownMols->addItems(drugModel()->drugData(d->m_DrugUid, Drug::Molecules).toStringList()); //drug->listOfMolecules());
    d->DCI->addItems(drugModel()->drugData(d->m_DrugUid, Drug::Inns).toStringList()); //drug->listOfInn());
    d->interactClass->addItems(drugModel()->drugData(d->m_DrugUid, Drug::InnClasses).toStringList()); //drug->listOfInnClasses());

    // manage interactions information
    d->m_InteractionsList.clear();
    d->CAT_textBrower->clear();
    d->Info_textBrowser->clear();
    d->listWidgetInteractions->clear();
    QString display;
    if (drugModel()->drugData(d->m_DrugUid, Drug::Interacts).toBool()) { //mfDrugsBase::instance()->drugHaveInteraction(m_Drug)) {
        // TODO: code here
//        d->m_InteractionsList = DrugsWidget::DrugsWidgetManager::instance()->currentInteractionManager()->getAllInteractionsFound();
        // populate the listwidget with found interactions
//        foreach(DrugsDB::IDrugInteraction *di , d->m_InteractionsList) {
//            new QListWidgetItem(drugModel()->drugData(d->m_DrugUid, Interaction::Icon).value<QIcon>(), di->header(), d->listWidgetInteractions);
//        }
    }
}

void DrugInfoPrivate::on_listWidgetInteractions_itemSelectionChanged()
{
    int id = listWidgetInteractions->currentRow();
    if (id >= m_InteractionsList.count()) return;
    Info_textBrowser->setHtml(m_InteractionsList[id]->risk());
    CAT_textBrower->setHtml(m_InteractionsList[id]->management());
}

void DrugInfoPrivate::on_butIAMSend_clicked()
{
    if (drugModel()->drugsList().isEmpty())
        return;

    // prepare message to send
    QString msg;
    msg = tr("Testing: ") + "\n";
    foreach(DrugsDB::IDrug * drug, drugModel()->drugsList())
        msg += drug->brandName() + "\n";

    // manage INN (DCI)
    if (chkAllInteractionsFound->isChecked()) {
        msg += tr("All interactions found.\n");
    } else {
        msg += tr("*** Missing interactions.\n");
    }

    if (chkAllInteractionsOK->isChecked())  {
        msg += tr("All interactions are OK.\n");
    } else {
        msg += tr("*** Wrong interactions.\n");
    }

    if (chkAllIAMTextsOK->isChecked()) {
        msg += tr("IAM Text is correct.\n");
    } else {
        msg += tr("*** IAM Text is incorrect.\n");
    }

    if (chkAllCATTextsOK->isChecked()) {
        msg += tr("CAT Text is correct.\n");
    } else {
        msg += tr("*** CAT Text is incorrect.\n");
    }

    msg += tr("Message: ") + InteractMessage->toPlainText() + "\n";

    if ((chkAllInteractionsFound->isChecked()) &&
        (chkAllInteractionsOK->isChecked()) &&
        (chkAllIAMTextsOK->isChecked()) &&
        (chkAllCATTextsOK->isChecked())) {
        msg += "\n" + tr("Checked interactions: ") + "\n";
        foreach(DrugsDB::IDrug *drug, drugModel()->drugsList()) {
            Q_UNUSED(drug);
            foreach(QVariant code, drugModel()->drugData(m_DrugUid,DrugsDB::Constants::Drug::CodeMoleculesList).toList())
                msg +=  code.toString() + "\n";
        }
    }

    m_Sender.setParent(m_Parent);
    m_Sender.setUser("eric");
    m_Sender.setMessage(msg);
    m_Sender.postMessage();
    m_InteractSent = true;
}

void DrugInfoPrivate::on_butSendINN_clicked()
{
    // prepare message to send
    QString msg;
    Utils::MessageSender::TypeOfMessage t;

    if (rbINNOk->isChecked()) {
        foreach(QVariant code,  drugModel()->drugData(m_DrugUid, DrugsDB::Constants::Drug::CodeMoleculesList).toList()) //m_Drug->listOfCodeMolecules().toList())
            msg +=  code.toString() + "\n";
        t = Utils::MessageSender::CorrectDrugsCoding;
    } else {
        msg.append(tr("ERROR: %1\n").arg(drugModel()->drugData(m_DrugUid, DrugsDB::Constants::Drug::Denomination).toString()));
        msg.append(QString("{\n %1 \n}\n").arg(INNMessage->toPlainText()));
        t = Utils::MessageSender::UncorrectDrugsCoding;
    }
    m_Sender.setTypeOfMessage(t);
    m_Sender.setParent(m_Parent);
    // TODO User name
    m_Sender.setUser("eric");
    m_Sender.setMessage(msg);
    m_Sender.postMessage();
    m_INNSent = true;
}

