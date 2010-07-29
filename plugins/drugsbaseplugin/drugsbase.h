/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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

// include Qt headers
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>

/**
 * \file drugsbase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 01 Mar 2010
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

    bool refreshAllDatabases();
    bool refreshDrugsBase();
    bool refreshDosageBase();

    // Manage drugs
    DrugsData *getDrugByCIP(const QVariant & CIP_id);
    DrugsData *getDrugByUID(const QVariant &UID);
    int        getUIDFromCIP(int CIP);
    QStringList getDrugCompositionAtcCodes(const int uid);
    QStringList getDrugInns(const QVariant &uid);

    // Manage Dosages
    void checkDosageDatabaseVersion();
    static QString dosageCreateTableSqlQuery();
    QHash<QString, QString> getDosageToTransmit();
    bool markAllDosageTransmitted(const QStringList &dosageUuids);
    QList<int> getAllUIDThatHaveRecordedDosages() const;
    QMultiHash<int,QString> getAllINNThatHaveRecordedDosages() const;

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
