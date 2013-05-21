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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_HTTPMULTIDOWNLOADER_H
#define UTILS_HTTPMULTIDOWNLOADER_H

#include <utils/global_exporter.h>
#include <QObject>
#include <QNetworkReply>

/**
 * \file httpmultidownloader.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 05 May 2013
*/

namespace Utils {
namespace Internal {
class HttpMultiDownloaderPrivate;
} // namespace Internal

class UTILS_EXPORT HttpMultiDownloader : public QObject
{
    Q_OBJECT

public:
    enum OnError {
        OnErrorStop = 0,
        OnErrorPursue
    };

    explicit HttpMultiDownloader(QObject *parent = 0);
    ~HttpMultiDownloader();
    
    void setOutputPath(const QString &absolutePath);
    QString outputPath() const;

    void setUseUidAsFileNames(bool useUidInsteadOfUrlFileName);
    bool useUidAsFileNames() const;

    void setDownloadErrorManagement(OnError onError);
    OnError downloadErrorManagement() const;

    bool clearXmlUrlFileLinks();
    bool saveXmlUrlFileLinks();
    bool readXmlUrlFileLinks();

    // Setter for URL
    void setUrls(const QList<QUrl> &urls);
    void setUrls(const QStringList &urls);
    void addUrls(const QList<QUrl> &urls);
    // TODO: void addUrl(const QUrl &url, const QString &relativeOutputFileName); + QTest
    const QList<QUrl> &urls() const;

    // Getters for URL
    QList<QUrl> downloadedUrls() const;
    QString lastErrorString(const QUrl &url) const;
    QNetworkReply::NetworkError networkError(const QUrl &url) const;
    QString outputAbsoluteFileName(const QUrl &url) const;

public Q_SLOTS:
    bool startDownload();

private Q_SLOTS:
    bool onCurrentDownloadFinished();

Q_SIGNALS:
    void progressMessageChanged(const QString &message);
    void downloadFinished(const QUrl &url);
    void downloadProgressPermille(int);
    void allDownloadFinished();

private:
    Internal::HttpMultiDownloaderPrivate *d;
};

} // namespace Utils

#endif  // UTILS_HTTPMULTIDOWNLOADER_H

