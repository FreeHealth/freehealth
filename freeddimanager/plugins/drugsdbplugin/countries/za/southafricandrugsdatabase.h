/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DRUGSDB_INTERNAL_SOUTHAFRICANDRUGSDATABASE_H
#define DDIMANAGER_DRUGSDB_INTERNAL_SOUTHAFRICANDRUGSDATABASE_H

#include <drugsdbplugin/idrugdatabase.h>
#include <QMap>
#include <QString>

/**
 * \file southafricandrugsdatabase.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 10 Feb 2014
*/

namespace DrugsDb {
namespace Internal {

class ZaDrugDatabase : public DrugsDb::Internal::IDrugDatabase
{
    Q_OBJECT

public:
    ZaDrugDatabase(QObject *parent = 0);
    ~ZaDrugDatabase();

    QString id() const {return "ZaDrugDatabase";}
    void setLicenseType(LicenseType type);
    bool createTemporaryStorage();

    bool startDownload();
    void getAllDrugLinksFromIndexesFiles();

    bool process();
    QString processMessage() const;

    bool unzipFiles();
    bool prepareData();
    bool populateDatabase();
    // bool linkMolecules();
    bool downloadSpcContents();

private Q_SLOTS:
    bool onIndexFilesDownloadFinished();
    bool onSpcDownloadFinished();

private:
    QMap<QString, QString> m_Drug_Link;
};

}  //  namespace Internal
}  //  End namespace DrugsDb

#endif // DDIMANAGER_DRUGSDB_INTERNAL_SOUTHAFRICANDRUGSDATABASE_H
