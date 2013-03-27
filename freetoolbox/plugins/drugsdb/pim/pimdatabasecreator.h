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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PIMDATABASECREATOR_H
#define PIMDATABASECREATOR_H

#include <coreplugin/ifullreleasestep.h>
#include <coreplugin/itoolpage.h>

#include <QStringList>
#include <QVector>
#include <QWidget>
#include <QDomDocument>
#include <QMultiHash>
#include <QString>

/**
 * \file pimdatabasecreator.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 07 Nov 2012
*/

namespace Utils {
class PubMedDownloader;
}

namespace DrugsDB {
namespace Internal {
class DrugBaseEssentials;

class PimDatabaseCreator : public QObject
{
    Q_OBJECT
public:
    PimDatabaseCreator(QObject *parent = 0);
    bool initialize();

    bool populateDatabaseWithPims(DrugsDB::Internal::DrugBaseEssentials *database);

private:
    bool savePim(DrugsDB::Internal::DrugBaseEssentials *database, const QDomElement &element, const int sourceId, const int typeId, const QMultiHash<QString, QString> &molNameToAtcCode);

//public Q_SLOTS:
//    void downloadNextSource();

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
    int m_ActiveDownloadId;
    QVector<int> m_SourceToDownload;
    Utils::PubMedDownloader *m_Downloader;
};

}  //  namespace Internal
}  //  namespace DrugsDB

#endif // PIMDATABASECREATOR_H
