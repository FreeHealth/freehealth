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
/**
 * \class DDI::Internal::DrugDrugInteractionTableModel
 * Table model over the DDI database. The model is sorted by DDI Ids (db primary key).
 */

#include "drugdruginteractiontablemodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/interactors/druginteractortablemodel.h>
#include <ddiplugin/ddi/drugdruginteraction.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_msgerror.h>

#include <QFont>
#include <QLocale>
#include <QDate>
#include <QColor>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
static inline DDI::Internal::DDIDatabase &ddiBase() {return DDI::DDICore::instance()->database();}

namespace DDI {
namespace Internal {
class DrugDrugInteractionTableModelPrivate
{
public:
    DrugDrugInteractionTableModelPrivate(DrugDrugInteractionTableModel *parent) :
        _sql(0),
        q(parent)
    {
        // Be aware that DrugDrugInteractionEditorWidget ui->comboLevel must be sync with this part !!!
        _levelsToComboIndex.insert("C", 0);
        _levelsToComboIndex.insert("D", 1);
        _levelsToComboIndex.insert("450", 2);
        _levelsToComboIndex.insert("Y", 3);
        _levelsToComboIndex.insert("T", 4);
        _levelsToComboIndex.insert("P", 5);
        _levelsToComboIndex.insert("I", 6);
    }

    ~DrugDrugInteractionTableModelPrivate()
    {
    }

    void createSqlModel()
    {
        _sql = new QSqlTableModel(q, ddiBase().database());
        _sql->setTable(ddiBase().table(Constants::Table_DDI));
        _sql->setEditStrategy(QSqlTableModel::OnManualSubmit);

        QObject::connect(_sql, SIGNAL(primeInsert(int,QSqlRecord&)), q, SLOT(populateNewRowWithDefault(int, QSqlRecord&)));
        QObject::connect(_sql, SIGNAL(layoutAboutToBeChanged()), q, SIGNAL(layoutAboutToBeChanged()));
        QObject::connect(_sql, SIGNAL(layoutChanged()), q, SIGNAL(layoutChanged()));
        QObject::connect(_sql, SIGNAL(modelAboutToBeReset()), q, SIGNAL(modelAboutToBeReset()));
        QObject::connect(_sql, SIGNAL(modelReset()), q, SIGNAL(modelReset()));
    }

    int modelColumnToSqlColumn(const int modelColumn)
    {
        int sql = -1;
        switch (modelColumn) {
        case DrugDrugInteractionTableModel::Id: sql = Constants::DDI_ID; break;
        case DrugDrugInteractionTableModel::Uid: sql = Constants::DDI_UID; break;
        case DrugDrugInteractionTableModel::IsValid: sql = Constants::DDI_ISVALID; break;
        case DrugDrugInteractionTableModel::FirstInteractorUid: sql = Constants::DDI_FIRSTINTERACTORUID; break;
        case DrugDrugInteractionTableModel::SecondInteractorUid: sql = Constants::DDI_SECONDINTERACTORUID; break;
        case DrugDrugInteractionTableModel::FirstInteractorLabel: sql = Constants::DDI_FIRSTINTERACTORUID; break;
        case DrugDrugInteractionTableModel::SecondInteractorLabel: sql = Constants::DDI_SECONDINTERACTORUID; break;
        case DrugDrugInteractionTableModel::IsReviewed: sql = Constants::DDI_ISREVIEWED; break;
        case DrugDrugInteractionTableModel::LevelCode:
        case DrugDrugInteractionTableModel::LevelName:
        case DrugDrugInteractionTableModel::LevelComboIndex: sql = Constants::DDI_LEVELCODE; break;
        case DrugDrugInteractionTableModel::DateCreation: sql = Constants::DDI_DATECREATE; break;
        case DrugDrugInteractionTableModel::DateLastUpdate: sql = Constants::DDI_DATEUPDATE; break;
        case DrugDrugInteractionTableModel::RiskFr: sql = Constants::DDI_RISKFR; break;
        case DrugDrugInteractionTableModel::RiskEn: sql = Constants::DDI_RISKEN; break;
        case DrugDrugInteractionTableModel::ManagementFr: sql = Constants::DDI_MANAGEMENTFR; break;
        case DrugDrugInteractionTableModel::ManagementEn: sql = Constants::DDI_MANAGEMENTEN; break;
        case DrugDrugInteractionTableModel::ReviewersStringList: sql = Constants::DDI_REVIEWERSSTRINGLIST; break;
        case DrugDrugInteractionTableModel::Source: sql = Constants::DDI_SOURCE; break;
        case DrugDrugInteractionTableModel::Comment: sql = Constants::DDI_COMMENT; break;
        case DrugDrugInteractionTableModel::FirstInteractorRouteOfAdministrationIds: sql = Constants::DDI_FIRSTINTERACTORROUTEOFADMINISTRATIONIDS; break;
        case DrugDrugInteractionTableModel::SecondInteractorRouteOfAdministrationIds: sql = Constants::DDI_SECONDINTERACTORROUTEOFADMINISTRATIONIDS; break;
        case DrugDrugInteractionTableModel::FirstDoseUseFrom: sql = Constants::DDI_FIRSTDOSEUSEFROM; break;
        case DrugDrugInteractionTableModel::FirstDoseUsesTo: sql = Constants::DDI_FIRSTDOSEUSESTO; break;
        case DrugDrugInteractionTableModel::FirstDoseFromValue: sql = Constants::DDI_FIRSTDOSEFROMVALUE; break;
        case DrugDrugInteractionTableModel::FirstDoseFromUnitsId: sql = Constants::DDI_FIRSTDOSEFROMUNITS; break;
        case DrugDrugInteractionTableModel::FirstDoseFromRepartitionId: sql = Constants::DDI_FIRSTDOSEFROMREPARTITION; break;
        case DrugDrugInteractionTableModel::FirstDoseToValue: sql = Constants::DDI_FIRSTDOSETOVALUE; break;
        case DrugDrugInteractionTableModel::FirstDoseToUnitsId: sql = Constants::DDI_FIRSTDOSETOUNITS; break;
        case DrugDrugInteractionTableModel::FirstDoseToRepartitionId: sql = Constants::DDI_FIRSTDOSETOREPARTITION; break;
        case DrugDrugInteractionTableModel::SecondDoseUseFrom: sql = Constants::DDI_SECONDDOSEUSEFROM; break;
        case DrugDrugInteractionTableModel::SecondDoseUsesTo: sql = Constants::DDI_SECONDDOSEUSESTO; break;
        case DrugDrugInteractionTableModel::SecondDoseFromValue: sql = Constants::DDI_SECONDDOSEFROMVALUE; break;
        case DrugDrugInteractionTableModel::SecondDoseFromUnitsId: sql = Constants::DDI_SECONDDOSEFROMUNITS; break;
        case DrugDrugInteractionTableModel::SecondDoseFromRepartitionId: sql = Constants::DDI_SECONDDOSEFROMREPARTITION; break;
        case DrugDrugInteractionTableModel::SecondDoseToValue: sql = Constants::DDI_SECONDDOSETOVALUE; break;
        case DrugDrugInteractionTableModel::SecondDoseToUnitsId: sql = Constants::DDI_SECONDDOSETOUNITS; break;
        case DrugDrugInteractionTableModel::SecondDoseToRepartitionId: sql = Constants::DDI_SECONDDOSETOREPARTITION; break;
        case DrugDrugInteractionTableModel::PMIDStringList: sql = Constants::DDI_PMIDSTRINGLIST; break;
        };
        return sql;
    }

    int numberOfLevels(const QString &levelCode)
    {
        int n = 0;
        foreach(const QString &level, _levelsToComboIndex.uniqueKeys())
            if (levelCode.contains(level))
                ++n;
        return n;
    }

    QString firstLevelCode(const QString &levelCode)
    {
        // Assuming that levels are one char excepts cytochrome p450 DDI,
        // We just have to return the first char of the \e levelCode or "450"
        if (levelCode.isEmpty())
            return QString();

        if (levelCode.at(0).isDigit())
            return "450";

        return levelCode.at(0);
    }

    QString levelName(QString levelCode)
    {
        return DrugDrugInteraction::levelName(levelCode);
    }

    // Check DDI sanity and store error messages into _rowErrors
    void checkInteractionErrors(int row)
    {
        DrugDrugInteractionTableModel::DrugDrugInteractionErrors errors;
        if (q->index(row, DrugDrugInteractionTableModel::RiskEn).data().toString().isEmpty()
                && q->index(row, DrugDrugInteractionTableModel::RiskFr).data().toString().isEmpty())
            errors |= DrugDrugInteractionTableModel::NoRiskDefined;
        if (!q->data(q->index(row, DrugDrugInteractionTableModel::IsReviewed)).toBool())
            errors |= DrugDrugInteractionTableModel::NotReviewed;
        if (q->isMultiLevel(row))
            errors |= DrugDrugInteractionTableModel::IsMultilevel;
        if (q->isUntranslated(row))
            errors |= DrugDrugInteractionTableModel::TranslationMissing;
        if (q->data(q->index(row, DrugDrugInteractionTableModel::PMIDStringList)).toStringList().isEmpty())
            errors |= DrugDrugInteractionTableModel::NoBibliographicReferences;
        if (!ddiCore()->drugInteractorTableModel()->interactorUidExists(q->data(q->index(row, DrugDrugInteractionTableModel::FirstInteractorUid)).toString()))
            errors |= DrugDrugInteractionTableModel::FirstInteractorDoesNotExists;
        if (!ddiCore()->drugInteractorTableModel()->interactorUidExists(q->data(q->index(row, DrugDrugInteractionTableModel::SecondInteractorUid)).toString()))
            errors |= DrugDrugInteractionTableModel::SecondInteractorDoesNotExists;
        QPersistentModelIndex pi = q->index(row, 0);
        _rowErrors.insert(pi, errors);
    }

public:
    QSqlTableModel *_sql;
    QHash<QString, int> _levelsToComboIndex;
    QHash<QPersistentModelIndex, DrugDrugInteractionTableModel::DrugDrugInteractionErrors> _rowErrors; // index column==0

private:
    DrugDrugInteractionTableModel *q;
};
}  // End namespace Internal
}  // End namespace DDI

/**
 * Creates a Drug-drug interaction table model database wrapper.
 * The model is empty, you can populate it using intialize()
 */
DrugDrugInteractionTableModel::DrugDrugInteractionTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::DrugDrugInteractionTableModelPrivate(this))
{
    setObjectName("DrugDrugInteractionTableModel");
    d->createSqlModel();
}

DrugDrugInteractionTableModel::~DrugDrugInteractionTableModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Initialize the model (fetch all interactors from database). */
bool DrugDrugInteractionTableModel::initialize()
{
    d->_rowErrors.clear();
    d->_sql->select();

    // Fetch all data in the source model
    while (d->_sql->canFetchMore(index(d->_sql->rowCount(), 0)))
        d->_sql->fetchMore(index(d->_sql->rowCount(), 0));

    return true;
}

bool DrugDrugInteractionTableModel::onDdiDatabaseChanged()
{
    delete d->_sql;
    d->_sql = 0;
    d->createSqlModel();
    return initialize();
}

/**
 * Check all DDI errors, this can be a long processing. Model is not resetted but
 * item colors can change. Use QAbstractItemView::update() to force view update.
*/
bool DrugDrugInteractionTableModel::checkInteractionErrors()
{
    for(int i=0; i < rowCount(); ++i)
        d->checkInteractionErrors(i);
    return true;
}

int DrugDrugInteractionTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int DrugDrugInteractionTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void DrugDrugInteractionTableModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool DrugDrugInteractionTableModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

QVariant DrugDrugInteractionTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), d->modelColumnToSqlColumn(index.column()));
        switch (index.column()) {
        case FirstInteractorLabel:
        case SecondInteractorLabel:
        {
            // Get the uid
            const QString &uid = sqlIndex.data().toString();
            // Return the label of the interactor (not the uid)
            return ddiCore()->database().interactorLabel(uid);
        }
        case LevelName: return d->levelName(d->_sql->data(sqlIndex).toString());
        case LevelComboIndex: return d->_levelsToComboIndex.value(d->_sql->data(sqlIndex).toString(), -1);
        case PMIDStringList: return d->_sql->data(sqlIndex, role).toString().split(";");
        // Dose comboIndex
        case FirstDoseFromUnitsId:
        case FirstDoseToUnitsId:
        case SecondDoseFromUnitsId:
        case SecondDoseToUnitsId:
            return units().indexOf(d->_sql->data(sqlIndex, role).toString());
        case FirstDoseFromRepartitionId:
        case FirstDoseToRepartitionId:
        case SecondDoseFromRepartitionId:
        case SecondDoseToRepartitionId:
            return repartitions().indexOf(d->_sql->data(sqlIndex, role).toString());
        default: return d->_sql->data(sqlIndex, role);
        }
    } else if (role==Qt::ForegroundRole) {
        QModelIndex error = this->index(index.row(), 0);
        if (!d->_rowErrors.contains(error))
            return QVariant();
        if (!d->_rowErrors.value(error).testFlag(NoError))
            return QColor(20,250,20).dark();
    } else if (role == Qt::CheckStateRole) {
        int sql = -1;
        switch (index.column()) {
        case IsValid: sql = Constants::DDI_ISVALID; break;
        case IsReviewed: sql = Constants::DDI_ISREVIEWED; break;
        case FirstDoseUseFrom: sql = Constants::DDI_FIRSTDOSEUSEFROM; break;
        case FirstDoseUsesTo: sql = Constants::DDI_FIRSTDOSEUSESTO; break;
        case SecondDoseUseFrom: sql = Constants::DDI_SECONDDOSEUSEFROM; break;
        case SecondDoseUsesTo: sql = Constants::DDI_SECONDDOSEUSESTO; break;
        default: sql = -1; break;
        }
        if (sql != -1) {
            QModelIndex sqlIndex = d->_sql->index(index.row(), sql);
            // using displayrole
            return d->_sql->data(sqlIndex).toBool()?Qt::Checked:Qt::Unchecked;
        }
    }
    return QVariant();
}

bool DrugDrugInteractionTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    // Can not setData for
    switch (index.column()) {
    case DrugDrugInteractionTableModel::FirstInteractorLabel:
    case DrugDrugInteractionTableModel::SecondInteractorLabel:
        return false;
    }

    int sql = d->modelColumnToSqlColumn(index.column());
    if (role == Qt::EditRole) {
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        switch (index.column()) {
        case IsValid:
        case IsReviewed:
        case FirstDoseUseFrom:
        case FirstDoseUsesTo:
        case SecondDoseUseFrom:
        case SecondDoseUsesTo:
            ok = d->_sql->setData(sqlIndex, value.toBool()?1:0, role);
            break;
        case LevelComboIndex:
            ok = d->_sql->setData(d->_sql->index(index.row(), Constants::DDI_LEVELCODE),
                                  d->_levelsToComboIndex.key(value.toInt()), role);
            break;
        case PMIDStringList:
            ok = d->_sql->setData(sqlIndex, value.toStringList().join(";"), role);
            break;

        // Dose comboIndex
        case FirstDoseFromUnitsId:
        case FirstDoseToUnitsId:
        case SecondDoseFromUnitsId:
        case SecondDoseToUnitsId:
            ok = d->_sql->setData(sqlIndex, unit(value.toInt()), role);
            break;
        case FirstDoseFromRepartitionId:
        case FirstDoseToRepartitionId:
        case SecondDoseFromRepartitionId:
        case SecondDoseToRepartitionId:
            ok = d->_sql->setData(sqlIndex, repartition(value.toInt()), role);
            break;
        default: ok = d->_sql->setData(sqlIndex, value, role); break;
        }

        // set the date update
        if (ok) {
            Q_EMIT dataChanged(index, index);
            sqlIndex = d->_sql->index(index.row(), Constants::DDI_DATEUPDATE);
            ok = d->_sql->setData(sqlIndex, QDateTime::currentDateTime(), role);
            if (ok) {
                QModelIndex idx = this->index(index.row(), DateLastUpdate);
                Q_EMIT dataChanged(idx, idx);
            }
        }

        return ok;
    }

    // TODO: manage CheckStateRole
    return false;
}

bool DrugDrugInteractionTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool ok = d->_sql->insertRows(row, count, parent);
    if (!ok)
        LOG_ERROR(d->_sql->lastError().text());
    return ok;
}

bool DrugDrugInteractionTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    for(int i = 0; i < count; ++i)
        d->_sql->setData(d->_sql->index(row+i, IsValid), 0);
    endRemoveRows();
    return true;
}

//QVariant DrugDrugInteractionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//        switch (section) {
//        case Id: return tr("Id");
//        case Uuid: return tr("Uuid");
//        case IsValid: return tr("Is valid");
//        case IsInteractingClass: return tr("Is interaction class");
//        case IsReviewed: return tr("Is reviewed");
//        case IsAutoFound: return tr("Is auto-found");
//        case DoNotWarnDuplicated: return tr("DoNotWarnDuplicated");
//        case TranslatedLabel: return tr("Translated label");
//        case EnLabel: return tr("English label");
//        case FrLabel: return tr("French label");
//        case DeLabel: return tr("Deustch label");
//        case ClassInformationFr: return tr("Class information (french)");
//        case ClassInformationEn: return tr("Class information (english)");
//        case ClassInformationDe: return tr("Class information (deustch)");
//        case ATCCodeStringList: return tr("ATC codes");
//        case DateOfCreation: return tr("Date of creation");
//        case DateLastUpdate: return tr("Date of update");
//        case DateReview: return tr("Date of review");
//        case PMIDStringList: return tr("PMID");
//        case ChildrenUuid: return tr("Children");
//        case Reference: return tr("Reference");
//        case Comment: return tr("Comment");
//        }
//    }
//    return QVariant();
//}

Qt::ItemFlags DrugDrugInteractionTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    switch (index.column()) {
    case IsValid:
    case IsReviewed:
    case FirstDoseUseFrom:
    case FirstDoseUsesTo:
    case SecondDoseUseFrom:
    case SecondDoseUsesTo:
        f |= Qt::ItemIsUserCheckable;
        break;
    }
    return f;
}

/** React at primeInsert() signal, populate the newly created row with the default values */
void DrugDrugInteractionTableModel::populateNewRowWithDefault(int row, QSqlRecord &record)
{
    Q_UNUSED(row);
    record.clearValues();
    for(int i = 0; i < d->_sql->columnCount(); ++i) {
        record.setGenerated(i, true);
        record.setValue(i, QVariant());
    }
    // We need to force the DDI_ID in the record (we can not let the db chose the ID value)
    record.setValue(Constants::DDI_ID, ddiBase().max(Constants::Table_DDI, Constants::DDI_ID).toInt() + 1);
    record.setValue(Constants::DDI_UID, Utils::createUid());
    record.setValue(Constants::DDI_ISREVIEWED, 0);
    record.setValue(Constants::DDI_ISVALID, 1);
    record.setValue(Constants::DDI_LEVELCODE, "P");
    record.setValue(Constants::DDI_DATECREATE, QDate::currentDate());
    record.setValue(Constants::DDI_DATEUPDATE, QDate::currentDate());
    record.setValue(Constants::DDI_SOURCE, "FreeMedForms");
    record.setValue(Constants::DDI_FIRSTDOSEUSEFROM, 0);
    record.setValue(Constants::DDI_FIRSTDOSEUSESTO, 0);
    record.setValue(Constants::DDI_SECONDDOSEUSEFROM, 0);
    record.setValue(Constants::DDI_SECONDDOSEUSESTO, 0);
}

QStringList DrugDrugInteractionTableModel::units()
{
    QStringList l;
    l << "µg";
    l << "mg";
    l << "g";
    l << "UI";
    l << "l";
    l << "dl";
    l << "cl";
    l << "ml";
    l << "µl";
    return l;
}

QStringList DrugDrugInteractionTableModel::repartitions()
{
    QStringList l;
    l << tkTr(Trans::Constants::MINUTES);
    l << tkTr(Trans::Constants::HOURS);
    l << tkTr(Trans::Constants::DAYS);
    l << tkTr(Trans::Constants::WEEKS);
    l << tkTr(Trans::Constants::MONTHS);
    l << tkTr(Trans::Constants::YEARS);
    l << tkTr(Trans::Constants::DECADES);
    return l;
}

QString DrugDrugInteractionTableModel::unit(int index)
{
    const QStringList &l = units();
    if (IN_RANGE_STRICT_MAX(index, 0, l.count()))
        return l.at(index);
    return QString();
}

QString DrugDrugInteractionTableModel::repartition(int index)
{
    const QStringList &l = repartitions();
    if (IN_RANGE_STRICT_MAX(index, 0, l.count()))
        return l.at(index);
    return QString();
}

bool DrugDrugInteractionTableModel::hasError(int row, DrugDrugInteractionError error)
{
    return d->_rowErrors.value(index(row, 0)).testFlag(error);
}

/**
 * Returns true if the DDI at row \e row is a multi-level DDI. You can then split this DDI
 * into severals ones (with one unique level) using splitMultiLevel().
 */
bool DrugDrugInteractionTableModel::isMultiLevel(int row) const
{
    QModelIndex index = this->index(row, LevelCode);
    return (d->numberOfLevels(index.data().toString()) > 1);
}

/** Splits a single multi-level DDI into multiple DDI with a single level */
bool DrugDrugInteractionTableModel::splitMultiLevel(int row)
{
    if (!isMultiLevel(row))
        return false;

    while (isMultiLevel(row)) {
        // Compute the LevelCode of both DDI
        QModelIndex fromLevelCode = index(row, LevelCode);
        QString fromLevel = fromLevelCode.data().toString();
        QString newDDILevel = d->firstLevelCode(fromLevel);
        setData(fromLevelCode, fromLevel.remove(0, newDDILevel.size()));

        // Create a QSqlQuery with an insert method
        QSqlQuery query(ddiBase().database());
        QString req = ddiBase().prepareInsertQuery(Constants::Table_DDI);
        query.prepare(req);
        for(int i=0; i < Constants::DDI_MaxParam; ++i) {
            QModelIndex fromIndex = index(row, i);
            query.bindValue(i, d->_sql->data(fromIndex));
        }
        query.bindValue(Constants::DDI_ID, QVariant());
        query.bindValue(Constants::DDI_UID, Utils::createUid());
        query.bindValue(Constants::DDI_LEVELCODE, newDDILevel);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            ddiBase().database().rollback();
            return false;
        }
    }

    // Submit the new DDI level
    if (!d->_sql->submitAll()) {
        LOG_ERROR("Unable to submit DDI");
        return false;
    }

    // Reset the model
    initialize();

    return true;
}

/** Returns true is the DDI miss at least one translation */
bool DrugDrugInteractionTableModel::isUntranslated(int row) const
{
    QModelIndex riskFr = index(row, RiskFr);
    QModelIndex riskEn = index(row, RiskEn);
    QModelIndex managementFr = index(row, ManagementFr);
    QModelIndex managementEn = index(row, ManagementEn);
    return (riskFr.data().toString().isEmpty() != riskEn.data().toString().isEmpty() ||
            managementFr.data().toString().isEmpty() != managementEn.data().toString().isEmpty());
}

/** */
QString DrugDrugInteractionTableModel::humanReadableDrugDrugInteractionOverView(int row) const
{
    QString tmp;

    // Check Interaction sanity
    d->checkInteractionErrors(row);
    DrugDrugInteractionErrors errorflag = d->_rowErrors.value(index(row, 0));
    QStringList errors;
    if (!errorflag.testFlag(NoError)) {
        if (errorflag.testFlag(NoRiskDefined))
            errors << tr("No risk defined");
        if (errorflag.testFlag(NotReviewed))
            errors << tr("Not reviewed");
        if (errorflag.testFlag(IsMultilevel))
            errors << tr("Is multi-level");
        if (errorflag.testFlag(TranslationMissing))
            errors << tr("Translation missing");
        if (errorflag.testFlag(NoBibliographicReferences))
            errors << tr("No biliographic references");
        if (errorflag.testFlag(FirstInteractorDoesNotExists))
            errors << tr("First interactor uid does not exists");
        if (errorflag.testFlag(SecondInteractorDoesNotExists))
            errors << tr("Second interactor uid does not exists");
    }
    bool hasError = !errors.isEmpty();
    QString indent = "&nbsp;&nbsp;&nbsp;&nbsp;";
    if (errors.isEmpty())
        errors << tr("Interaction looks sane");
    else
        indent.append("/!\\ ");

    // Get the risk and management text according to the current language
    int riskCol, managementCol;
    riskCol = RiskEn;
    managementCol = ManagementEn;
    if (QLocale().language() == QLocale::French) {
        riskCol = RiskFr;
        managementCol = ManagementFr;
    }
    tmp += QString("<b>%1 / %2</b><br />"
                   "<b>%3</b><br />"
                   "<span style='%4'>%5</span><br />"
                   "<u>%6:</u> %7<br />"
                   "<u>%8:</u> %9<br />")
            .arg(data(index(row, FirstInteractorLabel)).toString())
            .arg(data(index(row, SecondInteractorLabel)).toString())
            .arg(data(index(row, LevelName)).toString())
            .arg(hasError?"font-weight: 600; color: darkred;":"color: gray")
            .arg(indent + errors.join(QString("<br>%1").arg(indent)))
            .arg(tr("Risk"))
            .arg(data(index(row, riskCol)).toString())
            .arg(tr("Management"))
            .arg(data(index(row, managementCol)).toString());

    // TODO: dose related ?
    //    QString dose;
    //    if (ddi->firstInteractorDose().data(DrugDrugInteractionDose::UsesFrom).toBool()) {
    //        dose += QString("<br />* From: %1 %2 %3 <br />")
    //                .arg(ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromValue).toString())
    //                .arg(DrugDrugInteractionModel::unit(ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromUnits).toInt()))
    //                .arg(DrugDrugInteractionModel::repartition(ddi->firstInteractorDose().data(DrugDrugInteractionDose::FromRepartition).toInt()));
    //    }
    //    if (ddi->firstInteractorDose().data(DrugDrugInteractionDose::UsesTo).toBool()) {
    //        dose += QString("<br />* To: %1 %2 %3 <br />")
    //                .arg(ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToValue).toString())
    //                .arg(DrugDrugInteractionModel::unit(ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToUnits).toInt()))
    //                .arg(DrugDrugInteractionModel::repartition(ddi->firstInteractorDose().data(DrugDrugInteractionDose::ToRepartition).toInt()));
    //    }
    //    if (!dose.isEmpty()) {
    //        tmp += QString("<br /><span style=\"color:#666600\"><u>First Molecule Dose:</u> %1</span><br />").arg(dose);
    //        dose.clear();
    //    }
    //    if (ddi->secondInteractorDose().data(DrugDrugInteractionDose::UsesFrom).toBool()) {
    //        dose += QString("* From: %1 %2 %3 <br />")
    //                .arg(ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromValue).toString())
    //                .arg(DrugDrugInteractionModel::unit(ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromUnits).toInt()))
    //                .arg(DrugDrugInteractionModel::repartition(ddi->secondInteractorDose().data(DrugDrugInteractionDose::FromRepartition).toInt()));
    //    }
    //    if (ddi->secondInteractorDose().data(DrugDrugInteractionDose::UsesTo).toBool()) {
    //        dose += QString("* To: %1 %2 %3 <br />")
    //                .arg(ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToValue).toString())
    //                .arg(DrugDrugInteractionModel::unit(ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToUnits).toInt()))
    //                .arg(DrugDrugInteractionModel::repartition(ddi->secondInteractorDose().data(DrugDrugInteractionDose::ToRepartition).toInt()));
    //    }
    //    if (!dose.isEmpty()) {
    //        tmp += QString("<br /><span style=\"color:#666600\"><u>Second Molecule Dose:</u> %1</span><br />").arg(dose);
    //        dose.clear();
    //    }
    //        dose.clear();
    //        const QStringList &routes = ddi->data(DrugDrugInteractionModel::FirstInteractorRouteOfAdministrationIds).toStringList();
    //        if (routes.count() > 0) {
    //            if (!routes.at(0).isEmpty()) {
    //                dose += QString("* Routes: %1<br />")
    //                        .arg(routes.join(";"));
    //            }
    //        }
    //    dose.clear();

    // Append clickable bibliography
    const QStringList &pmids = data(index(row, PMIDStringList)).toStringList();
    QStringList pmidText;
    if (pmids.count() > 0) {
        if (!pmids.at(0).isEmpty()) {
            foreach(const QString &pmid, pmids)
                pmidText << QString("<a href=\"http://www.ncbi.nlm.nih.gov/pubmed/%1\">%1</a>").arg(pmid);
        }
    }
    if (!pmidText.isEmpty())
        tmp += QString("<span style=\"color:#003333\"><b>%1</b><ul><li>%2</li></ul></span><br />")
                .arg(tr("Bibliography:"))
                .arg(pmidText.join("</li><li>"));
    return tmp;

}

/** Returns the number of unreviewed DDI::DrugInteractor from the database. */
int DrugDrugInteractionTableModel::numberOfUnreviewed() const
{
    // directly ask the database instead of screening the model
    QHash<int, QString> where;
    where.insert(Constants::DDI_ISREVIEWED, "=0");
    return ddiBase().count(Constants::Table_DDI, Constants::DDI_ID, ddiBase().getWhereClause(Constants::Table_DDI, where));
}


/** Submit changes directly to the database */
bool DrugDrugInteractionTableModel::submitAll()
{
#if QT_VERSION >= 0x050000
    if (!d->_sql->isDirty()) {
        LOG_ERROR("Model is not dirty");
        return true;
    }
#endif

    bool ok = d->_sql->submitAll();
    if (!ok)
        LOG_ERROR(d->_sql->lastError().text());
    else
        d->_sql->database().commit();

    // As submitAll in QSqlTableModel repopulates the model, we need to re-initialize it
    initialize();

    return ok;
}

void DrugDrugInteractionTableModel::setSqlFilter(const QString &filter)
{
    d->_sql->setFilter(filter);
    d->_sql->select();
}

DrugDrugInteractionFilteredTableModel::DrugDrugInteractionFilteredTableModel(QObject *parent) :
    DrugDrugInteractionTableModel(parent)
{}

DrugDrugInteractionFilteredTableModel::~DrugDrugInteractionFilteredTableModel()
{}

bool DrugDrugInteractionFilteredTableModel::initialize()
{
    return DrugDrugInteractionTableModel::initialize();
}

void DrugDrugInteractionFilteredTableModel::filterLastUpdated(const QDate &since)
{
    QHash<int, QString> where;
    int table = Constants::Table_DDI;
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_ISREVIEWED, "=1");
    where.insert(Constants::DDI_DATEUPDATE, Constants::SQL_ISNOTNULL);
    where.insert(Constants::DDI_DATEUPDATE, QString("> '%1'").arg(since.toString(Qt::ISODate)));
    where.insert(Constants::DDI_DATECREATE, QString("< '%1'").arg(since.toString(Qt::ISODate)));
    setSqlFilter(ddiBase().getWhereClause(table, where));
}

void DrugDrugInteractionFilteredTableModel::filterNewItems(const QDate &since)
{
    QHash<int, QString> where;
    int table = Constants::Table_DDI;
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_ISREVIEWED, "=1");
    where.insert(Constants::DDI_DATECREATE, Constants::SQL_ISNOTNULL);
    where.insert(Constants::DDI_DATECREATE, QString("> '%1'").arg(since.toString(Qt::ISODate)));
    setSqlFilter(ddiBase().getWhereClause(table, where));
}

void DrugDrugInteractionFilteredTableModel::filterLastUpdatedAndNewItems(const QDate &since)
{
    Q_UNUSED(since);
    // TODO: code this
}

bool DrugDrugInteractionFilteredTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}

bool DrugDrugInteractionFilteredTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

bool DrugDrugInteractionFilteredTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

bool DrugDrugInteractionFilteredTableModel::submit()
{
    return true;
}

bool DrugDrugInteractionFilteredTableModel::submitAll()
{
    return true;
}
