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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Utils::HttpMultiDownloader
 */

#include "httpmultidownloader.h"
#include "httpdownloader.h"

#include <utils/log.h>
#include <translationutils/constants.h>

#include <QUrl>
#include <QDebug>

// TODO: manage file collition: AskUser, Overwriter, DontDownloadIfExists, DownloadIfNever

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Utils {
namespace Internal {
struct DownloadedUrl {
    QUrl url;
    QNetworkReply::NetworkError networkError;
    QString errorMessage;
    QString outputFile;
};

class HttpMultiDownloaderPrivate
{
public:
    HttpMultiDownloaderPrivate(HttpMultiDownloader *parent) :
        _downloader(0),
        _downloadingIndex(-1),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~HttpMultiDownloaderPrivate()
    {
    }

    const DownloadedUrl &downloadedUrl(const QUrl &url)
    {
        foreach(const DownloadedUrl &dld, _DownloadedUrl) {
            if (dld.url == url)
                return dld;
        }
        return _emptyDownloaded;
    }
    
public:
    QList<QUrl> _urls;
    QString _outputPath;
    Utils::HttpDownloader *_downloader;
    QList<DownloadedUrl> _DownloadedUrl;
    DownloadedUrl _emptyDownloaded;
    int _downloadingIndex;
    
private:
    HttpMultiDownloader *q;
};
} // namespace Internal
} // end namespace UserPlugin


/*! Constructor of the UserPlugin::HttpMultiDownloader class */
HttpMultiDownloader::HttpMultiDownloader(QObject *parent) :
    QObject(parent),
    d(new HttpMultiDownloaderPrivate(this))
{
    d->_downloader = new HttpDownloader(this);
    connect(d->_downloader, SIGNAL(downloadFinished()), this, SLOT(onCurrentDownloadFinished()));
}

/*! Destructor of the UserPlugin::HttpMultiDownloader class */
HttpMultiDownloader::~HttpMultiDownloader()
{
    if (d)
        delete d;
    d = 0;
}

/** Define the output path to store downloaded files */
void HttpMultiDownloader::setOutputPath(const QString &absolutePath)
{
    d->_outputPath = absolutePath;
    d->_downloader->setOutputPath(absolutePath);
}

/** Return the absolute output path for the download */
QString HttpMultiDownloader::outputPath() const
{
    return d->_outputPath;
}

/** Define the list of urls to download in one pass */
void HttpMultiDownloader::setUrls(const QList<QUrl> &urls)
{
    d->_urls = urls;
}

/** Define the list of urls to download in one pass */
void HttpMultiDownloader::setUrls(const QStringList &urls)
{
    d->_urls.clear();
    foreach(const QString &url, urls) {
        d->_urls << QUrl(url);
    }
}

/** Start downloading files one by one */
bool HttpMultiDownloader::startDownload()
{
    if (d->_urls.isEmpty()) {
        LOG_ERROR("Nothing to download");
        return false;
    }
    d->_downloadingIndex = 0;
    d->_downloader->setUrl(d->_urls.at(d->_downloadingIndex));
    if (!d->_downloader->startDownload()) {
        LOG_ERROR("Download not started");
        return false;
    }
    return true;
}

bool HttpMultiDownloader::onCurrentDownloadFinished()
{
    // Store everything in a DownloadedUrl
    DownloadedUrl dld;
    dld.url = d->_downloader->url();
    dld.errorMessage = d->_downloader->lastErrorString();
    dld.networkError = d->_downloader->networkError();
    dld.outputFile = d->_downloader->outputAbsoluteFileName();
    d->_DownloadedUrl.append(dld);

    // Emit downloadFinished(url)
    Q_EMIT downloadFinished(dld.url);

    // Emit downloadProgressPermille()
    // All downloads done -> Emit allDownloadFinished()
    if (d->_downloadingIndex == (d->_urls.count()-1)) {
        Q_EMIT allDownloadFinished();
        return true;
    }

    // Start next url
    ++d->_downloadingIndex;
    d->_downloader->setUrl(d->_urls.at(d->_downloadingIndex));
    if (!d->_downloader->startDownload()) {
        LOG_ERROR("Download not started");
        return false;
    }
    return true;
}

QString HttpMultiDownloader::lastErrorString(const QUrl &url) const
{
    const DownloadedUrl &dld = d->downloadedUrl(url);
    return dld.errorMessage;
}

QNetworkReply::NetworkError HttpMultiDownloader::networkError(const QUrl &url) const
{
    const DownloadedUrl &dld = d->downloadedUrl(url);
    return dld.networkError;
}

QString HttpMultiDownloader::outputAbsoluteFileName(const QUrl &url) const
{
    const DownloadedUrl &dld = d->downloadedUrl(url);
    return dld.outputFile;
}
