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
#include "tkDebugDialog.h"
#include "tkDebugDialog_p.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkSettings.h>

// include Qt headers
#include <Qt>
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>

//--------------------------------------------------------------------------------------------------------
//-------------------------------------------- PUBLIC PART -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
tkDebugDialog::tkDebugDialog( QWidget * parent, tkSettings * settings ) :
    QDialog( parent )
{
    Q_ASSERT_X( settings, "tkDebugDialog", "No tkSettings passed to constructor." );
    setObjectName( "tkDebugDialog" );
    // some initialization
    d = new tkDebugDialogPrivate( this, settings );
    d->initialize();
    setWindowTitle( qApp->applicationName() );
}

void tkDebugDialog::done( int code )
{
     if ( d->m_sending ) {
        tkGlobal::informativeMessageBox( tr( "You can not close this window." ),
                                         tr( "Is sending your debugging informations. You must wait "
                                             "the end of this process." ),
                                         "");
      }
     else
          QDialog::done( code );
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------- PRIVATE PART -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
tkDebugDialogPrivate::tkDebugDialogPrivate( QObject * parent, tkSettings * settings )
         : QObject( parent ), m_Parent( 0 ), m_settings( 0 ), m_MessageViewExpanded(true), m_ErrorViewExpanded(true)
{
    m_settings = settings;
    m_Parent = static_cast<QDialog*>( parent );
    setObjectName( "tkDebugDialogPrivate" );
}

void tkDebugDialogPrivate::initialize()
{
    setupUi(m_Parent);
    tabWidget->setCurrentIndex(0);
    // create connections
    connect( butSend, SIGNAL( clicked() ) , this, SLOT(on_butSend_clicked()) );
    connect( butSave, SIGNAL( clicked() ) , this, SLOT(on_butSave_clicked()) );
    connect( chronologicMessagesButton, SIGNAL( clicked() ) , this, SLOT(on_chronologicMessagesButton_clicked()) );
    connect( expandedMessagesButton, SIGNAL( clicked() ) , this, SLOT(on_expandedMessagesButton_clicked()) );
    connect( chronologicErrorButton, SIGNAL( clicked() ) , this, SLOT(on_chronologicErrorButton_clicked()) );
    connect( expandedErrorButton, SIGNAL( clicked() ) , this, SLOT(on_expandedErrorButton_clicked()) );
    m_sending = false;

    if (!tkGlobal::isDebugCompilation())
        butSend->setEnabled( tkLog::hasError() );

    // add settings to ui
    lblFileName->setText( QFileInfo( m_settings->fileName() ).fileName() );
    // add compilation informations
    m_settings->getTreeWidget( treeWidgetSettings );

    // add logs to ui
    tkLog::messagesToTreeWidget(treeMessages, m_MessageViewExpanded);
    tkLog::errorsToTreeWidget(treeErrors, m_ErrorViewExpanded);

    // resize columns
    treeMessages->resizeColumnToContents( 0 );
    treeMessages->resizeColumnToContents( 1 );
    treeErrors->resizeColumnToContents( 0 );
    treeErrors->resizeColumnToContents( 1 );
}


void tkDebugDialogPrivate::on_butSend_clicked()
{
    if (!tkGlobal::isDebugCompilation()) {
        if (!tkLog::hasError())  // this should never be in this member
            return;
    }
    tkLog::addMessage( this, tr( "Starting to send debugging informations" ) );

    QString msg = tkGlobal::askUser( tr("Starting to send debugging informations"), tr("Please describe your problem") );
    // get full log including settings and logs
    msg += "\n\n" + tkLog::toString( tkSettings::instance()->toString() );

    // send informations
    connect( &m_sender, SIGNAL( sent() ), this, SLOT( onSendMessage_done() ) );
    m_sender.setTypeOfMessage( tkSendMessage::InformationToDevelopper );
    m_sender.setMessage( msg );
    if (m_sender.postMessage()) {
        m_sending = true;
        // showing a messagebox
        m_infoMessageBox = new QMessageBox( m_Parent );
        m_infoMessageBox->setText( tr( "Sending debugging informations to dev team") );
        m_infoMessageBox->setInformativeText( tr("Trying to send informations to dev team.\n"
                                                 "Using Url : %1 \n"
                                                 "Please wait..." ).arg( m_sender.usedUrl() ) );
        m_infoMessageBox->setWindowTitle( qApp->applicationName() );
        m_infoMessageBox->setStandardButtons( QMessageBox::Ok );
        m_infoMessageBox->show();
    } else {
        m_sending = false;
    }
}

bool tkDebugDialogPrivate::on_butSave_clicked()
{
    Q_ASSERT_X( m_settings, "tkDebugDialog", "No tkSettings : can proceed member on_butSave_clicked()." );
    QString fileName = m_settings->path( tkSettings::ResourcesPath ) + "/logs.txt" ;
    tkLog::addMessage( this, tr( "Saving debugging logs to %1." ).arg( fileName ) );
    return tkGlobal::saveStringToFile( tkLog::toString(), fileName );
}

void tkDebugDialogPrivate::onSendMessage_done()
{
    tkLog::addMessage( this, tr( "Debugging informations correctly sent." ) );
    if ( m_infoMessageBox )
    {
        m_infoMessageBox->setInformativeText( tr("Debugging informations correctly send to dev team.\n"
                                             "Using Url : %1 \n"
                                             "%2" ).arg( m_sender.usedUrl(), m_sender.resultMessage() ) );
        m_infoMessageBox->exec();
        delete m_infoMessageBox;
        m_infoMessageBox=0;
    }
    m_sending = false;
}

void tkDebugDialogPrivate::on_expandedMessagesButton_clicked()
{
    if (m_MessageViewExpanded)
        return;
    m_MessageViewExpanded = true;
    tkLog::messagesToTreeWidget(treeMessages, m_MessageViewExpanded);
}

void tkDebugDialogPrivate::on_chronologicMessagesButton_clicked()
{
    if (!m_MessageViewExpanded)
        return;
    m_MessageViewExpanded = false;
    tkLog::messagesToTreeWidget(treeMessages, m_MessageViewExpanded);
}

void tkDebugDialogPrivate::on_expandedErrorButton_clicked()
{
    if (m_ErrorViewExpanded)
        return;
    m_ErrorViewExpanded = true;
    tkLog::errorsToTreeWidget(treeMessages, m_ErrorViewExpanded);
}

void tkDebugDialogPrivate::on_chronologicErrorButton_clicked()
{
    if (!m_ErrorViewExpanded)
        return;
    m_ErrorViewExpanded = false;
    tkLog::messagesToTreeWidget(treeMessages, m_ErrorViewExpanded);
}
