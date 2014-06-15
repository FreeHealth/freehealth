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
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UPDATECHECKERPRIVATE_H
#define UPDATECHECKERPRIVATE_H

#include <QObject>
#include <QProgressBar>
#include <QPointer>
#include <QUrl>

namespace Utils {
class HttpDownloader;
namespace Internal {

class UpdateCheckerPrivate : public QObject
{
     Q_OBJECT
public:
    UpdateCheckerPrivate(QObject *parent = 0);
    ~UpdateCheckerPrivate();

    void cancelDownload();
    bool getFile(const QUrl &url);
    void updateFound(const QString &);

private Q_SLOTS:
    void onDownloadFinished();

public:
    HttpDownloader *_downloader;
    QUrl m_Url;
    QString m_UpdateText;
    QString m_LastVersion;
    bool m_FileRetreived;
    QPointer<QProgressBar> m_ProgressBar;
};

}  // End Internal
}  // End Utils

#endif  // UPDATECHECKERPRIVATE_H
