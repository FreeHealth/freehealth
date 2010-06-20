/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "globaldrugsmodel.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>

#include <QList>
#include <QColor>
#include <QDir>
#include <QCache>
#include <QSqlError>

#include <QDebug>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

/**
  \todo no singleton otherwise search filter is applied to all views.
*/

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    GlobalDrugsModelPrivate(GlobalDrugsModel *parent) :
            q(parent), testAtc(false), testUid(false), testInn(false)
    {
        Q_ASSERT(q);
        ++numberOfInstances;
    }

    ~GlobalDrugsModelPrivate()
    {
        --numberOfInstances;
        if (numberOfInstances==0) {
            m_CachedAvailableDosageForUID.clear();
            drugAllergyCache.clear();
        }
    }

    void clearDrugAllergyCache()
    {
        drugAllergyCache.clear();
    }

    static void updateCachedAvailableDosage()
    {
        m_CachedAvailableDosageForUID.clear();
        m_CachedAvailableDosageForUID = drugsBase()->getAllUIDThatHaveRecordedDosages();
    }

    static bool UIDHasRecordedDosage(const int uid)
    {
        return m_CachedAvailableDosageForUID.contains(uid);
    }

    QString getConstructedDrugName(const int row)
    {
        if (drugsBase()->actualDatabaseInformations()) {
            QString tmp = drugsBase()->actualDatabaseInformations()->drugsNameConstructor;
            if (!tmp.isEmpty()) {
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_NAME),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_NAME)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_FORM),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_FORM)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_ROUTE),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_ROUTE)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_STRENGTH)).toString());
                return tmp;
            }
        }
        return q->QSqlTableModel::data(q->index(row, Constants::DRUGS_NAME)).toString();
    }

    bool hasAllergie(const QModelIndex &item)
    {
        int uid = q->index(item.row(), Constants::DRUGS_UID).data().toInt();
        if (drugAllergyCache.contains(uid)) {
            return drugAllergyCache.value(uid);
        }
        if (testInn) {
            // get all drugs inns
            const QStringList &inns = DrugsDB::Internal::DrugsBase::instance()->getDrugInns(q->index(item.row(), Constants::DRUGS_UID).data().toString());
            foreach(const QString &druginn, inns) {
                if (innAllergies.contains(druginn)) {
                    drugAllergyCache.insert(uid, true);
                    return true;
                }
            }
        }
        if (testAtc) {
            QString atc = q->index(item.row(), Constants::DRUGS_ATC).data().toString();
            if (atcAllergies.contains(atc)) {
                drugAllergyCache.insert(uid, true);
                return true;
            }
        }
        if (testUid) {
            if (uidAllergies.contains(QString::number(uid))) {
                drugAllergyCache.insert(uid, true);
                return true;
            }
        }
        drugAllergyCache.insert(uid, false);

        if (drugAllergyCache.size() > 10000) {
            drugAllergyCache.remove(drugAllergyCache.begin().key());
        }

        return false;
    }


public:
    QStringList atcAllergies, uidAllergies, innAllergies;
    bool testAtc, testUid, testInn;

private:
    static QHash<int, bool> drugAllergyCache;
    static QList<int> m_CachedAvailableDosageForUID;
    static int numberOfInstances;
    GlobalDrugsModel *q;
};

QList<int> GlobalDrugsModelPrivate::m_CachedAvailableDosageForUID;
QHash<int, bool> GlobalDrugsModelPrivate::drugAllergyCache;
int GlobalDrugsModelPrivate::numberOfInstances;

}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

void GlobalDrugsModel::updateAvailableDosages()
{
    Internal::GlobalDrugsModelPrivate::updateCachedAvailableDosage();
}

GlobalDrugsModel::GlobalDrugsModel(QObject *parent) :
        QSqlTableModel(parent, QSqlDatabase::database(Constants::DB_DRUGS_NAME)),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate(this);
    d->updateCachedAvailableDosage();
    setTable(drugsBase()->table(Constants::Table_DRUGS));
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert(Constants::DRUGS_MARKET, "=1");
    setFilter(drugsBase()->getWhereClause(Constants::Table_DRUGS, where));
    select();

    connect(drugsBase(), SIGNAL(dosageBaseHasChanged()), this, SLOT(updateCachedAvailableDosage()));
    connect(patient(), SIGNAL(dataChanged(int)), this, SLOT(refreshDrugsAllergies(int)));
}

GlobalDrugsModel::~GlobalDrugsModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

void GlobalDrugsModel::refreshDrugsAllergies(const int ref)
{
    if (ref==Core::IPatient::DrugsAllergiesWithoutPrecision ||
        ref==Core::IPatient::DrugsUidAllergies ||
        ref==Core::IPatient::DrugsInnAtcAllergies ||
        ref==Core::IPatient::DrugsInnAllergies ||
        ref==Core::IPatient::DrugsAtcAllergies) {
        qWarning() << "Updating Global Drugs Model";
        d->uidAllergies = patient()->value(Core::IPatient::DrugsUidAllergies).toStringList();
        d->atcAllergies = patient()->value(Core::IPatient::DrugsAtcAllergies).toStringList();
        d->innAllergies = patient()->value(Core::IPatient::DrugsInnAllergies).toStringList();
        d->uidAllergies.removeAll("");
        d->atcAllergies.removeAll("");
        d->innAllergies.removeAll("");
        d->testAtc = !d->atcAllergies.isEmpty();
        d->testUid = !d->uidAllergies.isEmpty();
        d->testInn = !d->innAllergies.isEmpty();
        qWarning() << d->innAllergies << d->testInn;
        d->clearDrugAllergyCache();
    }
}

QVariant GlobalDrugsModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (item.column() == DrugsDB::Constants::DRUGS_NAME) {
            return d->getConstructedDrugName(item.row());
        }
    } else if (role == Qt::BackgroundRole) {
        // test atc's patient allergies
        if (d->hasAllergie(item)) {
            QColor c = QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString());
            c.setAlpha(190);
            return c;
        }
        if (settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool()) {
            QModelIndex uid = index(item.row(), Constants::DRUGS_UID);
            if (d->UIDHasRecordedDosage(uid.data().toInt())) {
                QColor c = QColor(settings()->value(Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString());
                c.setAlpha(125);
                return c;
            }
        }

    } else if (role == Qt::ToolTipRole) {
        QString tmp = "<html><body>";
        if (d->hasAllergie(item)) {
            tmp += QString("<span style=\"align:center;vertical-align:middle\"><img src=\"%1\"><span style=\"color:red;font-weight:600\">WARNING : known allergy.</span><img src=\"%1\"></span><br />")
                   .arg(settings()->path(Core::ISettings::MediumPixmapPath) + QDir::separator() + QString(Core::Constants::ICONWARNING));
        }
        tmp += d->getConstructedDrugName(item.row()) + "<br />";
        // get composition
        if (settings()->value(Constants::S_SELECTOR_SHOWMOLECULES).toBool()) {
            QSqlTableModel compo(0, database());
            compo.setTable(drugsBase()->table(Constants::Table_COMPO));
            QHash<int, QString> where;
            where.insert(Constants::COMPO_UID, "=" + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_UID)).toString());
            compo.setFilter(drugsBase()->getWhereClause(Constants::Table_COMPO, where));
            compo.select();
            for(int i=0; i< compo.rowCount(); ++i) {
                tmp += "&nbsp;&nbsp;" + compo.data(compo.index(i, Constants::COMPO_MOL_NAME)).toString() + "<br />";
            }
        }
        // get form / route
        tmp += "&nbsp;&nbsp;&nbsp;&nbsp;" + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_FORM)).toString() + "<br />";
        tmp += "&nbsp;&nbsp;&nbsp;&nbsp;" + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_ROUTE)).toString() + "<br />";
        QString atc = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_ATC)).toString() + "</body></html>";
        if (atc.isEmpty())
            tmp += "&nbsp;&nbsp;&nbsp;&nbsp;No ATC found";
        else
            tmp += "&nbsp;&nbsp;&nbsp;&nbsp;" + atc;

        return tmp;
    } else if (role == Qt::DecorationRole) {
        if (d->hasAllergie(item))
            return theme()->icon(Core::Constants::ICONWARNING);
    }
    return QSqlTableModel::data(item,role);
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}
