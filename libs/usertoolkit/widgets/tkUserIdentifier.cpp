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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
 \class tkUSerIdentifier
  \brief This class is a dialog that ask user for is login/password, with a limited number of tries.
  You can show some informations on the left using the first parameter of the constructor.
  If there is not informations to show, the informations' groupBox is hidden.

  When the identification is good :
  - the dialog result() is setted to QDialog::Accepted,
  - current user is setted into user's model with the login and password of this dialog,
  - the lastLogin information is saved into database,
  - the login history is completed.

  In the other case, it is setted to QDialog::Rejected.

  You can retreive cryptedLogin and cryptedPassword using : login() and cryptedPassword().
  \ingroup usertoolkit widget_usertoolkit
  \ingroup usermanager
*/

#include "tkUserIdentifier.h"
#include "tkUserIdentifier_p.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTheme.h>
#include <tkLineEditEchoSwitcher.h>

// include usertoolkit headers
#include <tkUserGlobal.h>
#include <tkUserModel.h>

// include Qt headers
#include <QApplication>
#include <QMessageBox>

Q_TKUSER_USING_GLOBAL
Q_TK_USING_CONSTANTS

tkUserIdentifier::tkUserIdentifier( const QStringList & informations, QWidget * parent ) :
    QDialog( parent )
{
    // initialization
    setObjectName( "tkUserIdentifier" );
    d = new tkUserIdentifierPrivate( this );
    d->m_NumberOfTries = 0;
    d->initialize();
    setWindowTitle( qApp->applicationName() );
    if ( ! parent )
        tkGlobal::centerWidget( this );
    if ( informations.count() ) {
        foreach( const QString & s, informations )
            new QListWidgetItem( s , d->informationsWidget );
    } else
        d->groupInformations->hide();
}

void tkUserIdentifierPrivate::initialize()
{
    setupUi( m_Parent );
    lblAppName->setText( qApp->applicationName() );
    password->toogleEchoMode();
}

void tkUserIdentifier::done( int result )
{
    tkUserModel *m = tkUserModel::instance();
    if ( result == QDialog::Accepted ) {
        // ask database with lgin/password couple
        if ( ! m->isCorrectLogin(loginForSQL(d->login->lineEdit()->text() ), crypt( d->password->lineEdit()->text())) ) {
            d->m_NumberOfTries++;
            if ( d->m_NumberOfTries == MaxNumberOfTries )
                QDialog::done( QDialog::Rejected );
            else {
                QMessageBox mb( this );
                mb.setIcon( QMessageBox::Critical );
                mb.setWindowTitle( qApp->applicationName() );
                mb.setText( tr( "Incorrect login/password informations." ) );
                mb.setInformativeText( tr( "You can try %1 more time(s)." )
                                       .arg( MaxNumberOfTries - d->m_NumberOfTries ) );
                mb.setStandardButtons(QMessageBox::Ok);
                mb.setDefaultButton(QMessageBox::Ok);
                mb.exec();
            }
        } else {
            tkLog::addMessage( this, tr( "User is identified." ) );
            m->setCurrentUser( login(), cryptedPassword() );
            QDialog::done( QDialog::Accepted );
        }
    }
    else if ( result == QDialog::Rejected ) {
        tkLog::addMessage( this, tr( "User is not identified." ) );
        QDialog::done( QDialog::Rejected );
    }
}

inline QString tkUserIdentifier::cryptedPassword()
{
    return crypt( d->password->lineEdit()->text() );
}

inline QString tkUserIdentifier::login()
{
    return loginForSQL( d->login->lineEdit()->text() );
}
