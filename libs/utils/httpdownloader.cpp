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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Utils::HttpDownloader
 * Simple Http downlader. The object must not be deleted during the download process.
 * You can stop the current downloading using the cancelDownload().\n
 * This downloader manages server and proxy authentication. User will be asked for their
 * login and password if required.
 *
 * Proxy management: \n
 * If you are using a proxy, you must register it with the QNetworkProxy::setApplicationProxy()
 * before starting any downloads.
 * If the user and password are not defined, a dialog will ask the user for its log & pass.
 *
 * Using progress bar: \n
 * You can connect the downloadProgressPermille() signal which is emitted each time the
 * permille of the download changes. Remember to set your progress bar range to (0, 1000).
*/

#include "httpdownloader.h"
#include "httpdownloader_p.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/basiclogindialog.h>

#include <QProgressDialog>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QDir>
#include <QMainWindow>
#include <QProgressBar>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QNetworkProxy>

using namespace Utils;
using namespace Internal;

namespace {
const int MAX_AUTHENTIFICATION_TRIES = 3;
const char * const  ICONEYES = "eyes.png";
}

/** CTor of the object */
HttpDownloader::HttpDownloader(QObject *parent) :
    QObject(parent),
    d(new HttpDownloaderPrivate(this))
{
    setObjectName("HttpDownloader");
}

/** DTor of the object */
HttpDownloader::~HttpDownloader()
{
    if (d)
        delete d;
    d = 0;
}

/*! \deprecated should not have any direct contact to GUI!
 */
void HttpDownloader::setMainWindow(QMainWindow *win)
{
    if (d->progressDialog) {
        delete d->progressDialog;
        d->progressDialog = new QProgressDialog(win);
        d->progressDialog->setWindowModality(Qt::WindowModal);
        connect(d->progressDialog, SIGNAL(canceled()), d, SLOT(cancelDownload()));
    }
}

/**
 * Sets the URL to download. The downloaded file will be saved into the output path with the
 * same filename as the URL.
 * \sa setOutputPath()
*/
void HttpDownloader::setUrl(const QUrl &url)
{
    d->m_Url = url;
//    d->m_Url.setAuthority();
}

/**
 * Instead of downloading the content into a file, keep the content in a buffer.
 * To retrieve the content, use getBufferContent(). \n
 * \note By default, set to false.
 */
void HttpDownloader::setStoreInBuffer(bool store)
{
   d->_useBuffer = store;
}

QByteArray HttpDownloader::getBufferContent() const
{
    return d->_stringBuffer;
}

/** Sets the download output path (absolute path only) */
void HttpDownloader::setOutputPath(const QString &absolutePath)
{
    if (QDir(absolutePath).exists())
        d->m_Path = absolutePath;
    else
        d->m_Path.clear();
}

/*!
  * \brief Sets the output file name.
  *
  * By default, the output filename is the filename of the URL. This method sets a custom out put filename.
  * If you want to clear the output file name just pass an empty QString to this method.
  * \sa setUrl(), setOutputPath()
  */
void HttpDownloader::setOutputFileName(const QString &fileName)
{
    d->m_OutputFileName = fileName;
}

/*!
  * \brief Returns the output file name.
  *
  * By default, the output filename is the filename of the URL, you can customize it with setOutputFileName().
  * \sa setUrl(), setOutputFileName()
  */
QString HttpDownloader::outputFileName() const
{
    if (d->m_OutputFileName.isEmpty()) {
        QFileInfo fileInfo(d->m_Url.path());
        QString fileName =fileInfo.fileName();
        if (fileName.isEmpty())
            fileName = "index.html";
        return fileName;
    }
    return d->m_OutputFileName;
}

/*!
  * \brief Returns the output absolute path and file name.
  *
  * By default, the output filename is the filename of the URL, you can customize it with setOutputFileName().
  * \sa setUrl(), setOutputFileName(), setOutputPath()
  */
QString HttpDownloader::outputAbsoluteFileName() const
{
    return d->m_Path + QDir::separator() + outputFileName();
}

/** Defines the label of the progress */
void HttpDownloader::setLabelText(const QString &text)
{
    d->m_LabelText = text;
}

QString HttpDownloader::lastErrorString() const
{
    return d->lastError;
}

QNetworkReply::NetworkError HttpDownloader::networkError()
{
    return d->networkError;
}

/**
 * Starts the asynchronous downloading. When the download is finished
 * the downloadFinished() signal is emitted. You can follow the download progress
 * with the downloadProgress() or downloadProgressPercentsChanged() signals.
 */
bool HttpDownloader::startDownload()
{
    return d->startDownload();
}

/**
 * Stops the downloading. Emits the downloadFinished() signal.
 */
bool HttpDownloader::cancelDownload()
{
    d->cancelDownload();
    Q_EMIT downloadFinished();
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////     PRIVATE PART    ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
HttpDownloaderPrivate::HttpDownloaderPrivate(HttpDownloader *parent) :
    reply(0),
    file(0),
    progressDialog(0),
    progressBar(0),
    httpGetId(-1),
    httpRequestAborted(false),
    networkError(QNetworkReply::NoError),
    _useBuffer(false),
    q(parent)
{
    setObjectName("HttpDownloaderPrivate");
    // Use a proxy (take the first registered proxy)?
//    QList<QNetworkProxy> proxies = QNetworkProxyFactory::proxyForQuery(QNetworkProxyQuery(QUrl("http://www.google.fr")));
//    if (proxies.count()>=1) {
//        qnam.setProxy(proxies.at(0));
//        LOG("Using proxy: " + qnam.proxy().hostName());
//    }

    if (!QNetworkProxy::applicationProxy().hostName().isEmpty()) {
        qnam.setProxy(QNetworkProxy::applicationProxy());
        LOG("Using proxy: " + qnam.proxy().hostName());
    } else {
        qnam.setProxy(QNetworkProxy::NoProxy);
        LOG("Clearing proxy");
    }

    // Connect authentication request
    connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(&qnam, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
}

HttpDownloaderPrivate::~HttpDownloaderPrivate()
{}

bool HttpDownloaderPrivate::startDownload()
{
    if (progressBar) {
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        progressBar->setToolTip(tr("Initialization of the download"));
    }
    if (!m_Url.isValid())
        return false;
    if (m_Url.isEmpty())
        return false;
    return downloadFile();
}

/** Prepare the Http download */
bool HttpDownloaderPrivate::downloadFile()
{
    if (!_useBuffer) {
        QString fileName = q->outputAbsoluteFileName();

        if (QFile::exists(fileName)) {
            // FIXME: No direct GUI access from a non-GUI class!!!
            if (!Utils::yesNoMessageBox(tr("There already exists a file called %1 in "
                                           "the current directory. Overwrite?").arg(fileName), ""))
                return false;
            QFile::remove(fileName);
        }

        file = new QFile(fileName);
        if (!file->open(QIODevice::WriteOnly)) {
            // FIXME: No direct GUI access from a non-GUI class!!!
            Utils::warningMessageBox(tr("Unable to save the file %1: %2.")
                                     .arg(fileName).arg(file->errorString()), "");
            delete file;
            file = 0;
            return false;
        }
    } else {
        _stringBuffer.clear();
    }

    if (progressBar) {
        progressBar->setToolTip(m_LabelText);
    }

    // schedule the request
    httpRequestAborted = false;
    return startRequest(m_Url);
}

/** Begin the Http download */
bool HttpDownloaderPrivate::startRequest(const QUrl &url)
{
    if (!url.isValid())
        return false;
    if (m_LabelText.isEmpty()) {
        LOG(tr("Start downloading: %1 to %2").arg(m_Url.toString()).arg(m_Path));
    } else {
        LOG(m_LabelText);
    }
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDownloadProgress(qint64,qint64)));

    return true;
}

/** Cancel the download */
void HttpDownloaderPrivate::cancelDownload()
{
    httpRequestAborted = true;
    if (reply) {
        reply->abort();
        reply->deleteLater();
    }
    networkError = QNetworkReply::OperationCanceledError;
    lastError = tr("Download canceled.");
    _stringBuffer.clear();
}

/** Slot called when the downloading is finished (with or without error) */
void HttpDownloaderPrivate::httpFinished()
{
    qWarning() << "httpFinished" << reply->error() << reply->errorString();

    networkError = reply->error();

    if (httpRequestAborted || networkError != QNetworkReply::NoError) {

        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        if (progressDialog)
            progressDialog->hide();
        return;
    }

    if (progressBar) {
        if (networkError != QNetworkReply::NoError) {
            progressBar->setValue(0);
            lastError = tr("Download finished with an error: %1.").arg(reply->errorString());
            progressBar->setToolTip(lastError);
        } else  {
            progressBar->setValue(100);
            progressBar->setToolTip(tr("Download finished."));
        }
    }

    if (!_useBuffer) {
        file->flush();
        file->close();
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (networkError) {
        if (!_useBuffer)
            file->remove();
        Utils::informativeMessageBox(tr("Download failed: %1.")
                                     .arg(reply->errorString()), "", "", tr("HTTP"));
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = m_Url.resolved(redirectionTarget.toUrl());
        if (Utils::yesNoMessageBox(tr("Redirect to %1?").arg(newUrl.toString()),
                                   tr("Redirect to %1?").arg(newUrl.toString()),
                                   "", tr("HTTP"))) {
            m_Url = newUrl;
            reply->deleteLater();
            if (!_useBuffer) {
                file->open(QIODevice::WriteOnly);
                file->resize(0);
            }
            startRequest(m_Url);
            return;
        }
    } else {
        //        QString fileName = QFileInfo(QUrl(urlLineEdit->text()).path()).fileName();
        LOG(tr("Downloaded %1 to current directory.").arg("file"));//.arg(fileName));
    }

    reply->deleteLater();
    reply = 0;
    if (file)
        delete file;
    file = 0;

    Q_EMIT q->downloadFinished();
}

/** Read the content of the reply and write to output file */
void HttpDownloaderPrivate::httpReadyRead()
{
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (!_useBuffer && file)
        file->write(reply->readAll());
    else
        _stringBuffer += reply->readAll();
}

/**
  * Emits signals that can be connected to a QProgressBar which is then updated according
  * to the current download status (range and value). \n
  * Also computes the downloading in tenths of a percent and emits the downloadProgressPermille() signal.
  */
void HttpDownloaderPrivate::updateDownloadProgress(qint64 bytesReceived, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;

    Q_EMIT q->downloadProgress(bytesReceived, totalBytes);

    int permille = 0;
    if (totalBytes>0)
        permille = bytesReceived*1000/totalBytes;

    Q_EMIT q->downloadProgressPermille(permille);
}

/** Slot connected to server authentication required */
void HttpDownloaderPrivate::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    LOG("Server authentication required: " +  reply->url().toString());
    const QString &host = reply->url().toString();
    m_AuthTimes.insert(host, m_AuthTimes.value(host, 0) + 1);
    if (m_AuthTimes.value(host) > MAX_AUTHENTIFICATION_TRIES) {
        LOG_ERROR("Server authentication max tries achieved. " +  host);
        return;
    }
    Utils::BasicLoginDialog dlg;
    dlg.setModal(true);
    dlg.setTitle(tr("Server authentication required"));
    // TODO: manage icon
//    dlg.setToggleViewIcon(core().icon(ICONEYES));
    if (dlg.exec()==QDialog::Accepted) {
        authenticator->setUser(dlg.login());
        authenticator->setPassword(dlg.password());
    }
    // TODO: manage ServerEngineStatus here
}

/** Slot connected to proxy authentication required */
void HttpDownloaderPrivate::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    LOG("Proxy authentication required: " +  proxy.hostName());
    const QString &host = proxy.hostName();
    m_AuthTimes.insert(host, m_AuthTimes.value(host, 0) + 1);
    if (m_AuthTimes.value(host) > MAX_AUTHENTIFICATION_TRIES) {
        LOG_ERROR("Proxy authentication max tries achieved. " +  host);
        return;
    }
    if (!proxy.user().isEmpty() && !proxy.password().isEmpty()) {
        authenticator->setUser(proxy.user());
        authenticator->setPassword(proxy.password());
    } else {
        // Ask user for identification
        Utils::BasicLoginDialog dlg;
        dlg.setModal(true);
        dlg.setTitle(tr("Proxy authentication required"));
        // TODO: manage icon
//        dlg.setToggleViewIcon(core().icon(ICONEYES));
        if (dlg.exec()==QDialog::Accepted) {
            authenticator->setUser(dlg.login());
            authenticator->setPassword(dlg.password());
        }
    }
    // TODO: manage ServerEngineStatus here
}

//#ifndef QT_NO_OPENSSL
//void HttpDownloaderPrivate::sslErrors(QNetworkReply*,const QList<QSslError> &errors)
//{
//    QString errorString;
//    foreach (const QSslError &error, errors) {
//        if (!errorString.isEmpty())
//            errorString += ", ";
//        errorString += error.errorString();
//    }

//    if (QMessageBox::warning(this, tr("HTTP"),
//                             tr("One or more SSL errors has occurred: %1").arg(errorString),
//                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
//        reply->ignoreSslErrors();
//    }
//}
//#endif
