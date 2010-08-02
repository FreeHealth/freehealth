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
#include <drugsbaseplugin/drugsdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>

#include <QList>
#include <QColor>
#include <QDir>
#include <QCache>
#include <QSqlError>
#include <QPersistentModelIndex>

#include <qtconcurrentrun.h>
#include <QFuture>
#include <QFutureWatcher>

#include <QDebug>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    GlobalDrugsModelPrivate(GlobalDrugsModel *parent) :
            q(parent)
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

    void clearDrugIntoleranceCache()
    {
        drugIntoleranceCache.clear();
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
        const QString &drugName = q->QSqlTableModel::data(q->index(row, Constants::DRUGS_NAME)).toString();
        if (drugsBase()->actualDatabaseInformations()) {
            QString tmp = drugsBase()->actualDatabaseInformations()->drugsNameConstructor;
            if (!tmp.isEmpty()) {
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_NAME), drugName);
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_FORM),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_FORM)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_ROUTE),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_ROUTE)).toString());
                const QString &s = q->QSqlTableModel::data(q->index(row, Constants::DRUGS_STRENGTH)).toString();
                if (s.count(";") < 3)
                    tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH), s);
                else
                    tmp.remove(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH));
                return tmp;
            }
        }
        return drugName;
    }

    static bool testAtcAllergies(const QStringList &atc, const int uid)
    {
        foreach(const QString &code, atc) {
            if (fullAtcAllergies.contains(code)) {
                drugAllergyCache.insert(uid, true);
                qWarning() << "  nnnnnn";
                return true;
            }
            foreach(const QString &atcClass, classAtcAllergies) {
                if (code.startsWith(atcClass)) {
                    drugAllergyCache.insert(uid, true);
                    return true;
                }
            }
        }
        return false;
    }

    static bool hasAllergy(const int uid, const QStringList &inns, const QStringList &atc)
    {
        if (drugAllergyCache.contains(uid)) {
            return drugAllergyCache.value(uid);
        }
        if (m_testInnAllergies) {
            // get all drugs inns
            foreach(const QString &druginn, inns) {
                if (innAllergies.contains(druginn)) {
                    drugAllergyCache.insert(uid, true);
                    return true;
                }
            }
        }
        if (m_testAtcAllergies) {
            if (testAtcAllergies(atc, uid))
                return true;
        }
        if (m_testUidAllergies) {
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

    static bool hasAllergy(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        int uid = model->index(item.row(), Constants::DRUGS_UID).data().toInt();
        if (drugAllergyCache.contains(uid)) {
            return drugAllergyCache.value(uid);
        }
        return false;
    }

    static void checkAllergy(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        int uid = model->index(item.row(), Constants::DRUGS_UID).data().toInt();
        if (drugAllergyCache.contains(uid)) {
            return;
        }

        drugAllergyCache.insert(uid, false);

        if (m_testInnAllergies) {
            // get all drugs inns
            const QStringList &inns = drugsBase()->getDrugInns(QString::number(uid));
            foreach(const QString &druginn, inns) {
                if (innAllergies.contains(druginn)) {
                    drugAllergyCache.insert(uid, true);
                    break;
                }
            }
        }
        if (m_testAtcAllergies) {
            // get all molecules ATC codes
            QStringList atcs;
            atcs << drugsBase()->getDrugCompositionAtcCodes(uid);
            atcs << model->index(item.row(), Constants::DRUGS_ATC).data().toString();
            atcs.removeAll("");
            testAtcAllergies(atcs, uid);
        }
        if (m_testUidAllergies) {
            if (uidAllergies.contains(QString::number(uid))) {
                drugAllergyCache.insert(uid, true);
            }
        }
        if (drugAllergyCache.size() > 10000) {
            drugAllergyCache.remove(drugAllergyCache.begin().key());
        }
    }

    static bool testAtcIntolerances(const QStringList &atc, const int uid)
    {
        foreach(const QString &code, atc) {
            if (fullAtcIntolerances.contains(code)) {
                drugIntoleranceCache.insert(uid, true);
                return true;
            }
            foreach(const QString &atcClass, classAtcIntolerances) {
                if (code.startsWith(atcClass)) {
                    drugIntoleranceCache.insert(uid, true);
                    return true;
                }
            }
        }
        return false;
    }

    static bool hasIntolerance(const int uid, const QStringList &inns, const QStringList &atc)
    {
        if (drugIntoleranceCache.contains(uid)) {
            return drugIntoleranceCache.value(uid);
        }
        if (m_testInnIntolerances) {
            // get all drugs inns
            foreach(const QString &druginn, inns) {
                if (innIntolerances.contains(druginn)) {
                    drugIntoleranceCache.insert(uid, true);
                    return true;
                }
            }
        }
        if (m_testAtcIntolerances) {
            if (testAtcIntolerances(atc, uid))
                return true;
        }
        if (m_testUidIntolerances) {
            if (uidIntolerances.contains(QString::number(uid))) {
                drugIntoleranceCache.insert(uid, true);
                return true;
            }
        }
        drugIntoleranceCache.insert(uid, false);

        if (drugIntoleranceCache.size() > 10000) {
            drugIntoleranceCache.remove(drugIntoleranceCache.begin().key());
        }

        return false;
    }

    static bool hasIntolerance(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        int uid = model->index(item.row(), Constants::DRUGS_UID).data().toInt();
        if (drugIntoleranceCache.contains(uid)) {
            return drugIntoleranceCache.value(uid);
        }
        return false;
    }

    static void checkIntolerance(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        int uid = model->index(item.row(), Constants::DRUGS_UID).data().toInt();
        if (drugIntoleranceCache.contains(uid)) {
            return;
        }

        drugIntoleranceCache.insert(uid, false);

        if (m_testInnIntolerances) {
            // get all drugs inns
            const QStringList &inns = DrugsDB::Internal::DrugsBase::instance()->getDrugInns(QString::number(uid));
            foreach(const QString &druginn, inns) {
                if (innIntolerances.contains(druginn)) {
                    drugIntoleranceCache.insert(uid, true);
                    break;
                }
            }
        }

        if (m_testAtcIntolerances) {
            // get all molecules ATC codes
            QStringList atcs;
            atcs << drugsBase()->getDrugCompositionAtcCodes(uid);
            atcs << model->index(item.row(), Constants::DRUGS_ATC).data().toString();
//            qWarning() << model->index(item.row(), Constants::DRUGS_NAME).data().toString() << atcs;
            testAtcIntolerances(atcs, uid);
        }

        if (m_testUidIntolerances) {
            if (uidIntolerances.contains(QString::number(uid))) {
                drugIntoleranceCache.insert(uid, true);
            }
        }

        if (drugIntoleranceCache.size() > 10000) {
            drugIntoleranceCache.remove(drugIntoleranceCache.begin().key());
        }
    }

    static QPersistentModelIndex checkPhysiology(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        checkAllergy(item, model);
        checkIntolerance(item, model);
        return item;
    }

    // For QFuture use
public:
    QVector< QFutureWatcher<QPersistentModelIndex> * > m_Watchers;

public:
    static QStringList fullAtcAllergies, classAtcAllergies, uidAllergies, innAllergies;
    static QStringList fullAtcIntolerances, classAtcIntolerances, uidIntolerances, innIntolerances;
    static bool m_testAtcAllergies, m_testUidAllergies, m_testInnAllergies;
    static bool m_testAtcIntolerances, m_testUidIntolerances, m_testInnIntolerances;
    static QVector<int> m_ProcessedUid;

private:
    static QHash<int, bool> drugAllergyCache;
    static QHash<int, bool> drugIntoleranceCache;
    static QList<int> m_CachedAvailableDosageForUID;
    static int numberOfInstances;
    GlobalDrugsModel *q;
};

QList<int> GlobalDrugsModelPrivate::m_CachedAvailableDosageForUID;
QHash<int, bool> GlobalDrugsModelPrivate::drugAllergyCache;
QHash<int, bool> GlobalDrugsModelPrivate::drugIntoleranceCache;
int GlobalDrugsModelPrivate::numberOfInstances;

QStringList GlobalDrugsModelPrivate::fullAtcAllergies, GlobalDrugsModelPrivate::classAtcAllergies;
QStringList GlobalDrugsModelPrivate::uidAllergies, GlobalDrugsModelPrivate::innAllergies;

QStringList GlobalDrugsModelPrivate::fullAtcIntolerances, GlobalDrugsModelPrivate::classAtcIntolerances;
QStringList GlobalDrugsModelPrivate::uidIntolerances, GlobalDrugsModelPrivate::innIntolerances;

bool GlobalDrugsModelPrivate::m_testAtcAllergies = false;
bool GlobalDrugsModelPrivate::m_testUidAllergies = false;
bool GlobalDrugsModelPrivate::m_testInnAllergies = false;

bool GlobalDrugsModelPrivate::m_testAtcIntolerances = false;
bool GlobalDrugsModelPrivate::m_testUidIntolerances = false;
bool GlobalDrugsModelPrivate::m_testInnIntolerances = false;

QVector<int> GlobalDrugsModelPrivate::m_ProcessedUid;

}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

void GlobalDrugsModel::updateAvailableDosages()  // static
{
    Internal::GlobalDrugsModelPrivate::updateCachedAvailableDosage();
}

bool GlobalDrugsModel::hasAllergy(const DrugsDB::Internal::DrugsData *drug)  // static
{
    /** \todo This needs improvements in drugsBase()->getDrug... */
    QStringList atcs;
    foreach(int code, drugsBase()->getLinkedAtcIds(drug->listOfCodeMolecules()))
        atcs << drugsBase()->getAtcCode(code);
    atcs << drug->ATC();
    return Internal::GlobalDrugsModelPrivate::hasAllergy(drug->UID(), drug->listOfInn(), atcs);
}

bool GlobalDrugsModel::hasIntolerance(const DrugsDB::Internal::DrugsData *drug)  // static
{
    QStringList atcs;
    foreach(int code, drugsBase()->getLinkedAtcIds(drug->listOfCodeMolecules()))
        atcs << drugsBase()->getAtcCode(code);
    atcs << drug->ATC();
    return Internal::GlobalDrugsModelPrivate::hasIntolerance(drug->UID(), drug->listOfInn(), atcs);
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

    refreshDrugsAllergies(Core::IPatient::DrugsAllergiesWithoutPrecision);

    drugsBase()->database().transaction();

    connect(drugsBase(), SIGNAL(dosageBaseHasChanged()), this, SLOT(updateCachedAvailableDosage()));
    connect(patient(), SIGNAL(dataChanged(int)), this, SLOT(refreshDrugsAllergies(int)));
}

GlobalDrugsModel::~GlobalDrugsModel()
{
    qWarning() << "GlobalDrugsModel::~GlobalDrugsModel()";
    qWarning() << d->m_Watchers.count();
    for(int i = 0; i < d->m_Watchers.count() ; ++i) {
        d->m_Watchers.at(i)->cancel();
    }
    qDeleteAll(d->m_Watchers);
    d->m_Watchers.clear();
    if (d) {
        delete d;
        d=0;
    }
}

void GlobalDrugsModel::refreshDrugsAllergies(const int ref)
{
    if (ref & Core::IPatient::DrugsUidAllergies) {
        d->uidAllergies = patient()->value(Core::IPatient::DrugsUidAllergies).toStringList();
        d->uidAllergies.removeAll("");
        d->m_testUidAllergies = !d->uidAllergies.isEmpty();
        d->clearDrugAllergyCache();
    }
    if (ref & Core::IPatient::DrugsInnAllergies) {
        d->innAllergies = patient()->value(Core::IPatient::DrugsInnAllergies).toStringList();
        d->innAllergies.removeAll("");
        d->m_testInnAllergies = !d->innAllergies.isEmpty();
        d->clearDrugAllergyCache();
    }
    if (ref & Core::IPatient::DrugsAtcAllergies) {
        foreach(const QString &atc, patient()->value(Core::IPatient::DrugsAtcAllergies).toStringList()) {
            if (atc.length()==7)
                d->fullAtcAllergies.append(atc);
            else
                d->classAtcAllergies.append(atc);
        }
        d->fullAtcAllergies.removeAll("");
        d->classAtcAllergies.removeAll("");
        d->m_testAtcAllergies = !d->fullAtcAllergies.isEmpty() || !d->classAtcAllergies.isEmpty();
        d->clearDrugAllergyCache();
    }
    if (ref & Core::IPatient::DrugsAtcIntolerances) {
        foreach(const QString &atc, patient()->value(Core::IPatient::DrugsAtcIntolerances).toStringList()) {
            if (atc.length()==7)
                d->fullAtcIntolerances.append(atc);
            else
                d->classAtcIntolerances.append(atc);
        }
        d->fullAtcIntolerances.removeAll("");
        d->classAtcIntolerances.removeAll("");
        d->m_testAtcIntolerances = !d->fullAtcIntolerances.isEmpty() || !d->classAtcIntolerances.isEmpty();
        d->clearDrugIntoleranceCache();
    }
    if (ref & Core::IPatient::DrugsInnIntolerances) {
        d->innIntolerances = patient()->value(Core::IPatient::DrugsInnIntolerances).toStringList();
        d->innIntolerances.removeAll("");
        d->m_testInnIntolerances = !d->innIntolerances.isEmpty();
        d->clearDrugIntoleranceCache();
    }
    if (ref & Core::IPatient::DrugsUidIntolerances) {
        d->uidIntolerances = patient()->value(Core::IPatient::DrugsUidIntolerances).toStringList();
        d->uidIntolerances.removeAll("");
        d->m_testUidIntolerances = !d->uidIntolerances.isEmpty();
        d->clearDrugIntoleranceCache();
    }

}

void GlobalDrugsModel::physiologyProcessed()
{
    QFutureWatcher<QPersistentModelIndex> *watch = static_cast< QFutureWatcher<QPersistentModelIndex> *>(sender());
    if (!watch) {
        return;
    }
    if (!watch->future().isFinished()) {
        Utils::Log::addMessage(this, "Future is not finished.");
        return;
    }
    QPersistentModelIndex idx = watch->result();
    int id = d->m_Watchers.indexOf(watch);
    if (id<0) {
        Utils::Log::addMessage(this, "FutureWatcher not in vector.");
        return;
    }
    d->m_Watchers.remove(id);

    qWarning() << d->m_Watchers.count();

    delete watch;
//    Q_EMIT dataChanged(idx, idx);
}

QVariant GlobalDrugsModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (item.column()) {
        case DrugsDB::Constants::DRUGS_NAME: return d->getConstructedDrugName(item.row());
        case DrugsDB::Constants::DRUGS_STRENGTH:
            {
                const QString &s = QSqlTableModel::data(item,role).toString();
                if (s.count(";") > 2)
                    return QVariant();
                else
                    return s;
            }
        }
        return QSqlTableModel::data(item,role);
    }

    // Cache processed UIDs for allergies and intolerances
    // If cached  --> start the process in a QFuture ; connect QFuture to Model::dataChanged()
    // If !cached --> ok to return the values
    int uid = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_UID)).toInt();
    if (!d->m_ProcessedUid.contains(uid)) {
        QFuture<QPersistentModelIndex> processPhysiology = QtConcurrent::run(DrugsDB::Internal::GlobalDrugsModelPrivate::checkPhysiology, item, this);
        QFutureWatcher<QPersistentModelIndex> *watch = new QFutureWatcher<QPersistentModelIndex>;
        watch->setFuture(processPhysiology);
        d->m_Watchers.append(watch);
        connect(watch, SIGNAL(finished()), this, SLOT(physiologyProcessed()));
        connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), watch, SLOT(cancel()));
        d->m_ProcessedUid.append(uid);
    } else {
        if (role == Qt::BackgroundRole) {
            // test atc's patient allergies
            if (d->hasAllergy(item, this)) {
                QColor c = QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString());
                c.setAlpha(190);
                return c;
            } else if (d->hasIntolerance(item, this)) {
                QColor c = QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString());
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
            if (d->hasAllergy(item, this)) {
                tmp += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                       .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONFORBIDDEN))
                       .arg(tr("KNOWN ALLERGY"));
            } else if (d->hasIntolerance(item, this)) {
                tmp += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                       .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONWARNING))
                       .arg(tr("KNOWN INTOLERANCE"));
            }
            tmp += "<b>" + d->getConstructedDrugName(item.row()) + "</b><br>";
            QString atc = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_ATC)).toString();
            // get form / route
            tmp += QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_FORM)).toString() + "<br />";
            tmp += QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_ROUTE)).toString() + "<br />";
            if (atc.isEmpty())
                tmp += tr("No ATC found");
            else
                tmp += atc;
            tmp += "<br>";
            // get composition
            if (settings()->value(Constants::S_SELECTOR_SHOWMOLECULES).toBool()) {
                int uid = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_UID)).toInt();
                QSqlTableModel compo(0, database());
                compo.setTable(drugsBase()->table(Constants::Table_COMPO));
                QHash<int, QString> where;
                where.insert(Constants::COMPO_UID, QString("=%1").arg(uid));
                compo.setFilter(drugsBase()->getWhereClause(Constants::Table_COMPO, where));
                compo.select();
                tmp += tr("Composition:<br>");
                for(int i=0; i< compo.rowCount(); ++i) {
                    tmp +=  "&nbsp;&nbsp;&nbsp;*&nbsp;" + compo.data(compo.index(i, Constants::COMPO_MOL_NAME)).toString() + "<br>";
                }
                tmp += tr("ATC codes (for interaction engine):<br>");
                if (!atc.isEmpty())
                    tmp += "&nbsp;&nbsp;&nbsp;" + drugsBase()->getDrugCompositionAtcCodes(uid).join(";") + ";" + atc + "<br>";
                else
                    tmp += "&nbsp;&nbsp;&nbsp;" + drugsBase()->getDrugCompositionAtcCodes(uid).join(";") + "<br>";
            }
            tmp += "</body></html>";

            return tmp;
        } else if (role == Qt::DecorationRole) {
            if (d->hasAllergy(item, this))
                return theme()->icon(Core::Constants::ICONFORBIDDEN);
            else if (d->hasIntolerance(item, this))
                return theme()->icon(Core::Constants::ICONWARNING);
        }
    }
    return QSqlTableModel::data(item,role);
//    return QVariant();
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}
