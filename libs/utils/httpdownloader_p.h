/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_HTTPDOWNLOADER_PRIVATE_H
#define UTILS_HTTPDOWNLOADER_PRIVATE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QHash>
#include <QNetworkReply>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QFile;
class QProgressDialog;
class QMainWindow;
class QProgressBar;
QT_END_NAMESPACE

/**
 * \file httpdownloader_p.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 26 Nov 2012
*/

namespace Utils {
class HttpDownloader;
namespace Internal {

//FIXME: private part should be no QObject, but derive from QObjectPrivate
class HttpDownloaderPrivate : public QObject
{
    Q_OBJECT
public:
    explicit HttpDownloaderPrivate(HttpDownloader *parent);
    ~HttpDownloaderPrivate();

    bool startDownload();

public Q_SLOTS:
    bool startRequest(const QUrl &url);
    bool downloadFile();
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDownloadProgress(qint64 bytesRead, qint64 totalBytes);

    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);

//    void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
//#ifndef QT_NO_OPENSSL
//    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
//#endif

public:
    QString m_Path, m_OutputFileName, m_LabelText;
    QUrl m_Url;
    QNetworkAccessManager qnam;
    QPointer<QNetworkReply> reply;
    QFile *file;
    QProgressDialog *progressDialog;
    QProgressBar *progressBar;
    int httpGetId;
    bool httpRequestAborted;
    QHash<QString, int> m_AuthTimes;
    QString lastError;
    QNetworkReply::NetworkError networkError;
    bool _useBuffer;
    QByteArray _stringBuffer;

private:
    HttpDownloader *q;
};

}  // namespace Internal
}  // namespace Utils

#endif // UTILS_HTTPDOWNLOADER_PRIVATE_H
