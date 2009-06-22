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
#include <tkUpdateChecker.h>
#include "tkUpdateChecker_p.h"

// include freemedforms headers
#include <tkLog.h>
#include <tkSettings.h>

// include Qt headers
#include <QApplication>
#include <QDialog>
#include <QTextBrowser>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFrame>

Q_TK_USING_CONSTANTS

// Initializing static vars
//tkUpdateChecker *tkUpdateChecker::m_Instance = 0;

//tkUpdateChecker *tkUpdateChecker::instance( QObject *parent )
//{
//    if (!m_Instance) {
//        if (!parent)
//            parent = qApp;
//        m_Instance = new tkUpdateChecker(parent);
//    }
//    return m_Instance;
//}

tkUpdateChecker::tkUpdateChecker( QObject *parent )
          : QObject(parent), d(0)
{
    setObjectName( "tkUpdateChecker" );
//    tkLog::instance()->addObjectWatcher(this);
    d = new tkUpdateCheckerPrivate(this);
}

tkUpdateChecker::~tkUpdateChecker()
{
    if (d) {
        d->cancelDownload();
        delete d;
        d=0;
    }
}

void tkUpdateChecker::check( const QString &url )
{
    tkLog::addMessage( this, tr( "Check for Updates : %1" ).arg( url ) );
    d->getFile( QUrl(url) );
}

void tkUpdateChecker::check( const QUrl &url )
{
    tkLog::addMessage( this, tr( "Check for Updates : %1" ).arg( url.toString() ) );
    d->getFile( url );
}

QString tkUpdateChecker::lastVersion()
{
    return d->m_LastVersion;
}

QString tkUpdateChecker::updateText()
{
    return d->m_UpdateText;
}

bool tkUpdateChecker::hasUpdate()
{
    return (!d->m_UpdateText.isEmpty());
}

bool tkUpdateChecker::fileRetreived()
{
    return d->m_FileRetreived;
}

tkUpdateCheckerPrivate::tkUpdateCheckerPrivate( QObject *parent )
          : QObject(parent)
{
    setObjectName( "tkUpdateChecker" );
    connect( &m_Http, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
    m_FileRetreived = false;
}

tkUpdateCheckerPrivate::~tkUpdateCheckerPrivate()
{
}

bool tkUpdateCheckerPrivate::getFile( const QUrl &url )
{
    if ( !url.isValid() )
        return false;
    if ( url.scheme() != "http" )
        return false;
    if ( url.path().isEmpty() )
        return false;
    m_Url = url;
    m_Http.setHost( m_Url.host(), m_Url.port( 80 ) );
    m_Http.get( m_Url.path(), &m_Buffer );
    m_Http.close();
    return true;
}

void tkUpdateCheckerPrivate::httpDone( bool error )
{
    if (error) {
        tkLog::addError( this, tr( "Error %1 while retreiving update file %2" )
                         .arg(m_Http.errorString())
                         .arg(m_Url.toString()) );
        return;
    }

    QString update = QString::fromUtf8(m_Buffer.data());
    QString forLog = "";
    m_UpdateText.clear();
    m_LastVersion.clear();
    m_FileRetreived = true;

    // Get last version number from update file
    int idLast = 0;
    while (true) {
        idLast = update.indexOf( "version=", idLast );
        if ( idLast == -1 )
            break;
        idLast += 8;
        QString lastVersion = update.mid( idLast, update.indexOf("\n", idLast) - idLast );
        // If necessary keep changelog into update and emit updateFound with the changelog
        if ( qApp->applicationVersion() < lastVersion ) {
            int begin = update.indexOf( "{", idLast ) + 1;
            int end = update.indexOf( "}", begin );
            m_UpdateText.append( tr( "* Updates of version : %1" ).arg( lastVersion ) );
            m_UpdateText.append( update.mid( begin, end - begin ) + "\n" );
            forLog += lastVersion + "; ";
            if (m_LastVersion.isEmpty())
                m_LastVersion = lastVersion;
        }
    }

    if ( m_UpdateText.isEmpty() ) {
        tkLog::addMessage( this, tr( "Version up to date." ) );
        return;
    }

    forLog.chop(2);
    tkLog::addMessage( this, tr( "Update founded : %1" ).arg( forLog ));
    m_UpdateText = m_UpdateText.replace( "\t", "  " );
    static_cast<tkUpdateChecker*>(parent())->emitSignals();
}

void tkUpdateChecker::emitSignals()
{
    Q_EMIT updateFounded(d->m_UpdateText);
    Q_EMIT updateFounded();
}

void tkUpdateCheckerPrivate::cancelDownload()
{
    m_Http.abort();
}

void tkUpdateChecker::showUpdateInformations()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle( tr("Update Available") + " - " + qApp->applicationName() );
    dialog->resize( 500, 400 );
    QGridLayout *layout = new QGridLayout(dialog);
    QLabel *intro = new QLabel( "<b>" + tr("An update is available. Please check the web site : <a href=\"%1\">%1</a>")
                                .arg(tkSettings::instance()->path(tkSettings::WebSiteUrl)) + "</b> " , dialog );
    intro->setWordWrap(true);
    intro->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    intro->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    intro->setOpenExternalLinks(true);

    QFrame *line = new QFrame(dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QLabel *actual = new QLabel( "<b>" + tr("Current version :") + "</b> " + qApp->applicationVersion(), dialog );
    QLabel *uped = new QLabel(   "<b>" + tr("Last version :") + "</b> " + lastVersion(), dialog );
    QTextBrowser *browser = new QTextBrowser(dialog);
    browser->setPlainText(updateText());
    QDialogButtonBox *butBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, dialog);

    layout->addWidget(intro,   0, 0);
    layout->addWidget(line,    1, 0);
    layout->addWidget(actual,  5, 0);
    layout->addWidget(uped,    10, 0);
    layout->addWidget(browser, 15, 0);
    layout->addWidget(butBox,  20, 0);

    connect(butBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    dialog->exec();

    delete dialog;
}
