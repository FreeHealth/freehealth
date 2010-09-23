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
/**
  \class DrugsDB::GlobalDrugsModel
  This class provides the drugs model from the entire currently in use drugs database.
  The static members allow you to check drugs precautions (allergies and intolerances).
  The Drugs Precaution Model is always up to date.
*/

#include "globaldrugsmodel.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>
#include <drugsbaseplugin/drugsdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QList>
#include <QColor>
#include <QDir>
#include <QCache>
#include <QSqlError>
#include <QPersistentModelIndex>
#include <QTimer>
#include <QStandardItemModel>

#include <qtconcurrentrun.h>
#include <QFuture>
#include <QFutureWatcher>

#include <QDebug>

using namespace Trans::ConstantTranslations;
using namespace DrugsDB;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::Translators *translators() {return Core::ICore::instance()->translators();}

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
        if (numberOfInstances == 0) {
            m_CachedAvailableDosageForUID.clear();
            drugAllergyCache.clear();
            delete m_DrugsPrecautionsModel;
            m_DrugsPrecautionsModel = 0;
//            delete m_Timer;
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
        foreach(const QVariant &uid, drugsBase()->getAllUIDThatHaveRecordedDosages())
            m_CachedAvailableDosageForUID.append(uid.toString());
    }

    static bool UIDHasRecordedDosage(const QString &uid)
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

    static bool testAtcAllergies(const QStringList &atc, const QString &uid)
    {
        foreach(const QString &code, atc) {
            if (fullAtcAllergies.contains(code)) {
                drugAllergyCache.insert(uid, true);
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

    static bool hasAllergy(const QString &uid, const QStringList &inns, const QStringList &atc)
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
            if (uidAllergies.contains(uid)) {
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
        const QString &uid = model->index(item.row(), Constants::DRUGS_UID).data().toString();
        if (drugAllergyCache.contains(uid)) {
            return drugAllergyCache.value(uid);
        }
        return false;
    }

    static void checkAllergy(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        const QString &uid = model->index(item.row(), Constants::DRUGS_UID).data().toString();
        if (drugAllergyCache.contains(uid)) {
            return;
        }

        drugAllergyCache.insert(uid, false);

        if (m_testInnAllergies) {
            // get all drugs inns
            const QStringList &inns = drugsBase()->getDrugInns(uid);
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
//            atcs.clear();
        }
        if (m_testUidAllergies) {
            if (uidAllergies.contains(uid)) {
                drugAllergyCache.insert(uid, true);
            }
        }
        if (drugAllergyCache.size() > 10000) {
            drugAllergyCache.remove(drugAllergyCache.begin().key());
        }
    }

    static bool testAtcIntolerances(const QStringList &atc, const QString &uid)
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

    static bool hasIntolerance(const QString &uid, const QStringList &inns, const QStringList &atc)
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
            if (uidIntolerances.contains(uid)) {
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
        const QString &uid = model->index(item.row(), Constants::DRUGS_UID).data().toString();
        if (drugIntoleranceCache.contains(uid)) {
            return drugIntoleranceCache.value(uid);
        }
        return false;
    }

    static void checkIntolerance(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        const QString &uid = model->index(item.row(), Constants::DRUGS_UID).data().toString();
        if (drugIntoleranceCache.contains(uid)) {
            return;
        }

        drugIntoleranceCache.insert(uid, false);

        if (m_testInnIntolerances) {
            // get all drugs inns
            const QStringList &inns = DrugsDB::Internal::DrugsBase::instance()->getDrugInns(uid);
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
            testAtcIntolerances(atcs, uid);
//            atcs.clear();
        }

        if (m_testUidIntolerances) {
            if (uidIntolerances.contains(uid)) {
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

    static void addBranch(QStandardItem *rootAllergies, QStandardItem *rootIntolerances, const QString &name,
                          const QStringList &allergies, const QStringList &intolerances, bool atc, bool uids,
                          const QBrush &allergiesBrush, const QBrush &intolerancesBrush)
    {
        QStandardItem *allergiesItem = new QStandardItem(name);
        QStandardItem *intolerancesItem = new QStandardItem(name);
        allergiesItem->setForeground(allergiesBrush);
        intolerancesItem->setForeground(intolerancesBrush);
        rootAllergies->appendRow(allergiesItem);
        rootIntolerances->appendRow(intolerancesItem);
        //    allergies
        QStringList sorted = allergies;
        qSort(sorted);
        foreach(const QString &item, sorted) {
            QString lbl;
            if (atc)
                 lbl = drugsBase()->getAtcLabel(item);
            else if (uids)
                 lbl = drugsBase()->getDrugName(item);
            else lbl = item;
            if (!lbl.isEmpty()) {
                QStandardItem *i = new QStandardItem(lbl);
                i->setForeground(allergiesBrush);
                i->setToolTip(tkTr(Trans::Constants::ALLERGY_TO_1).arg(lbl));
                allergiesItem->appendRow(i);
            }
        }
        //    intol
        sorted.clear();
        sorted = intolerances;
        qSort(sorted);
        foreach(const QString &item, sorted) {
            QString lbl;
            if (atc)
                 lbl = drugsBase()->getAtcLabel(item);
            else if (uids)
                 lbl = drugsBase()->getDrugName(item);
            else lbl = item;
            if (!lbl.isEmpty()) {
                QStandardItem *i = new QStandardItem(lbl);
                i->setToolTip(tkTr(Trans::Constants::INTOLERANCE_TO_1).arg(lbl));
                i->setForeground(intolerancesBrush);
                intolerancesItem->appendRow(i);
            }
        }
    }

    static void updateDrugsPrecautionsModel()
    {
        if (!GlobalDrugsModelPrivate::m_DrugsPrecautionsModel)
            GlobalDrugsModelPrivate::m_DrugsPrecautionsModel = new QStandardItemModel();
        GlobalDrugsModelPrivate::m_DrugsPrecautionsModel->clear();
        QFont bold;
        bold.setBold(true);
        QStandardItem *rootItem = GlobalDrugsModelPrivate::m_DrugsPrecautionsModel->invisibleRootItem();
        if (GlobalDrugsModelPrivate::fullAtcAllergies.isEmpty() &&
            GlobalDrugsModelPrivate::classAtcAllergies.isEmpty() &&
            GlobalDrugsModelPrivate::uidAllergies.isEmpty() &&
            GlobalDrugsModelPrivate::innAllergies.isEmpty() &&
            GlobalDrugsModelPrivate::fullAtcIntolerances.isEmpty() &&
            GlobalDrugsModelPrivate::classAtcIntolerances.isEmpty() &&
            GlobalDrugsModelPrivate::uidIntolerances.isEmpty() &&
            GlobalDrugsModelPrivate::innIntolerances.isEmpty()) {
            QStandardItem *uniqueItem = new QStandardItem(tkTr(Trans::Constants::NO_ALLERGIES_INTOLERANCES));
            uniqueItem->setFont(bold);
            rootItem->appendRow(uniqueItem);
        } else {
            QStandardItem *allergiesItem = new QStandardItem(tkTr(Trans::Constants::KNOWN_ALLERGIES));
            QStandardItem *intolerancesItem = new QStandardItem(tkTr(Trans::Constants::KNOWN_INTOLERANCES));
            allergiesItem->setFont(bold);
            intolerancesItem->setFont(bold);
            QBrush allergiesBrush = QBrush(QColor(settings()->value(DrugsDB::Constants::S_ALLERGYBACKGROUNDCOLOR).toString()).darker(300));
            QBrush intolerancesBrush = QBrush(QColor(settings()->value(DrugsDB::Constants::S_INTOLERANCEBACKGROUNDCOLOR).toString()).darker(300));
            allergiesItem->setForeground(allergiesBrush);
            intolerancesItem->setForeground(intolerancesBrush);

            rootItem->appendRow(allergiesItem);
            rootItem->appendRow(intolerancesItem);

            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::ATC), QStringList() << GlobalDrugsModelPrivate::fullAtcAllergies << GlobalDrugsModelPrivate::classAtcAllergies, QStringList() << GlobalDrugsModelPrivate::fullAtcIntolerances << GlobalDrugsModelPrivate::classAtcIntolerances, true, false, allergiesBrush, intolerancesBrush);
            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::DRUGS), GlobalDrugsModelPrivate::uidAllergies, GlobalDrugsModelPrivate::uidIntolerances, false, true, allergiesBrush, intolerancesBrush);
            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::INN), GlobalDrugsModelPrivate::innAllergies, GlobalDrugsModelPrivate::innIntolerances, false, false, allergiesBrush, intolerancesBrush);
        }
    }


    // For QFuture use
public:
    QVector< QFutureWatcher<QPersistentModelIndex> * > m_Watchers;

public:
    static QStringList fullAtcAllergies, classAtcAllergies, uidAllergies, innAllergies;
    static QStringList fullAtcSuspectedAllergies, classAtcSuspectedAllergies, uidSuspectedAllergies, innSuspectedAllergies;
    static QStringList fullAtcIntolerances, classAtcIntolerances, uidIntolerances, innIntolerances;
    static bool m_testAtcAllergies, m_testUidAllergies, m_testInnAllergies;
    static bool m_testAtcIntolerances, m_testUidIntolerances, m_testInnIntolerances;
    static QVector<QString> m_ProcessedUid;
    static QVector<QPersistentModelIndex> m_UidToProcess;
    static QStandardItemModel *m_DrugsPrecautionsModel;
    QTimer *m_Timer;
    static int numberOfInstances;

private:
    static QHash<QString, bool> drugAllergyCache;
    static QHash<QString, bool> drugIntoleranceCache;
    static QList<QString> m_CachedAvailableDosageForUID;
    GlobalDrugsModel *q;
};

QList<QString> GlobalDrugsModelPrivate::m_CachedAvailableDosageForUID;
QHash<QString, bool> GlobalDrugsModelPrivate::drugAllergyCache;
QHash<QString, bool> GlobalDrugsModelPrivate::drugIntoleranceCache;
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

QVector<QString> GlobalDrugsModelPrivate::m_ProcessedUid;
QVector<QPersistentModelIndex> GlobalDrugsModelPrivate::m_UidToProcess;

QStandardItemModel *GlobalDrugsModelPrivate::m_DrugsPrecautionsModel = 0;

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
    return Internal::GlobalDrugsModelPrivate::hasAllergy(drug->UID().toString(), drug->listOfInn(), atcs);
}

bool GlobalDrugsModel::hasIntolerance(const DrugsDB::Internal::DrugsData *drug)  // static
{
    QStringList atcs;
    foreach(int code, drugsBase()->getLinkedAtcIds(drug->listOfCodeMolecules()))
        atcs << drugsBase()->getAtcCode(code);
    atcs << drug->ATC();
    return Internal::GlobalDrugsModelPrivate::hasIntolerance(drug->UID().toString(), drug->listOfInn(), atcs);
}

QStandardItemModel *GlobalDrugsModel::drugsPrecautionsModel()
{
    return GlobalDrugsModelPrivate::m_DrugsPrecautionsModel;
}


GlobalDrugsModel::GlobalDrugsModel(QObject *parent) :
        QSqlTableModel(parent, QSqlDatabase::database(Constants::DB_DRUGS_NAME)),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate(this);
    d->updateCachedAvailableDosage();

//    d->m_Timer = new QTimer;
//    d->m_Timer->setSingleShot(true);
//    connect(d->m_Timer, SIGNAL(timeout()), this, SLOT(processPhysiology()));

    setTable(drugsBase()->table(Constants::Table_DRUGS));
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert(Constants::DRUGS_MARKET, "=1");
    setFilter(drugsBase()->getWhereClause(Constants::Table_DRUGS, where));
    select();

    refreshDrugsPrecautions(patient()->index(0, Core::IPatient::DrugsAllergiesWithoutPrecision), patient()->index(0, Core::IPatient::DrugsAllergiesWithoutPrecision));

//    drugsBase()->database().transaction();

    connect(drugsBase(), SIGNAL(dosageBaseHasChanged()), this, SLOT(updateCachedAvailableDosage()));
    connect(drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsDatabaseChanged()));
    connect(patient(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(refreshDrugsPrecautions(QModelIndex, QModelIndex)));
    connect(translators(), SIGNAL(languageChanged()), this, SLOT(onDrugsDatabaseChanged()));
}

GlobalDrugsModel::~GlobalDrugsModel()
{
    qWarning() << "GlobalDrugsModel::~GlobalDrugsModel() // number of instances" << d->numberOfInstances;
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

void GlobalDrugsModel::onDrugsDatabaseChanged()
{
    GlobalDrugsModelPrivate::updateDrugsPrecautionsModel();
}

/**
  \brief slot to keep global model datas up to date
  \sa Core::IPatient
*/
void GlobalDrugsModel::refreshDrugsPrecautions(const QModelIndex &topleft, const QModelIndex &bottomright)
{
    bool refreshModel = false;
    int ref = topleft.column();
    if (ref & Core::IPatient::DrugsUidAllergies) {
        d->uidAllergies = patient()->data(Core::IPatient::DrugsUidAllergies).toStringList();
        d->uidAllergies.removeAll("");
        d->m_testUidAllergies = !d->uidAllergies.isEmpty();
        d->clearDrugAllergyCache();
        refreshModel = true;
    }
    if (ref & Core::IPatient::DrugsInnAllergies) {
        d->innAllergies = patient()->data(Core::IPatient::DrugsInnAllergies).toStringList();
        d->innAllergies.removeAll("");
        d->m_testInnAllergies = !d->innAllergies.isEmpty();
        d->clearDrugAllergyCache();
        refreshModel = true;
    }
    if (ref & Core::IPatient::DrugsAtcAllergies) {
        d->fullAtcAllergies.clear();
        d->classAtcAllergies.clear();
        foreach(const QString &atc, patient()->data(Core::IPatient::DrugsAtcAllergies).toStringList()) {
            if (atc.length()==7)
                d->fullAtcAllergies.append(atc);
            else
                d->classAtcAllergies.append(atc);
        }
        d->fullAtcAllergies.removeAll("");
        d->classAtcAllergies.removeAll("");
        d->m_testAtcAllergies = !d->fullAtcAllergies.isEmpty() || !d->classAtcAllergies.isEmpty();
        d->clearDrugAllergyCache();
        refreshModel = true;
    }
    if (ref & Core::IPatient::DrugsAtcIntolerances) {
        d->fullAtcIntolerances.clear();
        d->classAtcIntolerances.clear();
        foreach(const QString &atc, patient()->data(Core::IPatient::DrugsAtcIntolerances).toStringList()) {
            if (atc.length()==7)
                d->fullAtcIntolerances.append(atc);
            else
                d->classAtcIntolerances.append(atc);
        }
        d->fullAtcIntolerances.removeAll("");
        d->classAtcIntolerances.removeAll("");
        d->m_testAtcIntolerances = !d->fullAtcIntolerances.isEmpty() || !d->classAtcIntolerances.isEmpty();
        d->clearDrugIntoleranceCache();
        refreshModel = true;
    }
    if (ref & Core::IPatient::DrugsInnIntolerances) {
        d->innIntolerances = patient()->data(Core::IPatient::DrugsInnIntolerances).toStringList();
        d->innIntolerances.removeAll("");
        d->m_testInnIntolerances = !d->innIntolerances.isEmpty();
        d->clearDrugIntoleranceCache();
        refreshModel = true;
    }
    if (ref & Core::IPatient::DrugsUidIntolerances) {
        d->uidIntolerances = patient()->data(Core::IPatient::DrugsUidIntolerances).toStringList();
        d->uidIntolerances.removeAll("");
        d->m_testUidIntolerances = !d->uidIntolerances.isEmpty();
        d->clearDrugIntoleranceCache();
        refreshModel = true;
    }
    d->m_ProcessedUid.clear();
    if (refreshModel)
        d->updateDrugsPrecautionsModel();
    reset();
}

//void GlobalDrugsModel::physiologyProcessed()
//{
//    // OBSOLETE
//    QFutureWatcher<QPersistentModelIndex> *watch = static_cast< QFutureWatcher<QPersistentModelIndex> *>(sender());
//    if (!watch) {
//        return;
//    }
//    if (!watch->future().isFinished()) {
//        Utils::Log::addMessage(this, "Future is not finished.");
//        return;
//    }
//    QPersistentModelIndex idx = watch->result();
//    int id = d->m_Watchers.indexOf(watch);
//    if (id<0) {
//        Utils::Log::addMessage(this, "FutureWatcher not in vector.");
//        return;
//    }

//    Q_EMIT dataChanged(idx, idx);

//    d->m_Watchers.remove(id);
//    delete watch;
//    qWarning() << d->m_Watchers.count();// << idx.data();
//}

//void GlobalDrugsModel::processPhysiology()
//{
//    // OBSOLETE
//    qWarning() << " GlobalDrugsModel::processPhysiology()" <<  d->m_UidToProcess.count();
//    for(int i = 0 ; i < d->m_UidToProcess.count(); ++i) {
////        QFuture<QPersistentModelIndex> processPhysiology = QtConcurrent::run(DrugsDB::Internal::GlobalDrugsModelPrivate::checkPhysiology, d->m_UidToProcess.at(i), this);
////        QFutureWatcher<QPersistentModelIndex> *watch = new QFutureWatcher<QPersistentModelIndex>;
////        watch->setFuture(processPhysiology);
////        d->m_Watchers.append(watch);
////        connect(watch, SIGNAL(finished()), this, SLOT(physiologyProcessed()));
////        connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), watch, SLOT(cancel()));

//        QFuture<QPersistentModelIndex> processPhysiology = QtConcurrent::run(DrugsDB::Internal::GlobalDrugsModelPrivate::checkPhysiology, d->m_UidToProcess.at(i), this);
//        processPhysiology.waitForFinished();
//        Q_EMIT dataChanged(d->m_UidToProcess.at(i), d->m_UidToProcess.at(i));
//    }
//    d->m_UidToProcess.clear();
//}

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
        return QSqlTableModel::data(item, role);
    }

    // Cache processed UIDs for allergies and intolerances
    // If cached  --> start the process in a QFuture ; connect QFuture to Model::dataChanged()
    // If !cached --> ok to return the values
    const QString &uid = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_UID)).toString();

    if (!d->m_ProcessedUid.contains(uid)) {
//        d->m_UidToProcess.append(item);
        d->m_ProcessedUid.append(uid);
        QFuture<QPersistentModelIndex> processPhysiology = QtConcurrent::run(DrugsDB::Internal::GlobalDrugsModelPrivate::checkPhysiology, item, this);
        processPhysiology.waitForFinished();
//        if (d->m_UidToProcess.count() == 20) {
//            d->m_Timer->start();
//            qApp->processEvents();
//        } else {
//            d->m_Timer->start(1);
//        }
    }
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
            if (d->UIDHasRecordedDosage(uid.data().toString())) {
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
    } else if (role == Qt::DecorationRole && item.column()==Constants::DRUGS_NAME) {
        if (d->hasAllergy(item, this))
            return theme()->icon(Core::Constants::ICONFORBIDDEN);
        else if (d->hasIntolerance(item, this))
            return theme()->icon(Core::Constants::ICONWARNING);
    }
//    return QSqlTableModel::data(item,role);
        return QVariant();
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}
