/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGSBASE_PROTOCOLSBASE_H
#define DRUGSBASE_PROTOCOLSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

namespace DrugsDB {
namespace Internal {
class ProtocolsBasePrivate;

class DRUGSBASE_EXPORT ProtocolsBase : public QObject, public Utils::Database
{
    Q_OBJECT
    explicit ProtocolsBase(QObject *parent = 0);
    bool init();

public:
    static ProtocolsBase *instance(QObject *parent = 0);
    ~ProtocolsBase();

//    bool refreshDosageBase();

//    void checkDosageDatabaseVersion();
//    static QString dosageCreateTableSqlQuery();
//    QHash<QString, QString> getDosageToTransmit();
//    bool markAllDosageTransmitted(const QStringList &dosageUuids);
//    QList<QVariant> getAllUIDThatHaveRecordedDosages() const;
//    QMultiHash<int,QString> getAllINNThatHaveRecordedDosages() const;


//private:
//    bool createDatabase(const QString & connectionName , const QString & dbName,
//                        const QString & pathOrHostName,
//                        TypeOfAccess access, AvailableDrivers driver,
//                        const QString & /*login*/, const QString & /*pass*/,
//                        const int /*port*/,
//                        CreationOption /*createOption*/
//                       );
//private Q_SLOTS:
//    void onCoreDatabaseServerChanged();
private:
    ProtocolsBasePrivate *d;
    static ProtocolsBase *m_Instance;
};


}  // End namespace Internal
}  // End namespace DrugsDB

#endif // DRUGSBASE_PROTOCOLSBASE_H
