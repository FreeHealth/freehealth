/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "druginfo.h"
#include "druginfo_p.h"

/**
  \class DrugInfo
  \brief Show a dialog with drugs informations and interactions founded.
  This dialog allows user to send debugging datas.
  \ingroup freediams drugswidget
*/

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

#include <drugsbaseplugin/drugsdata.h>
#include <drugsbaseplugin/drugsmodel.h>
#include <drugsbaseplugin/drugsinteraction.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/interactionsmanager.h>

#include <drugsplugin/drugswidgetmanager.h>
#include <drugsplugin/constants.h>


// include Qt headers
#include <QMessageBox>
#include <QApplication>

using namespace DrugsWidget::Internal;
using namespace DrugsWidget::Constants;

inline static DrugsDB::DrugsModel *drugModel() { return DrugsWidget::DrugsWidgetManager::instance()->currentDrugsModel(); }

DrugInfo::DrugInfo( const int m_CIS,  QWidget *parent )
          : QDialog( parent ), d(0)
{
    d = new DrugInfoPrivate(this);
    setDrug(m_CIS);
}

DrugInfoPrivate::DrugInfoPrivate( QDialog *parent )
    : QObject(parent), m_Parent(parent)
{
    setupUi(parent);
    m_INNSent = false;
    m_InteractSent = false;
    parent->setWindowTitle( qApp->applicationName() );
    listWidgetInteractions->setStyleSheet( "QListWidget#listWidgetInteractions:item { padding: 5px; }" );

    // no tests abilities for instance
    Core::ISettings *s = Core::ICore::instance()->settings();
    if ( ( s->value(S_USER ).isValid() &&
           ( !s->value(S_USER ).toString().isEmpty() ) ) ) {
        //           qWarning() << qApp->property( "user" ) << qApp->property( "password" );
        groupTestINN->show();
        groupTestIAM->hide();
    } else {
        groupTestINN->hide();
        groupTestIAM->hide();
    }

    connect( butSendINN, SIGNAL(clicked()), this, SLOT(on_butSendINN_clicked()));
    connect( butIAMSend, SIGNAL(clicked()), this, SLOT(on_butIAMSend_clicked()));
    connect( listWidgetInteractions, SIGNAL(itemSelectionChanged()),
             this, SLOT(on_listWidgetInteractions_itemSelectionChanged()));
}


bool DrugInfoPrivate::checkSent()
{
    bool sendingMessage = false;
    //     if ((( chkAllInteractionsFound->isChecked() ) ||
    //               ( chkAllInteractionsOK->isChecked() ) ||
    //               ( chkAllIAMTextsOK->isChecked() ) ||
    //               ( chkAllCATTextsOK->isChecked() ) ||
    //               ( !InteractMessage->toPlainText().isEmpty() ) ) &&
    //               ( !m_InteractSent ) )
    //     {
    //          if ( QMessageBox::warning( this, qApp->applicationName(),
    //                                     tr( "Interactions Informations will be lost.\nDo you want to send them ?" ),
    //                                     QMessageBox::No | QMessageBox::Yes ) == QMessageBox::Yes )
    //          {
    //               on_butIAMSend_clicked();
    //               sendingMessage = true;
    //          }
    //     }


    if ( ( ( rbINNOk->isChecked() ) ||
           ( rbINNWrong->isChecked() ) ||
           ( !INNMessage->toPlainText().isEmpty() ) ) &&
         ( !m_INNSent ) ) {
        if ( QMessageBox::warning( m_Parent, qApp->applicationName(),
                                   tr( "INN Informations will be lost.\nDo you want to send them ?" ),
                                   QMessageBox::No | QMessageBox::Yes ) == QMessageBox::Yes ) {
            on_butIAMSend_clicked();
            sendingMessage = true;
        }
    }
    return sendingMessage;
}


void DrugInfo::accept()
{
    if ( d->checkSent() )
        connect( &d->m_Sender, SIGNAL( sent() ) , SLOT( done() ) );
    else QDialog::accept();
}

void DrugInfo::reject()
{
    if ( d->checkSent() )
        connect( &d->m_Sender, SIGNAL( sent() ) , SLOT( done() ) );
    else QDialog::reject();
}


void DrugInfo::setDrug( const int CIS )
{
    using namespace DrugsDB::Constants;
    d->m_CIS = CIS;
    // manage drugs informations
    d->drugName->setText( drugModel()->drugData( d->m_CIS, Drug::Denomination ).toString() );
    d->knownMols->addItems( drugModel()->drugData( d->m_CIS, Drug::Molecules ).toStringList() ); //drug->listOfMolecules() );
    d->DCI->addItems( drugModel()->drugData( d->m_CIS, Drug::Inns ).toStringList() ); //drug->listOfInn() );
    d->interactClass->addItems( drugModel()->drugData( d->m_CIS, Drug::InnClasses ).toStringList() ); //drug->listOfInnClasses() );

    // manage interactions informations
    d->m_InteractionsList.clear();
    d->CAT_textBrower->clear();
    d->Info_textBrowser->clear();
    d->listWidgetInteractions->clear();
    QString display;
    if ( drugModel()->drugData( d->m_CIS, Drug::Interacts ).toBool() ) { //mfDrugsBase::instance()->drugHaveInteraction( m_Drug ) ) {
        d->m_InteractionsList = DrugsWidget::DrugsWidgetManager::instance()->currentInteractionManager()->getAllInteractionsFound();
//        d->m_InteractionsList = mfDrugsBase::instance()->getInteractions( m->drugData( d->m_CIS, Drug::CIS ).toInt() );
        // populate the listwidget with founded interactions
        foreach( DrugsDB::Internal::DrugsInteraction * di , d->m_InteractionsList ) {
            new QListWidgetItem( drugModel()->drugData( d->m_CIS, Interaction::Icon ).value<QIcon>(), di->header(), d->listWidgetInteractions );
        }
    }

}

void DrugInfoPrivate::on_listWidgetInteractions_itemSelectionChanged()
{
    int id = listWidgetInteractions->currentRow();
    if ( id >= m_InteractionsList.count() ) return;
    Info_textBrowser->setHtml( m_InteractionsList[ id ]->information() );
    CAT_textBrower->setHtml( m_InteractionsList[ id ]->whatToDo() );
}

void DrugInfoPrivate::on_butIAMSend_clicked()
{
    if ( drugModel()->drugsList().isEmpty() )
        return;

    // prepare message to send
    QString msg;
    msg = tr( "Testing : " ) + "\n";
    foreach( DrugsDB::Internal::DrugsData * drug, drugModel()->drugsList() )
        msg += drug->denomination() + "\n";

    // manage INN (DCI)
    if ( chkAllInteractionsFound->isChecked() ) {
        msg += tr( "All interactions found.\n" );
    } else {
        msg += tr( "*** Missing interactions.\n" );
    }

    if ( chkAllInteractionsOK->isChecked() )  {
        msg += tr( "All interactions are OK.\n" );
    } else {
        msg += tr( "*** Wrong interactions.\n" );
    }

    if ( chkAllIAMTextsOK->isChecked() ) {
        msg += tr( "IAM Text is correct.\n" );
    } else {
        msg += tr( "*** IAM Text is incorrect.\n" );
    }

    if ( chkAllCATTextsOK->isChecked() ) {
        msg += tr( "CAT Text is correct.\n" );
    } else {
        msg += tr( "*** CAT Text is incorrect.\n" );
    }

    msg += tr( "Message : " ) + InteractMessage->toPlainText() + "\n";

    if (( chkAllInteractionsFound->isChecked() ) &&
        ( chkAllInteractionsOK->isChecked() ) &&
        ( chkAllIAMTextsOK->isChecked() ) &&
        ( chkAllCATTextsOK->isChecked() ) ) {
        msg += "\n" + tr( "Checked interactions : " ) + "\n";
        foreach( DrugsDB::Internal::DrugsData *drug, drugModel()->drugsList() ) {
            Q_UNUSED(drug);
            foreach( QVariant code,  drugModel()->drugData(m_CIS,DrugsDB::Constants::Drug::CodeMoleculesList).toList() )
                msg +=  code.toString() + "\n";
        }
    }

    m_Sender.setParent( m_Parent );
    m_Sender.setUser( "eric" );
    m_Sender.setMessage( msg );
    m_Sender.postMessage();
    m_InteractSent = true;
}

void DrugInfoPrivate::on_butSendINN_clicked()
{
    // prepare message to send
    QString msg;
    Utils::MessageSender::typeOfMessage t;

    if ( rbINNOk->isChecked() ) {
        foreach( QVariant code,  drugModel()->drugData( m_CIS, DrugsDB::Constants::Drug::CodeMoleculesList).toList()) //m_Drug->listOfCodeMolecules().toList() )
            msg +=  code.toString() + "\n";
        t = Utils::MessageSender::CorrectDrugsCoding;
    } else {
        msg.append( tr( "ERROR : %1\n" ).arg( drugModel()->drugData( m_CIS, DrugsDB::Constants::Drug::Denomination).toString() ) );
        msg.append( QString( "{\n %1 \n}\n").arg( INNMessage->toPlainText() ) );
        t = Utils::MessageSender::UncorrectDrugsCoding;
    }
    m_Sender.setTypeOfMessage( t );
    m_Sender.setParent( m_Parent );
    // TODO User name
    m_Sender.setUser( "eric" );
    m_Sender.setMessage( msg );
    m_Sender.postMessage();
    m_INNSent = true;
}

