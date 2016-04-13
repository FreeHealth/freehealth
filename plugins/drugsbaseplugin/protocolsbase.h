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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSBASE_PROTOCOLSBASE_H
#define DRUGSBASE_PROTOCOLSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <utils/database.h>

#include <QMultiHash>
#include <QString>
#include <QList>
#include <QVariant>

/**
 * \file protocolsbase.h
 * \author Eric MAEKER
 * \version 0.6.4
 * \date 26 Feb 2012
*/

namespace DrugsDB {
class DrugBaseCore;

namespace Internal {
class ProtocolsBasePrivate;
}

class DRUGSBASE_EXPORT ProtocolsBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class DrugsDB::DrugBaseCore;

protected:
    explicit ProtocolsBase(QObject *parent = 0);
    void forceReinitialization();
    bool initialize();

private Q_SLOTS:
    void onCoreFirstRunCreationRequested();

public:
    ~ProtocolsBase();
    bool isInitialized() const;

    QString dosageCreateTableSqlQuery();
    void checkDosageDatabaseVersion();

    QHash<QString, QString> getDosageToTransmit();
    bool markAllDosageTransmitted(const QStringList &dosageUuids);

    QList<QVariant> getAllUIDThatHaveRecordedDosages() const;
    QMultiHash<int,QString> getAllINNThatHaveRecordedDosages() const;

private:
    bool createDatabase(const QString & connectionName , const QString & dbName,
                        const QString & pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString & /*login*/, const QString & /*pass*/,
                        const int /*port*/,
                        CreationOption /*createOption*/
                       );
    bool onCoreDatabaseServerChanged();

Q_SIGNALS:
    void protocolsBaseIsAboutToChange();
    void protocolsBaseHasChanged();

private:
    Internal::ProtocolsBasePrivate *d;
};

}  // End namespace DrugsDB

#endif // DRUGSBASE_PROTOCOLSBASE_H
