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
#ifndef UTILS_HTTPDOWNLOADER_H
#define UTILS_HTTPDOWNLOADER_H

#include <utils/global_exporter.h>
#include <QObject>
QT_BEGIN_NAMESPACE
class QProgressBar;
class QUrl;
class QMainWindow;
QT_END_NAMESPACE

/**
 * \file httpdownloader.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 26 Nov 2012
*/

namespace Utils {
namespace Internal {
class HttpDownloaderPrivate;
}  // namespace Internal

class UTILS_EXPORT HttpDownloader : public QObject
{
    Q_OBJECT
    friend class Utils::Internal::HttpDownloaderPrivate;

public:
    explicit HttpDownloader(QObject *parent = 0);
    ~HttpDownloader();

    void setMainWindow(QMainWindow *win);
    void setProgressBar(QProgressBar *bar);
    void setUrl(const QUrl &url);
    void setOutputPath(const QString &absolutePath);
    void setLabelText(const QString &text);

public Q_SLOTS:
    bool startDownload();
    bool cancelDownload();

Q_SIGNALS:
    void downloadFinished();
    void downloadProgressRange(qint64,qint64);
    void downloadProgressRead(qint64);
    void downloadProgressPercents(int);

private:
    Internal::HttpDownloaderPrivate *d;
};

}  // namespace Utils

#endif // UTILS_HTTPDOWNLOADER_H
