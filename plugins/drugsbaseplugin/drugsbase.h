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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSBASE_H
#define DRUGSBASE_H

#include <drugsbaseplugin/drugsbase_exporter.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugbaseessentials.h>

#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QMultiHash>
#include <QVector>

/**
 * \file drugsbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 04 Feb 2012
*/

namespace MedicalUtils {
class EbmData;
}

namespace DrugsDB {
class DrugBaseCore;
class DatabaseInfos;
class IDrug;

namespace Internal {
class DrugInfo;
class DrugsBasePrivate;
}

class DRUGSBASE_EXPORT DrugsBase : public QObject, public Internal::DrugBaseEssentials
{
    Q_OBJECT
    friend class DrugBaseCore;

protected:
    DrugsBase(QObject *parent = 0);
    bool initialize();
    void onCoreDatabaseServerChanged();
    bool datapackChanged();

public:
    ~DrugsBase();

    // Initializer / Checkers
    bool isInitialized();
    void logChronos(bool state);
    const DatabaseInfos *actualDatabaseInformation() const;
    bool isDatabaseTheDefaultOne() const;
    bool isRoutesAvailable() const;

    QVector<DatabaseInfos *> getAllDrugSourceInformation();
    DatabaseInfos *getDrugSourceInformation(const QString &drugSourceUid = QString::null);

    // OBSOLETE
    bool refreshAllDatabases();
    // END OBSOLETE
    bool refreshDrugsBase();
    bool changeCurrentDrugSourceUid(const QVariant &uid);
//    bool refreshDosageBase();

    // Manage drugs
    QString getDrugNameByDrugId(const QVariant &drugId);
    QString getDrugName(const QString &uid1, const QString &uid2 = QString::null, const QString &uid3 = QString::null) const;
    IDrug *getDrugByUID(const QVariant &uid1, const QVariant &uid2 = QVariant(), const QVariant &uid3 = QVariant(), const QVariant &oldUid = QVariant(), const QString &sourceUid = QString::null);
    IDrug *getDrugByOldUid(const QVariant &oldUid, const QString &sourceUid = QString::null);
    QVariantList getDrugUids(const QVariant &drugId);
    IDrug *getDrugByDrugId(const QVariant &drugId);

    // Manage labels
    QString getLabel(const int masterLid, const QString &lang = QString::null);

    // Used by the GlobalDrugsModel
    QStringList getDrugCompositionAtcCodes(const QVariant &drugId);
    QStringList getDrugInns(const QVariant &drugId);
    QStringList getRouteLabels(const QVariant &drugId, const QString &lang = QString::null);
    QStringList getFormLabels(const QVariant &drugId, const QString &lang = QString::null);
    QHash<QString, QString> getDrugFullComposition(const QVariant &drugId, const QString &lang = QString::null);
    QStringList getDrugMolecularComposition(const QVariant &drugId, const QString &lang = QString::null);

    // Manage drug classification
    int getAtcCodeForMoleculeId(const int molId) const;
    QString getAtcLabel(const int atcId) const;
    QString getAtcLabel(const QString &code) const;
    QString getAtcCode(const int atcId) const;
    QString getInnDenominationFromSubstanceCode(const int molId) const;

    QVector<int> getLinkedMoleculeCodes(QVector<int> &atc_ids) const;
    QVector<int> getLinkedMoleculeCodes(const int atc_id) const;
    QVector<int> getLinkedMoleculeCodes(const QString &atcLabel) const;
    QVector<int> getLinkedAtcIds(const QVector<int> &mids) const;
    QVector<int> getLinkedAtcIds(const int mid) const;

    QVector<int> getAllMoleculeCodeWithAtcStartingWith(const QString &code) const;

    // Interacting classes
    bool isInteractingClass(int atcId);
    QList<int> interactingClassContent(int classId);
    int interactingClassSingleAtcCount(int classId);

    // Bibliography
    QVector<MedicalUtils::EbmData *> getAllBibliographyFromTree(const QList<int> &allInnAndInteractingClassesIds);

    // Summary of Product Characteristics
    QString getDrugSpc(const QVariant &drugId);

//protected:
public:
    static const QString separator;

    // managins drugs
//    bool drugsINNIsKnown(const IDrug *drug);

Q_SIGNALS:
    void drugsBaseIsAboutToChange();
    void drugsBaseHasChanged();

private:
//    bool createDatabase(const QString & connectionName , const QString & dbName,
//                        const QString & pathOrHostName,
//                        TypeOfAccess access, AvailableDrivers driver,
//                        const QString & /*login*/, const QString & /*pass*/,
//                        const int /*port*/,
//                        CreationOption /*createOption*/
//                       );

private:
    Internal::DrugsBasePrivate *d;
};

}  // End DrugsDB

#endif
