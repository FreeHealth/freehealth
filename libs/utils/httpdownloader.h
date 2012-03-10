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
#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <utils/global_exporter.h>
#include <QObject>

#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QFile;
class QProgressDialog;
class QNetworkReply;
class QMainWindow;
class QProgressBar;
QT_END_NAMESPACE

/**
 * \file httpdownloader.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 20 Jan 2012
*/

namespace Utils {

class UTILS_EXPORT HttpDownloader : public QObject
{
    Q_OBJECT
public:
    explicit HttpDownloader(QObject *parent = 0);

    // OBSOLETE
    void setMainWindow(QMainWindow *win);
    // REPLACED BY
    void setProgressBar(QProgressBar *bar);

    void setUrl(const QUrl &url);
    void setOutputPath(const QString &absolutePath);
    void setLabelText(const QString &text) {m_LabelText = text;}

    void startDownload();

Q_SIGNALS:
    void downloadFinished();
    void downloadProgressRange(qint64,qint64);
    void downloadProgressRead(qint64);

private Q_SLOTS:
    void startRequest(const QUrl &url);
    void downloadFile();
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
//    void onDownloadProgressRange(qint64,qint64);
    void updateProgressBar(qint64 bytesRead, qint64 totalBytes);
//    void slotAuthenticationRequired(QNetworkReply*,QAuthenticator *);
//#ifndef QT_NO_OPENSSL
//    void sslErrors(QNetworkReply*,const QList<QSslError> &errors);
//#endif

private:
    QString m_Path, m_LabelText;
    QUrl m_Url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    QProgressDialog *progressDialog;
    QProgressBar *progressBar;
    int httpGetId;
    bool httpRequestAborted;
};

}

#endif // HTTPDOWNLOADER_H
