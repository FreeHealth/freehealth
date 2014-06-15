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
#ifndef ICDDOWNLOADER_H
#define ICDDOWNLOADER_H

#include <icdplugin/icd_exporter.h>

#include <QObject>
QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

/**
 * \file icddownloader.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 13 Oct 2010
*/


namespace Utils {
class HttpDownloader;
}

namespace ICD {

class ICD_EXPORT IcdDownloader : public QObject
{
    Q_OBJECT

public:
    IcdDownloader(QObject *parent = 0);
    ~IcdDownloader();

public Q_SLOTS:
    bool createDatabase();

protected Q_SLOTS:
    bool downloadRawSources();
    bool downloadFinished();
    bool populateDatabaseWithRawSources();

Q_SIGNALS:
    void processEnded();

private:
    Utils::HttpDownloader *m_Downloader;
    QProgressDialog *m_Progress;
};

}  // End namespace ICD

#endif // ICDDOWNLOADER_H
