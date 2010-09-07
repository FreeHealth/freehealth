/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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

/**
  \class Utils::UpdateChecker
  \brief Threaded update checker over the internet. Connect the SIGNAL to catch the update.
  - Usage :
  \code
      Utils::UpdateChecker::instance()->check("http://my.url.com/update.file.txt");
      connect( Utils::UpdateChecker::instance(), SIGNAL(updateFound()), this, SLOT(on_UpdateFound()));
      // OR
      connect( Utils::UpdateChecker::instance(), SIGNAL(updateFound(const QString &)), this, SLOT(on_UpdateFound(const QString &)));
      // OR
      connect( Utils::UpdateChecker::instance(), SIGNAL(updateFound()), UpdateChecker::instance(), SLOT(showUpdateInformations()));
  \endcode

  - You can :
      - check for update with hasUpdate(),
      - get the extracted update's text using updateText(),
      - stop the downloading of the file using cancel().
*/

#include "updatechecker.h"
#include "updatechecker_p.h"
#include <utils/log.h>

#include <translationutils/constanttranslations.h>

#include <QApplication>
#include <QDialog>
#include <QTextBrowser>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFrame>
#include <QDate>


using namespace Utils;
using namespace Utils::Internal;
using namespace Trans::ConstantTranslations;


namespace Utils {
namespace Internal {

UpdateCheckerPrivate::UpdateCheckerPrivate( QObject *parent )
          : QObject(parent), m_ProgressBar(0)
{
    setObjectName( "UpdateChecker" );
    m_Http = new QHttp(this);
    connect(m_Http, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
    connect(m_Http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(updateDataReadProgress(int, int)));
    m_FileRetreived = false;
}

UpdateCheckerPrivate::~UpdateCheckerPrivate()
{
}

bool UpdateCheckerPrivate::getFile( const QUrl &url )
{
    Log::addMessage( this, "getFile" );
    if ((!url.isValid()) || (url.scheme() != "http") || (url.path().isEmpty())) {
        Q_EMIT static_cast<UpdateChecker*>(parent())->done(false);
        return false;
    }
    m_Url = url;
    m_Http->setHost(m_Url.host(), m_Url.port(80));
    m_Http->get(m_Url.path(), &m_Buffer);
    m_Http->close();
    return true;
}

void UpdateCheckerPrivate::updateDataReadProgress(int bytesRead, int totalBytes)
{
//    if (httpRequestAborted)
//        return;
    if (m_ProgressBar) {
        m_ProgressBar->setMaximum(totalBytes);
        m_ProgressBar->setValue(bytesRead);
    }
}

void UpdateCheckerPrivate::httpDone(bool error)
{
    Q_EMIT static_cast<UpdateChecker*>(parent())->done(error);
    if (error) {
        Log::addError( this, tr( "Error %1 while retreiving update file %2" )
                         .arg(m_Http->errorString())
                         .arg(m_Url.toString()), __FILE__, __LINE__);
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
        Log::addMessage( this, tkTr(Trans::Constants::VERSION_UPTODATE) );
        return;
    }

    forLog.chop(2);
    Log::addMessage( this, tkTr(Trans::Constants::UPDATE_AVAILABLE) + ": " + forLog);
    m_UpdateText = m_UpdateText.replace( "\t", "  " );
    static_cast<UpdateChecker*>(parent())->emitSignals();
}

/** \brief Abort the download */
void UpdateCheckerPrivate::cancelDownload()
{
    m_Http->abort();
}

} // End Internal
} // End Utils


/** \brief Constructor */
UpdateChecker::UpdateChecker( QObject *parent )
          : QObject(parent), d(0)
{
    setObjectName("UpdateChecker");
    d = new Internal::UpdateCheckerPrivate(this);
}

/** \brief Destructor */
UpdateChecker::~UpdateChecker()
{
    if (d) {
        d->cancelDownload();
        delete d;
        d=0;
    }
}

bool UpdateChecker::needsUpdateChecking(QSettings *settings) const
{
    int chk = settings->value(Constants::S_CHECKUPDATE, Check_AtStartup).toInt();
    QDate last = settings->value(Constants::S_LAST_CHECKUPDATE, QDate::currentDate()).toDate();
    if ((chk == Check_AtStartup)
        || ((chk == Check_EachWeeks) && (last.addDays(7) < QDate::currentDate()))
        || ((chk == Check_EachMonth) && (last.addMonths(1) < QDate::currentDate()))
        || ((chk == Check_EachQuarters) && (last.addMonths(3) < QDate::currentDate())) ) {
        return true;
    }
    return false;
}

bool UpdateChecker::isChecking() const
{
    return (d->m_Http->currentId() != 0);
}

/**
  \brief Check for update using the URL \e url.
  \li Download the selected file
  \li Find the latest version notified in it
  \li Compare with the QApplication::applicationVersion() string
*/
void UpdateChecker::check(const QString &url)
{
    Log::addMessage(this, tkTr(Trans::Constants::CHECKING_UPDATE_FROM_1).arg(url));
    d->getFile(QUrl(url));
}

/**
  \brief Check for update using the URL \e url.
  \sa check()
*/
void UpdateChecker::check(const QUrl &url)
{
    Log::addMessage( this, tkTr(Trans::Constants::CHECKING_UPDATE_FROM_1).arg(url.toString()));
    d->getFile(url);
}

void UpdateChecker::cancel()
{
    d->cancelDownload();
}

QProgressBar *UpdateChecker::progressBar(QWidget *w)
{
    if (!d->m_ProgressBar)
        d->m_ProgressBar = new QProgressBar(w);
    return d->m_ProgressBar;
}

/**
  \brief Returns the latest version notified in the downloaded update file
*/
QString UpdateChecker::lastVersion()
{
    return d->m_LastVersion;
}

/**
  \brief Returns the update text for the latest version notified in the downloaded update file
*/
QString UpdateChecker::updateText()
{
    return d->m_UpdateText;
}

/**
  \brief Returns true if there is a later version notified in the downloaded update file
*/
bool UpdateChecker::hasUpdate()
{
    return (!d->m_UpdateText.isEmpty());
}

/**
  \brief Returns true if the update file was correctly downloaded
*/
bool UpdateChecker::fileRetreived()
{
    return d->m_FileRetreived;
}

/** \internal */
void UpdateChecker::emitSignals()
{
    Q_EMIT updateFound(d->m_UpdateText);
    Q_EMIT updateFound();
}


/**
  \brief Opens a dialog window that show all the update text available
*/
void UpdateChecker::showUpdateInformations()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle( tkTr(Trans::Constants::UPDATE_AVAILABLE) + " - " + qApp->applicationName() );
    dialog->resize( 500, 400 );
    QGridLayout *layout = new QGridLayout(dialog);
    QLabel *intro = new QLabel( "<b>" + tr("An update is available. Please check the web site : <a href=\"%1\">%1</a>")
                                .arg(qApp->organizationDomain()) + "</b> " , dialog );
    intro->setWordWrap(true);
    intro->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    intro->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    intro->setOpenExternalLinks(true);

    QFrame *line = new QFrame(dialog);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QLabel *actual = new QLabel( "<b>" + tkTr(Trans::Constants::CURRENT_VERSION) + "</b> " + qApp->applicationVersion(), dialog );
    QLabel *uped = new QLabel(   "<b>" + tkTr(Trans::Constants::LATEST_VERSION) + "</b> " + lastVersion(), dialog );
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
