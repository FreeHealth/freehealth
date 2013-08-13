/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * \note Unit-test available (see: tests/auto/auto.pro)
 */

#include "httpmultidownloader.h"
#include "httpdownloader.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QUrl>
#include <QDir>
#include <QDomDocument>
#include <QDebug>

// TODO: manage file collition: AskUser, Overwriter, DontDownloadIfExists, DownloadIfNever

using namespace Utils;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
const char *const TAG_ROOT = "MultiDownloader";
const char *const TAG_SERVER = "Server";
const char *const TAG_URLROOT = "Url";
const char *const ATTRIB_URL = "u";
const char *const ATTRIB_FILENAME = "f";
const char *const ATTRIB_ERRORMSG = "m";
const char *const ATTRIB_ERROR = "e";
const char *const DEFAULT_XML_FILENAME = "multidownloader.xml";
}

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
        _useUidAsFileName(false),
        _onError(HttpMultiDownloader::OnErrorPursue),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~HttpMultiDownloaderPrivate()
    {
    }

    const DownloadedUrl &downloadedUrl(const QUrl &url)
    {
        foreach(const DownloadedUrl &dld, _downloadedUrl) {
            if (dld.url == url)
                return dld;
        }
        return _emptyDownloaded;
    }

    QString uuidFileName(const QUrl &url)
    {
        return QString("%1.%2")
                .arg(Utils::createUid())
                .arg(QFileInfo(url.toString()).completeSuffix());
    }
    
public:
    QList<QUrl> _urls;
    QString _outputPath;
    Utils::HttpDownloader *_downloader;
    QList<DownloadedUrl> _downloadedUrl;
    DownloadedUrl _emptyDownloaded;
    int _downloadingIndex;
    bool _useUidAsFileName;
    HttpMultiDownloader::OnError _onError;
    
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

/**
 * To avoid file collition, you can force the downloader to use QUuid as file names instead of the
 * QUrl filename. You must define this property before any download.\n
 * To keep a trace of this link (local file <-> url), you can save the XML data using saveXmlUrlFileLinks()
 * then read this file using readXmlUrlFileLinks(). \n
 * By default, this property is set to false.
 */
void HttpMultiDownloader::setUseUidAsFileNames(bool useUidInsteadOfUrlFileName)
{
    d->_useUidAsFileName = useUidInsteadOfUrlFileName;
}

bool HttpMultiDownloader::useUidAsFileNames() const
{
    return d->_useUidAsFileName;
}

void HttpMultiDownloader::setDownloadErrorManagement(OnError onError)
{
    d->_onError = onError;
}

/** Be default, set to HttpMultiDownloader::OnErrorPursue*/
HttpMultiDownloader::OnError HttpMultiDownloader::downloadErrorManagement() const
{
    return d->_onError;
}

/**
 * If an XML output file exists in the outputpath it will be deleted.
 * \sa saveXmlUrlFileLinks(), readXmlUrlFileLinks()
 */
bool HttpMultiDownloader::clearXmlUrlFileLinks()
{
    QFile xmlFile(QString("%1/%2").arg(outputPath()).arg(::DEFAULT_XML_FILENAME));
    if (xmlFile.exists())
        xmlFile.remove();
    return true;
}

/**
 * Save all information to easily switch from QUrl to local file name in the outputPath(). \n
 * Only available if the setUseUidAsFileNames() was set to \e true.
 */
bool HttpMultiDownloader::saveXmlUrlFileLinks()
{
    if (!d->_useUidAsFileName)
        return false;
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressMessageChanged(tr("Saving cache"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QDomDocument doc("FreeMedForms");
    QDomElement element = doc.createElement(::TAG_ROOT);
    doc.appendChild(element);
    QDir output(outputPath());
    int count = d->_downloadedUrl.count();
    int i = 0;
    // TODO: group url per server and manage the ::TAG_SERVER
    // The XML structure will then be
    // <Server u="http://www.common.com/path/of/the/following/url">
    //   <Url u="./relative/to/common.html" .../>
    // </Server>
    foreach(const DownloadedUrl &url, d->_downloadedUrl) {
        QDomElement urlElement = doc.createElement(::TAG_URLROOT);
        urlElement.setAttribute(::ATTRIB_URL, url.url.toString());
        urlElement.setAttribute(::ATTRIB_FILENAME, output.relativeFilePath(url.outputFile));
        urlElement.setAttribute(::ATTRIB_ERRORMSG, url.errorMessage);
        urlElement.setAttribute(::ATTRIB_ERROR, url.networkError);
        ++i;
        if (i % 10 == 0) {
            int permille = i/count*1000;
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            Q_EMIT downloadProgressPermille(permille);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        element.appendChild(urlElement);
    }
    bool ok = Utils::saveStringToFile(QString("<?xml version='1.0' encoding='UTF-8'?>\n" + doc.toString(2)),
                                   QString("%1/%2").arg(outputPath()).arg(::DEFAULT_XML_FILENAME),
                                   Utils::Overwrite, Utils::DontWarnUser);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressMessageChanged(tr("Cache saved"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    return ok;
}

/**
 * Read all information to easily switch from QUrl to local file name from the outputPath().
 */
bool HttpMultiDownloader::readXmlUrlFileLinks()
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressMessageChanged(tr("Processing cache"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QDomDocument doc;
    if (!doc.setContent(Utils::readTextFile(QString("%1/%2").arg(outputPath()).arg(::DEFAULT_XML_FILENAME), Utils::DontWarnUser))) {
        LOG_ERROR("Wrong xml file");
        return false;
    }
    QDomElement element = doc.firstChildElement(::TAG_ROOT);
    QDir output(outputPath());
    int count = element.childNodes().count();
    int i = 0;
    element = element.firstChildElement(::TAG_URLROOT);
    while (!element.isNull()) {
        DownloadedUrl url;
        url.url = QUrl(element.attribute(::ATTRIB_URL));
        url.outputFile = output.absoluteFilePath(element.attribute(::ATTRIB_FILENAME));
        url.errorMessage = element.attribute(::ATTRIB_ERRORMSG);
        url.networkError = QNetworkReply::NetworkError(element.attribute(::ATTRIB_ERROR).toInt());
        d->_urls << url.url;
        d->_downloadedUrl << url;
        ++i;
        if (i % 10 == 0) {
            int permille = i/count*1000;
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            Q_EMIT downloadProgressPermille(permille);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        element = element.nextSiblingElement(::TAG_URLROOT);
    }
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressMessageChanged(tr("Cache processed"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    return true;
}

/**
 * Define the list of urls to download in one pass.
 * This will overwrite any recorded url to download list but will not
 * reset the downloaded information (eg if you used readXmlUrlFileLinks()
 * which populates the downloaded information).
 */
void HttpMultiDownloader::setUrls(const QList<QUrl> &urls)
{
    d->_urls = urls;
}

/**
 * Define the list of urls to download in one pass.
 * This will overwrite any recorded url to download list but will not
 * reset the downloaded information (eg if you used readXmlUrlFileLinks()
 * which populates the downloaded information).
 */
void HttpMultiDownloader::setUrls(const QStringList &urls)
{
    d->_urls.clear();
    foreach(const QString &url, urls) {
        d->_urls << QUrl(url);
    }
}

/** Add a list of urls to download in one pass */
void HttpMultiDownloader::addUrls(const QList<QUrl> &urls)
{
    d->_urls << urls;
}

/** Returns the list of urls to download */
const QList<QUrl> &HttpMultiDownloader::urls() const
{
    return d->_urls;
}

/** Start downloading files one by one */
bool HttpMultiDownloader::startDownload()
{
    if (d->_urls.isEmpty()) {
        LOG_ERROR("Nothing to download");
        Q_EMIT allDownloadFinished();
        return false;
    }
    d->_downloadingIndex = 0;
    d->_downloader->setUrl(d->_urls.at(d->_downloadingIndex));
    if (d->_useUidAsFileName) {
        d->_downloader->setOutputFileName(d->uuidFileName(d->_urls.at(d->_downloadingIndex)));
    }
    if (!d->_downloader->startDownload()) {
        LOG_ERROR("Download not started");
        return false;
    }
    return true;
}

/**
 * Slot connected to URL download finished. Save the current download and start a new one.
 * Emits allDownloadFinished() if nothing's left to download.
 */
bool HttpMultiDownloader::onCurrentDownloadFinished()
{
    // Store everything in a DownloadedUrl
    DownloadedUrl dld;
    dld.url = d->_downloader->url();
    dld.errorMessage = d->_downloader->lastErrorString();
    dld.networkError = d->_downloader->networkError();
    dld.outputFile = d->_downloader->outputAbsoluteFileName();
    d->_downloadedUrl.append(dld);

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
    if (d->_useUidAsFileName) {
        d->_downloader->setOutputFileName(d->uuidFileName(d->_urls.at(d->_downloadingIndex)));
    }
    if (!d->_downloader->startDownload()) {
        LOG_ERROR("Download not started");
        return false;
    }
    return true;
}

/** Return the full list of downloaded urls (with or without success) */
QList<QUrl> HttpMultiDownloader::downloadedUrls() const
{
    QList<QUrl> urls;
    foreach(const DownloadedUrl &dld, d->_downloadedUrl)
        urls << dld.url;
    return urls;
}

/** Returns the error message linked with the \e url */
QString HttpMultiDownloader::lastErrorString(const QUrl &url) const
{
    const DownloadedUrl &dld = d->downloadedUrl(url);
    return dld.errorMessage;
}

/** Returns the error linked with the \e url */
QNetworkReply::NetworkError HttpMultiDownloader::networkError(const QUrl &url) const
{
    const DownloadedUrl &dld = d->downloadedUrl(url);
    return dld.networkError;
}

/** Returns the output filename linked with the \e url */
QString HttpMultiDownloader::outputAbsoluteFileName(const QUrl &url) const
{
    const DownloadedUrl &dld = d->downloadedUrl(url);
    return dld.outputFile;
}
