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
#include <QSqlQueryModel>

#include <qtconcurrentrun.h>
#include <QFuture>
#include <QFutureWatcher>

#include <QDebug>

using namespace Trans::ConstantTranslations;
using namespace DrugsDB;
using namespace Internal;

/** \todo code allergies/intolerances to drugs __uids__ */

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *base() {return DrugsDB::Internal::DrugsBase::instance();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::Translators *translators() {return Core::ICore::instance()->translators();}

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    enum ModelPrivateRepresentation {
        Priv_DID = 0,
        Priv_BrandName,
        Priv_Strength,
        Priv_ATC_ID,
        Priv_Market
    };

    GlobalDrugsModelPrivate(GlobalDrugsModel *parent) :
            m_SearchMode(0), q(parent)
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

    void setQueryModelSearchMode(const int searchMode)
    {
        m_SearchMode = searchMode;
        switch (m_SearchMode)
        {
        case GlobalDrugsModel::SearchByBrandName:
            {
                m_SqlQueryWithoutWhere = base()->select(Constants::Table_DRUGS, QList<int>()
                                     << Constants::DRUGS_DID
                                     << Constants::DRUGS_NAME
                                     << Constants::DRUGS_STRENGTH
                                     << Constants::DRUGS_ATC_ID
                                     << Constants::DRUGS_MARKET);
                break;
            }
        case GlobalDrugsModel::SearchByMolecularName:
            {
                Utils::FieldList get;
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_DID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_NAME);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_ATC_ID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_MARKET);
                Utils::JoinList joins;
                joins << Utils::Join(Constants::Table_DRUGS, Constants::DRUGS_DID, Constants::Table_COMPO, Constants::COMPO_DID);
                joins << Utils::Join(Constants::Table_COMPO, Constants::COMPO_MID, Constants::Table_MOLS, Constants::MOLS_MID);
                m_SqlQueryWithoutWhere = base()->select(get, joins);
                break;
            }
        case GlobalDrugsModel::SearchByInnName:
            {
                Utils::FieldList get;
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_DID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_NAME);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_ATC_ID);
                get << Utils::Field(Constants::Table_DRUGS, Constants::DRUGS_MARKET);
                Utils::JoinList joins;
                joins << Utils::Join(Constants::Table_COMPO, Constants::COMPO_DID, Constants::Table_DRUGS, Constants::DRUGS_DID);
                joins << Utils::Join(Constants::Table_LK_MOL_ATC, Constants::LK_MID, Constants::Table_COMPO, Constants::COMPO_MID);
                joins << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID, Constants::Table_LK_MOL_ATC, Constants::LK_ATC_ID);
                joins << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID);
                joins << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
                m_SqlQueryWithoutWhere = base()->select(get, joins);
                break;
            }
        }
        if (!m_Filter.isEmpty())
            q->setQuery(m_SqlQueryWithoutWhere + "WHERE " + m_Filter, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        else
            q->setQuery(m_SqlQueryWithoutWhere, QSqlDatabase::database(Constants::DB_DRUGS_NAME));

    }

    void setQueryModelFilter(const QString &searchFor)
    {
        switch (m_SearchMode)
        {
        case GlobalDrugsModel::SearchByBrandName:
            {
                m_Filter = base()->fieldName(Constants::Table_DRUGS, Constants::DRUGS_NAME) + " LIKE '" + searchFor + "%'";
                break;
            }
        case GlobalDrugsModel::SearchByMolecularName:
            {
                m_Filter = base()->fieldName(Constants::Table_MOLS, Constants::MOLS_NAME) + " LIKE '" + searchFor + "%'";
                break;
            }
        case GlobalDrugsModel::SearchByInnName:
            {
                m_Filter = base()->fieldName(Constants::Table_LABELS, Constants::LABELS_LABEL) + " LIKE '" + searchFor + "%'";
                break;
            }
        }
        if (!m_Filter.isEmpty())
            q->setQuery(m_SqlQueryWithoutWhere + "WHERE " + m_Filter, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        else
            q->setQuery(m_SqlQueryWithoutWhere, QSqlDatabase::database(Constants::DB_DRUGS_NAME));
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
        foreach(const QVariant &uid, base()->getAllUIDThatHaveRecordedDosages())
            m_CachedAvailableDosageForUID.append(uid.toString());
    }

    static bool UIDHasRecordedDosage(const QString &uid)
    {
        return m_CachedAvailableDosageForUID.contains(uid);
    }

    QString getConstructedDrugName(const int row)
    {
        const QString &drugName = q->QSqlQueryModel::data(q->index(row, Priv_BrandName)).toString();
        if (base()->actualDatabaseInformations()) {
            QString tmp = base()->actualDatabaseInformations()->drugsNameConstructor;
            if (!tmp.isEmpty()) {
                tmp.replace(base()->fieldName(Constants::Table_DRUGS, Constants::DRUGS_NAME), drugName);
                tmp.replace("FORM", q->data(q->index(row, GlobalDrugsModel::Routes)).toString());
                tmp.replace("ROUTE", q->data(q->index(row, GlobalDrugsModel::Forms)).toString());
                QString s = q->QSqlQueryModel::data(q->index(row, Priv_Strength)).toString();
                if (s.count(";") < 3)
                    s.clear();
                tmp.replace(base()->fieldName(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH), s);
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
        const QString &uid = model->QSqlQueryModel::data(model->index(item.row(), Priv_DID)).toString();
        if (drugAllergyCache.contains(uid)) {
            return drugAllergyCache.value(uid);
        }
        return false;
    }

    static void checkAllergy(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        const QString &uid = model->QSqlQueryModel::data(model->index(item.row(), Priv_DID)).toString();
        if (drugAllergyCache.contains(uid)) {
            return;
        }

        drugAllergyCache.insert(uid, false);

        if (m_testInnAllergies) {
            // get all drugs inns
            const QStringList &inns = base()->getDrugInns(uid);
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
            atcs << base()->getDrugCompositionAtcCodes(uid);
            atcs << base()->getAtcCode(model->QSqlQueryModel::index(item.row(), GlobalDrugsModelPrivate::Priv_ATC_ID).data().toInt());
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
        const QString &uid = model->QSqlQueryModel::data(model->index(item.row(), Priv_DID)).toString();
        if (drugIntoleranceCache.contains(uid)) {
            return drugIntoleranceCache.value(uid);
        }
        return false;
    }

    static void checkIntolerance(const QPersistentModelIndex &item, const GlobalDrugsModel *model)
    {
        const QString &uid = model->QSqlQueryModel::data(model->index(item.row(), Priv_DID)).toString();
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
            atcs << base()->getDrugCompositionAtcCodes(uid);
            atcs << base()->getAtcCode(model->index(item.row(), Constants::DRUGS_ATC_ID).data().toInt());
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
                 lbl = base()->getAtcLabel(item);
            else if (uids)
                 lbl = base()->getDrugName(item);
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
                 lbl = base()->getAtcLabel(item);
            else if (uids)
                 lbl = base()->getDrugName(item);
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

            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::ATC), QStringList() << GlobalDrugsModelPrivate::fullAtcAllergies << GlobalDrugsModelPrivate::classAtcAllergies, QStringList() << GlobalDrugsModelPrivate::fullAtcIntolerances << GlobalDrugsModelPrivate::classAtcIntolerances, true, false, allergiesBrush, intolerancesBrush);
            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::DRUGS), GlobalDrugsModelPrivate::uidAllergies, GlobalDrugsModelPrivate::uidIntolerances, false, true, allergiesBrush, intolerancesBrush);
            addBranch(allergiesItem, intolerancesItem, tkTr(Trans::Constants::INN), GlobalDrugsModelPrivate::innAllergies, GlobalDrugsModelPrivate::innIntolerances, false, false, allergiesBrush, intolerancesBrush);

            rootItem->appendRow(allergiesItem);
            rootItem->appendRow(intolerancesItem);
        }
    }


    // For QFuture use
public:
    int m_SearchMode;
    QString m_SqlQueryWithoutWhere, m_Filter;
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

bool GlobalDrugsModel::hasAllergy(const IDrug *drug)  // static
{
//    QTime t;
//    t.start();
    bool ret = Internal::GlobalDrugsModelPrivate::hasAllergy(drug->drugId().toString(), drug->listOfInn(), drug->allAtcCodes());
//    Utils::Log::logTimeElapsed(t, 0, Q_FUNC_INFO);
    return ret;
}

bool GlobalDrugsModel::hasIntolerance(const IDrug *drug)  // static
{
    return Internal::GlobalDrugsModelPrivate::hasIntolerance(drug->drugId().toString(), drug->listOfInn(), drug->allAtcCodes());
}

QStandardItemModel *GlobalDrugsModel::drugsPrecautionsModel()
{
    return GlobalDrugsModelPrivate::m_DrugsPrecautionsModel;
}


/** \todo Add drug source management */
GlobalDrugsModel::GlobalDrugsModel(const SearchMode searchMode, QObject *parent) :
        QSqlQueryModel(parent),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate(this);
    d->updateCachedAvailableDosage();
    d->setQueryModelSearchMode(searchMode);

    refreshDrugsPrecautions(patient()->index(0, Core::IPatient::DrugsAllergiesWithoutPrecision), patient()->index(0, Core::IPatient::DrugsAllergiesWithoutPrecision));

    connect(base(), SIGNAL(dosageBaseHasChanged()), this, SLOT(updateCachedAvailableDosage()));
    connect(base(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsDatabaseChanged()));
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

void GlobalDrugsModel::setSearchMode(const int searchMode)
{
    d->setQueryModelSearchMode(searchMode);
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
    QModelIndex idx = index(item.row(), GlobalDrugsModelPrivate::Priv_DID);
    const QString &uid = QSqlQueryModel::data(idx).toString();

//    qWarning() << uid << role << QSqlQueryModel::data(index(item.row(), GlobalDrugsModelPrivate::Priv_DID));
    if (role == Qt::DisplayRole) {
        switch (item.column()) {
        case DrugId: return QSqlQueryModel::data(index(item.row(), GlobalDrugsModelPrivate::Priv_DID));
        case BrandName: return d->getConstructedDrugName(item.row());
        case Strength:
            {
                const QString &s = QSqlQueryModel::data(QSqlQueryModel::index(item.row(), GlobalDrugsModelPrivate::Priv_Strength)).toString();
                if (s.count(";") > 3)
                    return QVariant();
                return s;
            }
        case Routes:
            {
                return base()->getRouteLabels(uid.toInt()).join(", ");
//                QString lang = QLocale().name().left(2);
//                QStringList v;
//                if (routes.keys().contains(lang)) {
//                    v = routes.values(lang);
//                } else {
//                    v = routes.values("en");
//                }
//                return v.join(", ");
            }
        case Forms:
            {
                return base()->getFormLabels(uid.toInt()).join(", ");
//                QString lang = QLocale().name().left(2);
//                QStringList v;
//                if (forms.keys().contains(lang)) {
//                    v = forms.values(lang);
//                } else {
//                    v = forms.values("xx");
//                }
//                return v.join(", ");
            }
        }
        return QVariant();
    }

    // Cache processed UIDs for allergies and intolerances
    // If cached  --> start the process in a QFuture ; connect QFuture to Model::dataChanged()
    // If !cached --> ok to return the values

    if (!d->m_ProcessedUid.contains(uid)) {
        d->m_ProcessedUid.append(uid);
        QFuture<QPersistentModelIndex> processPhysiology = QtConcurrent::run(DrugsDB::Internal::GlobalDrugsModelPrivate::checkPhysiology, item, this);
        processPhysiology.waitForFinished();
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
            if (d->UIDHasRecordedDosage(uid)) {
                QColor c = QColor(settings()->value(Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString());
                c.setAlpha(125);
                return c;
            }
        }
    } else if (role == Qt::ToolTipRole) {
        QString tmp = "<html><body>";
        // Allergy Intolerance ?
        if (d->hasAllergy(item, this)) {
            tmp += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                   .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONFORBIDDEN))
                   .arg(tr("KNOWN ALLERGY"));
        } else if (d->hasIntolerance(item, this)) {
            tmp += QString("<table width=100%><tr><td><img src=\"%1\"></td><td width=100% align=center><span style=\"color:red;font-weight:600\">%2</span></td><td><img src=\"%1\"></span></td></tr></table><br>")
                   .arg(settings()->path(Core::ISettings::SmallPixmapPath) + QDir::separator() + QString(Core::Constants::ICONWARNING))
                   .arg(tr("KNOWN INTOLERANCE"));
        }

        // Name, ATC and UID
        QString atc = base()->getAtcCode(QSqlQueryModel::data(QSqlQueryModel::index(item.row(), GlobalDrugsModelPrivate::Priv_ATC_ID)).toInt());
        if (!atc.isEmpty())
            atc.prepend(" ; ATC: ");
        QString uidName = "UID";
        if (base()->actualDatabaseInformations()) {
            if (!base()->actualDatabaseInformations()->drugsUidName.isEmpty())
                uidName = base()->actualDatabaseInformations()->drugsUidName;
        }
        // Marketed infos
        QString mark;
        /** \todo code here */
//        mark = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_AUTHORIZATION)).toString();
//        if (!mark.isEmpty()) {
//            mark.prepend("<br>");
//            mark += " | " + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_TYPE_MP)).toString();
//        }

        // Create tooltip
        tmp += QString("<table border=1 cellpadding=2 cellspacing=2 width=100%>"
                        " <tr>"
                        "   <td colspan=2 rowspan=1 align=center>"
                        "       <span style=\"font-weight: bold;\">%1</span>"
                        "       %2"
                        "   </td>"
                        " </tr>"
                        " <tr>"
                        "   <td colspan=2 rowspan=1>"
                        "       %3"
                        "       <br>%4"
                        "       <br>%5%6%7"
                        "   </td>"
                        " </tr>")
                .arg(d->getConstructedDrugName(item.row()))
                .arg(mark)
                .arg(tr("Form(s): ") + base()->getFormLabels(uid).join(", "))
                .arg(tr("Route(s): ") + base()->getRouteLabels(uid).join(", "))
                .arg(tr("Identifier(s): ") + uidName + ":")
                .arg(uid)
                .arg(atc)
                ;

        // get composition
        if (settings()->value(Constants::S_SELECTOR_SHOWMOLECULES).toBool()) {
            QHash<QString, QString> compo = base()->getDrugFullComposition(uid);
            if (!compo.isEmpty()) {
                QHashIterator<QString, QString> i(compo);
                while (i.hasNext()) {
                    i.next();
                    tmp += QString("<tr><td>%1</td><td>%2</td></tr>")
                           .arg(i.key())
                           .arg(i.value());
                }
            } else {
                QStringList mols = base()->getDrugMolecularComposition(uid);
                foreach(const QString &s, mols) {
                    tmp += QString("<tr><td>%1</td><td>&nbsp;</td></tr>")
                           .arg(s);
                }
            }
            // END
//            tmp += tr("ATC codes (for interaction engine):<br>");
//            if (!atc.isEmpty())
//                tmp += "&nbsp;&nbsp;&nbsp;" + base()->getDrugCompositionAtcCodes(uid).join(";") + ";" + atc + "<br>";
//            else
//                tmp += "&nbsp;&nbsp;&nbsp;" + base()->getDrugCompositionAtcCodes(uid).join(";") + "<br>";
        }
        tmp += "</table></body></html>";

        return tmp;
    } else if (role == Qt::DecorationRole && item.column()==BrandName) {
        if (d->hasAllergy(item, this))
            return theme()->icon(Core::Constants::ICONFORBIDDEN);
        else if (d->hasIntolerance(item, this))
            return theme()->icon(Core::Constants::ICONWARNING);
    }
    return QVariant();
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::setFilter(const QString &searchFor)
{
    d->setQueryModelFilter(searchFor);
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}
