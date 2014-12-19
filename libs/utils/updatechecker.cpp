/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
      connect( Utils::UpdateChecker::instance(), SIGNAL(updateFound()), UpdateChecker::instance(), SLOT(showUpdateInformation()));
  \endcode

  - You can :
      - check for update with hasUpdate(),
      - get the extracted update's text using updateText(),
      - stop the downloading of the file using cancel().
*/

#include "updatechecker.h"
#include "updatechecker_p.h"

#include <utils/log.h>
#include <utils/httpdownloader.h>
#include <translationutils/constants.h>
#include <translationutils/trans_spashandupdate.h>

#include <QApplication>
#include <QDialog>
#include <QTextBrowser>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QFrame>
#include <QDate>
#include <QSettings>

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {

UpdateCheckerPrivate::UpdateCheckerPrivate( QObject *parent )
    : QObject(parent), m_ProgressBar(0)
{
    setObjectName("UpdateChecker");
    _downloader = new Utils::HttpDownloader(this);
    _downloader->setStoreInBuffer(true);
    connect(_downloader, SIGNAL(downloadFinished()), this, SLOT(onDownloadFinished()));
}

UpdateCheckerPrivate::~UpdateCheckerPrivate()
{
    _downloader->cancelDownload();
}

bool UpdateCheckerPrivate::getFile(const QUrl &url)
{
    LOG("getFile");
    if ((!url.isValid()) || (url.scheme() != "http") || (url.path().isEmpty())) {
        Q_EMIT qobject_cast<UpdateChecker*>(parent())->done(false);
        return false;
    }
    m_Url = url;
    _downloader->setUrl(m_Url);
    return _downloader->startDownload();
}

void UpdateCheckerPrivate::onDownloadFinished()
{
    bool error = _downloader->networkError() != QNetworkReply::NoError;

    Q_EMIT static_cast<UpdateChecker*>(parent())->done(error);
    if (error) {
        LOG_ERROR(tr( "Error %1 while retrieving update file %2" )
                         .arg(_downloader->lastErrorString())
                         .arg(m_Url.toString()));
        return;
    }

    QString update = QString::fromUtf8(_downloader->getBufferContent());
    QString forLog = "";
    m_UpdateText.clear();
    m_LastVersion.clear();
    m_FileRetrieved = true;

    // Get last version number from update file
    int idLast = 0;
    while (true) {
        idLast = update.indexOf( "version=", idLast );
        if (idLast == -1)
            break;
        idLast += 8;
        QString lastVersion = update.mid(idLast, update.indexOf("\n", idLast) - idLast);

        // TODO: Use Utils::VersionNumber

        // If necessary keep changelog into update and emit updateFound with the changelog
        if (qApp->applicationVersion() < lastVersion) {
            int begin = update.indexOf( "{", idLast ) + 1;
            int end = update.indexOf( "}", begin );
            m_UpdateText.append( tr( "* Updates of version: %1" ).arg( lastVersion ) );
            m_UpdateText.append( update.mid( begin, end - begin ) + "\n" );
            forLog += lastVersion + "; ";
            if (m_LastVersion.isEmpty())
                m_LastVersion = lastVersion;
        }
    }

    if ( m_UpdateText.isEmpty() ) {
        LOG(tkTr(Trans::Constants::VERSION_UPTODATE));
        return;
    }

    forLog.chop(2);
    LOG(tkTr(Trans::Constants::UPDATE_AVAILABLE) + ": " + forLog);
    m_UpdateText = m_UpdateText.replace( "\t", "  " );
    static_cast<UpdateChecker*>(parent())->emitSignals();
}

/** \brief Abort the download */
void UpdateCheckerPrivate::cancelDownload()
{
    _downloader->cancelDownload();
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
    int chk = settings->value(Constants::S_CHECKUPDATE, Trans::Constants::CheckUpdate_AtStartup).toInt();
    QDate last = settings->value(Constants::S_LAST_CHECKUPDATE, QDate::currentDate()).toDate();
    if ((chk == Trans::Constants::CheckUpdate_AtStartup)
        || ((chk == Trans::Constants::CheckUpdate_EachWeeks) && (last.addDays(7) < QDate::currentDate()))
        || ((chk == Trans::Constants::CheckUpdate_EachMonth) && (last.addMonths(1) < QDate::currentDate()))
        || ((chk == Trans::Constants::CheckUpdate_EachQuarters) && (last.addMonths(3) < QDate::currentDate())) ) {
        return true;
    }
    return false;
}

/** \todo Code  this */
bool UpdateChecker::isChecking() const
{
    // TODO: code here
    return false;
//    return (d->_downloader-
}

/**
  \brief Check for update using the URL \e url.
  \li Download the selected file
  \li Find the latest version notified in it
  \li Compare with the QApplication::applicationVersion() string
*/
void UpdateChecker::check(const QString &url)
{
    LOG(tkTr(Trans::Constants::CHECKING_UPDATE_FROM_1).arg(url));
    d->getFile(QUrl(url));
}

/**
  \brief Check for update using the URL \e url.
  \sa check()
*/
void UpdateChecker::check(const QUrl &url)
{
    LOG(tkTr(Trans::Constants::CHECKING_UPDATE_FROM_1).arg(url.toString()));
    d->getFile(url);
}

/** \brief Cancels all downloads */
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
  \brief Returns true if the update file was successfully downloaded
*/
bool UpdateChecker::fileRetrieved()
{
    return d->m_FileRetrieved;
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
void UpdateChecker::showUpdateInformation()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle( tkTr(Trans::Constants::UPDATE_AVAILABLE) + " - " + qApp->applicationName() );
    dialog->resize( 500, 400 );
    QGridLayout *layout = new QGridLayout(dialog);
    QLabel *intro = new QLabel( "<b>" + tr("An update is available. Please check the web site: <a href=\"%1\">%1</a>")
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
