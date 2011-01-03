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
#ifndef DRUGSBASE_H
#define DRUGSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/interactionsbase.h>

#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>

/**
 * \file drugsbase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 23 Sept 2010
*/

/** \todo Some parts should not be Internals */

namespace DrugsDB {
class DatabaseInfos;

namespace Internal {
class DrugsData;
class DrugInteraction;
class DrugInfo;
class DrugsBasePrivate;

class DRUGSBASE_EXPORT DrugsBase : public QObject, public InteractionsBase, public Utils::Database
{
    Q_OBJECT
    DrugsBase(QObject *parent = 0);
    bool init();

    friend class DrugsModel;
    friend class DrugsBasePrivate;
    friend class Drugs;
    friend class DrugInfo;
    friend class DrugInteraction;

public:
    static DrugsBase *instance();
    ~DrugsBase();

    // Initializer / Checkers
    static bool isInitialized() { return m_initialized; }
    void logChronos(bool state);
    const DatabaseInfos *actualDatabaseInformations() const;
    DatabaseInfos *getDatabaseInformations(const QString &connectionName);
    bool isDatabaseTheDefaultOne() const;
    bool isRoutesAvailable() const;

    bool refreshAllDatabases();
    bool refreshDrugsBase();
    bool refreshDosageBase();

    // Manage drugs
    QString getDrugName(const QString &uid) const;
    DrugsData *getDrugByCIP(const QVariant & CIP_id);
    DrugsData *getDrugByUID(const QVariant &UID);
    QVariant getUIDFromCIP(int CIP);
    QStringList getDrugCompositionAtcCodes(const QVariant &uid);
    QStringList getDrugInns(const QVariant &uid);

    // Manage Dosages
    void checkDosageDatabaseVersion();
    static QString dosageCreateTableSqlQuery();
    QHash<QString, QString> getDosageToTransmit();
    bool markAllDosageTransmitted(const QStringList &dosageUuids);
    QList<QVariant> getAllUIDThatHaveRecordedDosages() const;
    QMultiHash<int,QString> getAllINNThatHaveRecordedDosages() const;
    int getRouteId(const QString &fromLabel);

//protected:
public:
    static const QString separator;

    // managins drugs
    bool drugsINNIsKnown(const DrugsData *drug);

Q_SIGNALS:
    void drugsBaseIsAboutToChange();
    void drugsBaseHasChanged();
    void dosageBaseIsAboutToChange();
    void dosageBaseHasChanged();


private:
    bool createDatabase(const QString & connectionName , const QString & dbName,
                        const QString & pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString & /*login*/, const QString & /*pass*/,
                        const int /*port*/,
                        CreationOption /*createOption*/
                       );
private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    // intialization state
    static DrugsBase *m_Instance;
    static bool m_initialized;
    DrugsBasePrivate *d;
    bool m_IsDefaultDB;
};

}  // End Internal
}  // End DrugsDB

#endif
