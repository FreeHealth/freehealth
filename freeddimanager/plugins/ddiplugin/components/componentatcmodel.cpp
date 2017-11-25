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
 * \class DDI::ComponentAtcModel
 * This model holds information about drug's component and
 * their link to any ATC code(s). \n
 * This model is not in manual submit but this should be configurable.
 * To save data to the database, call submitAll().
*/

#include "componentatcmodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/atc/atctablemodel.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/components/componentlinkerdata.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QColor>
#include <QIcon>
#include <QSqlTableModel>
#include <QHash>

using namespace DDI;
using namespace Internal;

// TODO: add a report including
// - linked ATC does not exist in the database
// - "association" in the ATC label

static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
static inline DDI::Internal::DDIDatabase &ddiBase()  { return DDI::DDICore::instance()->database(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

namespace DDI {
namespace Internal {

class PrivateResult : public ComponentLinkerResult
{
public:
    PrivateResult()
    {}

    ~PrivateResult()
    {}

    // Private interface
    /**
     * Define the component name list that can not be linked to any drug interactor / ATC code. \n
     * This data is defined by the DDI::ComponentModel::componentLinker() procedure.
     */
    void setUnfoundComponents(const QStringList &unfound) {unfoundComponentsAssociation = unfound;}

    /**
     * Define the component name list that can not be linked to any drug interactor / ATC code. \n
     * This data is defined by the DDI::ComponentModel::componentLinker() procedure.
     */
    void addUnfoundComponent(const QString &unfound)
    {
        if (!unfoundComponentsAssociation.contains(unfound))
            unfoundComponentsAssociation << unfound;
    }

    /** Add an error message */
    void addErrorMessage(const QString &s) {_errors << s;}

    /** Add a message */
    void addMessage(const QString &s) {_msg << s;}

    /** Add a link between component and interactor */
    void addComponentToAtcLink(int componentId, int atcId, const ComponentLinkerData &data)
    {
        if (compoIdToAtcId.values(componentId).contains(atcId))
            return;

        compoIdToAtcId.insertMulti(componentId, atcId);
        QList<int> eq = data.equivalences(componentId);
        for(int i = 0; i < eq.count(); ++i) {
            addComponentToAtcLink(eq.at(i), atcId, data);
        }
    }

    /** Add a link between component and interactor */
    void addComponentWithoutAtcLink(int componentId, const ComponentLinkerData &data)
    {
        addComponentToAtcLink(componentId, -1, data);
    }

    void setCompletion(double completion)
    {
        _completionPercentage = completion;
    }
};


class ComponentAtcModelPrivate
{
public:
    ComponentAtcModelPrivate(ComponentAtcModel *parent) :
        _sql(0),
        _rows(0),
        q(parent)
    {
    }

    ~ComponentAtcModelPrivate()
    {}

    void createSqlModel()
    {
        _sql = new QSqlTableModel(q, ddiBase().database());
        _sql->setTable(ddiBase().table(Constants::Table_COMPONENTS));
        // _sql->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // QObject::connect(_sql, SIGNAL(primeInsert(int,QSqlRecord&)), q, SLOT(populateNewRowWithDefault(int, QSqlRecord&)));
        QObject::connect(_sql, SIGNAL(layoutAboutToBeChanged()), q, SIGNAL(layoutAboutToBeChanged()));
        QObject::connect(_sql, SIGNAL(layoutChanged()), q, SIGNAL(layoutChanged()));
        QObject::connect(_sql, SIGNAL(modelAboutToBeReset()), q, SIGNAL(modelAboutToBeReset()));
        QObject::connect(_sql, SIGNAL(modelReset()), q, SIGNAL(modelReset()));
    }

    int modelColumnToSqlColumn(const int modelColumn)
    {
        int sql = -1;
        switch (modelColumn) {
        case ComponentAtcModel::Id: sql = Constants::COMPO_ID; break;
        case ComponentAtcModel::Uid: sql = Constants::COMPO_UID; break;
        case ComponentAtcModel::DrugDatabaseComponentUid1: sql = Constants::COMPO_DRUGDB_UID1; break;
        case ComponentAtcModel::DrugDatabaseComponentUid2: sql = Constants::COMPO_DRUGDB_UID2; break;
        case ComponentAtcModel::IsValid: sql = Constants::COMPO_ISVALID; break;
        case ComponentAtcModel::IsReviewed: sql = Constants::COMPO_ISREVIEWED; break;
        case ComponentAtcModel::Name: sql = Constants::COMPO_LABEL; break;
        case ComponentAtcModel::AtcCodeList: sql = Constants::COMPO_ATCCODES; break;
        case ComponentAtcModel::SuggestedAtcCodeList: sql = Constants::COMPO_SUGGESTED; break;
        case ComponentAtcModel::Reviewer: sql = Constants::COMPO_REVIEWERS; break;
        case ComponentAtcModel::DateCreation: sql = Constants::COMPO_DATECREATE; break;
        case ComponentAtcModel::DateUpdate: sql = Constants::COMPO_DATEUPDATE; break;
        case ComponentAtcModel::Comments: sql = Constants::COMPO_COMMENT; break;
        };
        return sql;
    }

    // Returns all available ATC codes and names from the DDI::DDIDatabase (not the drugs database)
    // All codes and names are uppercase and without accents
    QHash<QString, QString> getAllAtcCodeAndName(const QString &lang)
    {
        QHash<QString, QString> atcCodeToName;
        LOG_FOR(q, "Getting ATC Information from the interactions database");
        QSqlDatabase db = ddiBase().database();
        if (!db.isOpen()) {
            return atcCodeToName;
        }
        QList<int> fields;
        fields << Constants::ATC_CODE;
        if (lang.compare("fr", Qt::CaseInsensitive) == 0)
            fields << Constants::ATC_FR;
        else if (lang.compare("en", Qt::CaseInsensitive) == 0)
            fields << Constants::ATC_EN;
        else if (lang.compare("de", Qt::CaseInsensitive) == 0)
            fields << Constants::ATC_DE;
        else if (lang.compare("sp", Qt::CaseInsensitive) == 0)
            fields << Constants::ATC_SP;
        else
            fields << Constants::ATC_FR;

        db.transaction();
        QString req = ddiBase().select(Constants::Table_ATC, fields);
        QSqlQuery query(db);
        if (query.exec(req)) {
            while (query.next())
                atcCodeToName.insert(query.value(0).toString().toUpper(),
                                     Utils::removeAccents(query.value(1).toString().toUpper()));
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            db.rollback();
            return atcCodeToName;
        }
        query.finish();
        db.commit();
        return atcCodeToName;
    }

    // Add ATC suggestion to the components
    // QHash key is the exact component label (with accents), ATC suggested represents
    // the list of ATC code coma separated
    // You must surround any call to this member with beginResetModel() and endResetModel()
    bool saveComponentsAndSuggestions(const QHash<QString, QString> &compoLblToAtcSuggested,
                                      const QStringList &knownComponents)
    {
        QSqlDatabase db = ddiBase().database();
        if (!db.isOpen()) {
            return false;
        }
        db.transaction();
        QSqlQuery query(db);
        QHash<int, QString> where;
        QStringList alreadySuggested;
        LOG_FOR(q, QString("Saving %1 ATC link suggestions").arg(compoLblToAtcSuggested.count()));

        // Save all components that are not already in the database
        foreach(const QString &lbl, knownComponents) {
            QString sqlLbl = lbl.toUpper();
            sqlLbl = sqlLbl.replace("''", "'");
            sqlLbl = sqlLbl.replace("'", "''");
            where.insert(Constants::COMPO_LABEL, QString("='%1'").arg(sqlLbl));
            if (ddiBase().count(Constants::Table_COMPONENTS,
                                Constants::COMPO_ID,
                                ddiBase().getWhereClause(Constants::Table_COMPONENTS, where)) == 0) {
                // Insert a new line in the database
                QString req = ddiBase().prepareInsertQuery(Constants::Table_COMPONENTS);
                query.prepare(req);
                query.bindValue(Constants::COMPO_ID, QVariant());
                query.bindValue(Constants::COMPO_UID, Constants::correctedUid(lbl));
                query.bindValue(Constants::COMPO_DRUGDB_UID1, _dbUid1);
                query.bindValue(Constants::COMPO_DRUGDB_UID2, _dbUid2);
                query.bindValue(Constants::COMPO_ISVALID, 1);
                query.bindValue(Constants::COMPO_ISREVIEWED, 0);
                query.bindValue(Constants::COMPO_LABEL, lbl);
                query.bindValue(Constants::COMPO_ATCCODES, QVariant());
                query.bindValue(Constants::COMPO_SUGGESTED, alreadySuggested.join(";"));
                query.bindValue(Constants::COMPO_DATECREATE, QDate::currentDate().toString(Qt::ISODate));
                query.bindValue(Constants::COMPO_DATEUPDATE, QDate::currentDate().toString(Qt::ISODate));
                query.bindValue(Constants::COMPO_REVIEWERS, QVariant());
                query.bindValue(Constants::COMPO_COMMENT, QVariant());

                // qWarning() << "inserted" << lbl << alreadySuggested.join(";");

                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR(q, query);
                    query.finish();
                    db.rollback();
                    return false;
                }
            }
        }
        db.commit();

        db.transaction();
        // Save suggested ATC codes
        // TODO: manage component equivalence +++

        foreach(const QString &lbl, compoLblToAtcSuggested.keys()) {
            alreadySuggested.clear();
            QString sqlLbl = lbl.toUpper();
            sqlLbl = sqlLbl.replace("''", "'");
            sqlLbl = sqlLbl.replace("'", "''");
            where.insert(Constants::COMPO_LABEL, QString("='%1'").arg(sqlLbl));

            // Get the suggested ATC code list for the component
            QString req = ddiBase().select(Constants::Table_COMPONENTS, Constants::COMPO_SUGGESTED, where);
            if (query.exec(req)) {
                if (query.next())
                    alreadySuggested = query.value(0).toString().split(";", QString::SkipEmptyParts);
            } else {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                db.rollback();
                return false;
            }
            query.finish();

            // Add new ATC, remove duples and save to database
            alreadySuggested << compoLblToAtcSuggested.value(lbl).split(";", QString::SkipEmptyParts);
            alreadySuggested.removeDuplicates();

            // Update the line
            req = ddiBase().prepareUpdateQuery(Constants::Table_COMPONENTS, QList<int>()
                                               << Constants::COMPO_SUGGESTED
                                               << Constants::COMPO_DATEUPDATE, where);
            query.prepare(req);
            query.bindValue(0, alreadySuggested.join(";"));
            query.bindValue(1, QDate::currentDate().toString(Qt::ISODate));

            // qWarning() << "updated" << lbl << alreadySuggested.join(";");

            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                query.finish();
                db.rollback();
                return false;
            }
            query.finish();
        }
        query.finish();
        db.commit();
        return true;
    }

public:
    QSqlTableModel *_sql;
    QString _reviewer, _dbUid1, _dbUid2;
    int _rows;
    QString _drugsDbFilter;

private:
    ComponentAtcModel *q;
};

}  // namespace Internal
}  // namespace DDI

ComponentAtcModel::ComponentAtcModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::ComponentAtcModelPrivate(this))
{
    setObjectName("ComponentAtcModel");
    d->createSqlModel();
}

ComponentAtcModel::~ComponentAtcModel()
{
    if (d)
        delete d;
    d = 0;
}

/**
 * The model must be initialized before any usage in views.
 * Returns \e true if initialization succeeded, otherwise \e false.
 */
bool ComponentAtcModel::initialize()
{
    // Fetch all row from the sql model
    d->_sql->select();
    while (d->_sql->canFetchMore(index(d->_sql->rowCount(), 0)))
        d->_sql->fetchMore(index(d->_sql->rowCount(), 0));
    return true;
}

/** If the Interaction database changed, update the content of the model */
bool ComponentAtcModel::onDdiDatabaseChanged()
{
    delete d->_sql;
    d->_sql = 0;
    d->createSqlModel();
    return initialize();
}

/** Return all available Drug database Uid from the database */
QStringList ComponentAtcModel::availableDrugsDatabases() const
{
    return ddiBase().availableComponentDrugsDatabaseUids();
}

/** Filter data according to the drug database uid \e dbUid1 and \e dbUid2 */
bool ComponentAtcModel::selectDatabase(const QString &dbUid1, const QString &dbUid2)
{
//    if (dbUid == d->_drugsDbFilter)
//        return true;
//    qWarning() << "ComponentAtcModel::selectDatabase" << dbUid;
    beginResetModel();
    d->_dbUid1 = dbUid1;
    d->_dbUid2 = dbUid2;
    QHash<int, QString> where;
    where.insert(Constants::COMPO_DRUGDB_UID1, QString("='%1'").arg(dbUid1));
    if (!dbUid2.isEmpty())
        where.insert(Constants::COMPO_DRUGDB_UID2, QString("='%1'").arg(dbUid2));
    QString filter = ddiBase().getWhereClause(Constants::Table_COMPONENTS, where);
    d->_sql->setFilter(filter);
    initialize();
    endResetModel();
    return true;
}

/** Return all available (registered) drug component database uid */
QStringList ComponentAtcModel::databaseUids() const
{
    QStringList uids;
    uids <<  d->_dbUid1;
    if (!d->_dbUid2.isEmpty())
        uids << d->_dbUid2;
    return uids;
}

/** Define the reviewer \e name to use */
void ComponentAtcModel::setActualReviewer(const QString &name)
{
    d->_reviewer = name;
}

int ComponentAtcModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int ComponentAtcModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void ComponentAtcModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool ComponentAtcModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

void ComponentAtcModel::fetchAll()
{
    while (d->_sql->canFetchMore(d->_sql->index(0,0)))
        d->_sql->fetchMore(d->_sql->index(0,0));
}

QVariant ComponentAtcModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), d->modelColumnToSqlColumn(index.column()));
        switch (index.column()) {
        case IsReviewed: return d->_sql->data(sqlIndex).toInt()==1?"Reviewed":"Unreviewed";
        case IsValid: return d->_sql->data(sqlIndex).toInt()==1?"Valid":"Invalid";
        default: break;
        }
        return d->_sql->data(sqlIndex, role);
    } else if (role == Qt::EditRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), d->modelColumnToSqlColumn(index.column()));
        return d->_sql->data(sqlIndex, role);
    } else if (role == Qt::CheckStateRole) {
        int sql = -1;
        switch (index.column()) {
        case IsValid: sql = Constants::COMPO_ISVALID; break;
        case IsReviewed: sql = Constants::COMPO_ISREVIEWED; break;
        default: sql = -1; break;
        }
        if (sql != -1) {
            QModelIndex sqlIndex = d->_sql->index(index.row(), sql);
            // using displayrole
            return d->_sql->data(sqlIndex).toBool()?Qt::Checked:Qt::Unchecked;
        }
    } else if (role == Qt::ToolTipRole) {
        QStringList atc = d->_sql->index(index.row(), Constants::COMPO_ATCCODES).data().toString().split(";", QString::SkipEmptyParts);
        QStringList atcFull;
        if (!atc.isEmpty()) {
            foreach(const QString &c, atc)
                atcFull << QString("%1 - %2").arg(c).arg(ddiBase().atcLabelForCode(c));
        }
        atc = d->_sql->index(index.row(), Constants::COMPO_SUGGESTED).data().toString().split(";", QString::SkipEmptyParts);
        QStringList suggestedFull;
        if (!atc.isEmpty()) {
            foreach(const QString &c, atc)
                suggestedFull << QString ("%1 - %2").arg(c).arg(ddiBase().atcLabelForCode(c));
        }

        QStringList lines;
        lines << QString("<b>%1</b><br>    %2")
                 .arg(d->_sql->index(index.row(), Constants::COMPO_LABEL).data().toString())
                 .arg(d->_sql->index(index.row(), Constants::COMPO_ISREVIEWED).data().toBool()?"Reviewed":"Unreviewed")
                 .replace(" ", "&nbsp;");
        if (!atcFull.isEmpty()) {
            lines << QString("<u>%1</u>").arg(tr("Linked ATC codes:"));
            lines << QString("&nbsp;&nbsp;%1").arg(atcFull.join("<br>&nbsp;&nbsp;"));
        }
        if (!suggestedFull.isEmpty()) {
            lines << QString("<u>%1</u>").arg(tr("Suggested ATC codes:"));
            lines << QString("&nbsp;&nbsp;%1").arg(suggestedFull.join("<br>&nbsp;&nbsp;"));
        }
        return lines.join("<br>");
    } else if (role == Qt::DecorationRole) {
        if (index.column() == FancyButton)
            return theme()->icon(Core::Constants::ICONHELP);
    }
    return QVariant();
}

bool ComponentAtcModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    int sql = d->modelColumnToSqlColumn(index.column());
    if (role == Qt::EditRole) {
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        if (sqlIndex.data() == value)
            return true;

        switch (index.column()) {
        case IsValid:
        case IsReviewed:
            // Switch to boolean
            ok = d->_sql->setData(sqlIndex, value.toBool()?1:0, role);
            break;
        case AtcCodeList:
        case SuggestedAtcCodeList:
            // Switch to coma separated list
            ok = d->_sql->setData(sqlIndex, value.toString().replace(" ", ";"), role);
            break;
        default:
            // Store as it is
            ok = d->_sql->setData(sqlIndex, value, role); break;
        }

        // set the date update
        if (ok) {
            Q_EMIT dataChanged(index, index);
            sqlIndex = d->_sql->index(index.row(), Constants::COMPO_DATEUPDATE);
            ok = d->_sql->setData(sqlIndex, QDateTime::currentDateTime().toString(Qt::ISODate), role);
            if (ok) {
                QModelIndex idx = this->index(index.row(), DateUpdate);
                Q_EMIT dataChanged(idx, idx);
            }
        }
        return ok;
    } else if (role == Qt::CheckStateRole) {
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        switch (index.column()) {
        case IsValid:
        case IsReviewed:
            ok = d->_sql->setData(sqlIndex, value.toInt()==Qt::Checked?1:0, Qt::EditRole);
            break;
        default: break;
        }

                // If is reviewed == Qt::Checked & no ATC & +suggested -> Set ATC with suggested
                if (index.column() == IsReviewed && value.toInt() == Qt::Checked) {
                    QModelIndex atcIndex = d->_sql->index(index.row(), Constants::COMPO_ATCCODES);
                    QString atc = d->_sql->data(atcIndex).toString().simplified();
                    if (atc.isEmpty()) {
                        QString sugg = d->_sql->data(d->_sql->index(index.row(), Constants::COMPO_SUGGESTED)).toString().simplified();
                        d->_sql->setData(atcIndex, sugg);
                        QModelIndex idx = this->index(index.row(), AtcCodeList);
                        Q_EMIT dataChanged(idx, idx);
                    }
                }

        // set the date update
        if (ok) {
            Q_EMIT dataChanged(index, index);
            sqlIndex = d->_sql->index(index.row(), Constants::COMPO_DATEUPDATE);
            ok = d->_sql->setData(sqlIndex, QDateTime::currentDateTime().toString(Qt::ISODate), role);
            if (ok) {
                QModelIndex idx = this->index(index.row(), DateUpdate);
                Q_EMIT dataChanged(idx, idx);
            }
        }
        return ok;
    }

    return false;
}

Qt::ItemFlags ComponentAtcModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    switch (index.column()) {
    case Id:
    case Uid:
    case DrugDatabaseComponentUid1:
    case DrugDatabaseComponentUid2:
    case Name:
    case DateUpdate:
    case DateCreation:
    case FancyButton:
        return f;
    case IsReviewed:
    case IsValid:
        f |= Qt::ItemIsUserCheckable;
        break;
    default:
        f |= Qt::ItemIsEditable;
    }

    return f;
}

QVariant ComponentAtcModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id:
            return "Id";
        case Uid:
            return "Uid";
        case DrugDatabaseComponentUid1:
            return tr("Drugs database Uid1");
        case DrugDatabaseComponentUid2:
            return tr("Drugs database Uid2");
        case Name:
            return tr("Component name");
        case AtcCodeList:
            return tr("ATC code");
        case SuggestedAtcCodeList:
            return tr("Suggested ATC code");
        case IsValid:
            return tr("Validity");
        case IsReviewed:
            return tr("Reviewed");
        case Reviewer:
            return tr("Reviewers");
        case Comments:
            return tr("Comments");
        case DateCreation:
            return tr("Date of creation");
        case DateUpdate:
            return tr("Date of update");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

/**
 * Returns a human readable overview of the model (number of components,
 * number of linked component, number non linked but suggested...)
 */
QString ComponentAtcModel::overview() const
{
    QStringList r;
    r << tr("Total: %1").arg(d->_sql->rowCount());
    QSqlDatabase db = ddiBase().database();
    if (!db.isOpen()) {
        LOG_ERROR("Can not connect to DDI::DDIDatabase");
        return QString::null;
    }

    QHash<int, QString> where;
    where.insert(Constants::COMPO_ISVALID, "=1");
    where.insert(Constants::COMPO_DRUGDB_UID1, QString("='%1'").arg(d->_dbUid1));
    if (!d->_dbUid2.isEmpty())
        where.insert(Constants::COMPO_DRUGDB_UID2, QString("='%1'").arg(d->_dbUid2));

    // With ATC codes
    where.insert(Constants::COMPO_ATCCODES, "<> ''");
    int count = ddiBase().count(Constants::Table_COMPONENTS,
                        Constants::COMPO_ID,
                        ddiBase().getWhereClause(Constants::Table_COMPONENTS, where));
    r << tr("+Atc: %1").arg(count);

    // Without ATC codes but with suggestion
    where.insert(Constants::COMPO_ATCCODES, "= ''");
    where.insert(Constants::COMPO_SUGGESTED, "<> ''");
    count = ddiBase().count(Constants::Table_COMPONENTS,
                        Constants::COMPO_ID,
                        ddiBase().getWhereClause(Constants::Table_COMPONENTS, where));
    r << tr("-Atc+Sugg: %1").arg(count);

    // Total reviewed
     where.insert(Constants::COMPO_ISREVIEWED, "=1");
     where.remove(Constants::COMPO_ATCCODES);
     where.remove(Constants::COMPO_SUGGESTED);
     count = ddiBase().count(Constants::Table_COMPONENTS,
                         Constants::COMPO_ID,
                         ddiBase().getWhereClause(Constants::Table_COMPONENTS, where));
     r << tr("Reviewed: %1").arg(count);
    return r.join(" ; ");
}

/**
 * Create the link between drugs components and drug interactors / ATC codes. \n
 * Use the DDI::ComponentLinkerData to give and take data.
 * \sa DDI::ComponentLinkerData
 */
ComponentLinkerResult &ComponentAtcModel::startComponentLinkage(const ComponentLinkerData &data)
{
    PrivateResult *result = new PrivateResult;
    QSqlDatabase db = ddiBase().database();
    if (!db.isOpen()) {
        result->addErrorMessage("Can not connect to DDI::DDIDatabase");
        LOG_ERROR("Can not connect to DDI::DDIDatabase");
        return *result;
    }

    // Get all ATC Code and Label
    QHash<QString, QString> atcCodeToName = d->getAllAtcCodeAndName(data.lang);
    qWarning() << "ATC" << atcCodeToName.count();

    const QStringList &knownComponentNames = data.compoIds.uniqueKeys();
    LOG(QString("Number of distinct molecules: %1").arg(knownComponentNames.count()));

    // Algorythm is be centered on the model data and automatic associations are only suggested
    // Create links using the model data (reviewed items)
    // For all items, try to find link -> then populate the model
    // This will allow to correct wrong automatic association

    // FIXME: all components must be managed uppercase

    // Ask the current model database for links (DDI::COMPO table)
    db.transaction();
    int z = 0;
    QSqlQuery query(db);
    QString req;
    QHash<int, QString> where;
    where.insert(Constants::COMPO_ISVALID, "=1");
    where.insert(Constants::COMPO_ISREVIEWED, "=1");
    where.insert(Constants::COMPO_DRUGDB_UID1, QString("='%1'").arg(d->_dbUid1));
    if (!d->_dbUid2.isEmpty())
        where.insert(Constants::COMPO_DRUGDB_UID2, QString("='%1'").arg(d->_dbUid2));

    foreach(const QString &componentLbl, knownComponentNames) {
        // Should we search for labels or Constants::correctedUid()?
        QString lbl = componentLbl.toUpper();
        QString sqlName = lbl;
        sqlName = sqlName.replace("''","'");
        sqlName = sqlName.replace("'", "''");
        where.insert(Constants::COMPO_LABEL, QString("='%1'").arg(sqlName));
        req = ddiBase().select(Constants::Table_COMPONENTS, QList<int>()
                               << Constants::COMPO_ATCCODES, where);
        if (query.exec(req)) {
            if (query.next()) {
                QString atcCodes = query.value(0).toString().simplified();
                // FIXME: One component can have multiple id?
                QList<int> componentIds = data.compoIds.values(componentLbl);
                if (!atcCodes.isEmpty()) {
                    foreach(const QString &code, atcCodes.split(";", QString::SkipEmptyParts)) {
                        int atcId = data.atcCodeIds.value(code);
                        for(int i =0; i < componentIds.count(); ++i)
                            result->addComponentToAtcLink(componentIds.at(i), atcId, data);
                    }
                } else {
                    for(int i = 0; i < componentIds.count(); ++i)
                        result->addComponentWithoutAtcLink(componentIds.at(i), data);
                }
                ++z;
            }
        } else {
            LOG_QUERY_ERROR(query);
            continue;
        }
        query.finish();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    qWarning() << "MODEL " << z;

    // Try to find component / ATC links & suggest to the user
    QHash<QString, QString> compoLblToAtcSuggested;

    // TODO: During this process, we should manage component equivalence (in comment? add links?)
    // First pass: For all components, try to find exact match component label <-> atc label


//    // Draft code: Automatically save ATC link to DB with isReviewed state
//    query.finish();
//    QStringList alreadySuggested;
//    where.remove(Constants::COMPO_ISVALID);
//    where.remove(Constants::COMPO_ISREVIEWED);
//    // End draft

    foreach(const QString &componentLbl, knownComponentNames) {
        if (componentLbl.isEmpty())
            continue;
        if (compoLblToAtcSuggested.contains(componentLbl))
            continue;
        // Always remove accents for ATC autochecking (ATC labels are retrieved without accents)
        // Does component name exact-matches an ATC label
        QString transformedLbl = Utils::removeAccents(componentLbl);
        QStringList atcCodes = atcCodeToName.keys(transformedLbl);

        // Now check if we find something
        if (!atcCodes.isEmpty()) {
            compoLblToAtcSuggested.insert(componentLbl, atcCodes.join(";"));

//            // Draft code: Automatically save ATC link to DB with isReviewed state
//            alreadySuggested.clear();
//            QString sqlLbl = componentLbl.toUpper();
//            sqlLbl = sqlLbl.replace("''", "'");
//            sqlLbl = sqlLbl.replace("'", "''");
//            where.insert(Constants::COMPO_LABEL, QString("='%1'").arg(sqlLbl));

//            // Get the suggested ATC code list for the component
//            QString req = ddiBase().select(Constants::Table_COMPONENTS, Constants::COMPO_SUGGESTED, where);
//            if (query.exec(req)) {
//                if (query.next())
//                    alreadySuggested = query.value(0).toString().split(";", QString::SkipEmptyParts);
//            } else {
//                LOG_QUERY_ERROR(query);
//                query.finish();
//                db.rollback();
//                return *result;
//            }
//            query.finish();

//            // Add new ATC, remove duples and save to database
//            alreadySuggested << atcCodes;
//            alreadySuggested.removeDuplicates();
//            // Remove all Z* codes
//            for(int i = alreadySuggested.count()-1; i > 0; --i) {
//                if (alreadySuggested.at(i).startsWith("Z"))
//                    alreadySuggested.removeAt(i);
//            }

//            // Update the line
//            req = ddiBase().prepareUpdateQuery(Constants::Table_COMPONENTS, QList<int>()
//                                               << Constants::COMPO_ISREVIEWED
//                                               << Constants::COMPO_ATCCODES
//                                               << Constants::COMPO_SUGGESTED
//                                               << Constants::COMPO_DATEUPDATE, where);
//            query.prepare(req);
//            query.bindValue(0, 1);
//            query.bindValue(1, alreadySuggested.join(";"));
//            query.bindValue(2, alreadySuggested.join(";"));
//            query.bindValue(3, QDate::currentDate().toString(Qt::ISODate));

//            qWarning() << "updated" << sqlLbl << alreadySuggested.join(";");

//            if (!query.exec()) {
//                LOG_QUERY_ERROR(query);
//                query.finish();
//                db.rollback();
//                return *result;
//            }
//            query.finish();
//            // End draft
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
    db.commit();
    //    return *result;

    // Second pass: find non-exact match component label (without prefix) <-> atc label
    foreach(const QString &componentLbl, knownComponentNames) {
        if (componentLbl.isEmpty())
            continue;
        if (compoLblToAtcSuggested.contains(componentLbl))
            continue;

        // Always remove accents for ATC autochecking (ATC labels are retrieved without accents)
        // Component name does not exact-matches an ATC label (already tested)
        // And the component is not already found using component linking
        QString transformedLbl = Utils::removeAccents(componentLbl);
        QStringList atcCodes;

        // Try to find the ATC label using component name transformation
        // -> Remove prefix: CHLORHYDRATE DE, ACETATE DE
        QStringList toRemove;
        toRemove << "CHLORHYDRATE DE" << "CHLORHYDRATE D'"
                 << "DICHLORHYDRATE DE" << "DICHLORHYDRATE D'"
                 << "ACETATE DE" << "ACETATE D'"
                 << "MONOHYDRATE DE" << "MONOHYDRATE D'"
                 << "SULFATE DE" << "SULFATE D'"
                 << "BISULFATE DE" << "BISULFATE D'"
                 << "TRINITRATE D'" << "TRINITRATE DE"
                 << "DINITRATE D'" << "DINITRATE DE"
                 << "NITRATE D'" << "NITRATE DE"
                 << "BROMURE DE" << "BROMURE D'"
                 << "MESILATE DE" << " BASE" << " ANHYDRE"
                 << "MALATE DE" << "MALATE D'"
                 << "MALEATE DE" << "MALEATE D'"
                 << "HYDROCHLORIDE" << "MONOSULFIDE"
                 << "CHLORIDE" << "PEROXIDE" << "CITRATE"
                 << "HYDROBROMIDE" << "SUCCINATE"
                 << "ACETATE" << "DISULFIDE"
                 << "SULFATE" << "TROMETHAMINE" << "TROMETHAMINE"
                 << "BISULFATE" << "LACTATE"
                 << "DIHYDROCHLORIDE" << "FUMARATE" << "TANNATE"
                 << "BITARTRATE" << "TARTRATE" << "ESTOLATE"
                 << "BROMIDE" << "CAPROATE" << "MALATE"
                 << "MESYLATE" << "DISODIUM" << "ETHANOLATE" << "IODIDE"
                 << "MALEATE" << "NITRATE" << "SODIUM";
        foreach(const QString &prefix, toRemove) {
            if (transformedLbl.contains(prefix)) {
                QString tmp = transformedLbl;
                tmp.remove(prefix);
                tmp = tmp.simplified();
                atcCodes << atcCodeToName.keys(tmp);
            }
        }

        // Now check if we find something
        if (!atcCodes.isEmpty()) {
            compoLblToAtcSuggested.insert(componentLbl, atcCodes.join(";"));
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    // Third pass: find non-exact match component label (remove (*)) <-> atc label
    foreach(const QString &componentLbl, knownComponentNames) {
        if (componentLbl.isEmpty())
            continue;
        if (compoLblToAtcSuggested.contains(componentLbl))
            continue;

        // Always remove accents for ATC autochecking (ATC labels are retrieved without accents)
        // Component name does not exact-matches an ATC label (already tested)
        // And the component is not already found using component linking
        QString transformedLbl = Utils::removeAccents(componentLbl);
        QStringList atcCodes;

        // Try to find the ATC label using component name transformation
        // Not found try some transformations
        // remove '(*)'
        if (componentLbl.contains("(")) {
            transformedLbl = componentLbl.left(componentLbl.indexOf("(")).simplified();
            atcCodes = atcCodeToName.keys(transformedLbl);
        }

        // Now check if we find something
        if (!atcCodes.isEmpty()) {
            compoLblToAtcSuggested.insert(componentLbl, atcCodes.join(";"));
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    LOG(QString("Suggestion found (number of components): %1").arg(result->componentIdToAtcId().uniqueKeys().count()));
    result->addMessage(QString("Suggestion found (number of components): %1").arg(result->componentIdToAtcId().uniqueKeys().count()));

    // Send suggestion to the current model
    beginResetModel();
    d->saveComponentsAndSuggestions(compoLblToAtcSuggested, knownComponentNames);
    endResetModel();

    // Recreate unfound components list
    QHash<int, QString> unfoundComponents;
    const QList<int> &allCompoIds = data.compoIds.values();
    for(int i=0; i < allCompoIds.count(); ++i) {
        int id = allCompoIds.at(i);
        if (!result->containsComponentId(id)) {
            const QString &lbl = data.compoIds.key(id);
            if (lbl.contains("HOMÉOPATHIQUE", Qt::CaseInsensitive))
                result->addComponentWithoutAtcLink(id, data);
            else
                unfoundComponents.insert(id, lbl);
        }
    }
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Compute completion percent
    result->setCompletion((double)result->componentIdToAtcId().uniqueKeys().count() / (double)data.compoIds.count() * 100.0);
    LOG(QString("Molecule links completion: %1").arg(result->completionPercentage()));
    qWarning() << "UNFOUND " << unfoundComponents.keys().count();

    QStringList v = unfoundComponents.values();
    // qWarning() << v.join("  ;  ");

//    qWarning()
//            << "\nNUMBER OF MOLECULES" << knownComponentNames.count()
//            << "\nCORRECTED BY NAME" << data->correctedByName.keys().count()
//            << "\nCORRECTED BY ATC" << data->correctedByAtcCode.uniqueKeys().count()
//            << "\nFOUNDED" << data->moleculeIdToAtcId.uniqueKeys().count()
//            << QString("\nLINKERMODEL (WithATC:%1;WithoutATC:%2) %3").arg(modelFound).arg(reviewedWithoutAtcLink).arg(modelFound + reviewedWithoutAtcLink)
//            << "\nLINKERNATURE" << natureLinkerNb
//            << "\nLEFT" << (data->unfoundMoleculeAssociations.count() - reviewedWithoutAtcLink)
//            << "\nCONFIDENCE INDICE" << data->completionPercentage
//            << "\n\n";

    return *result;
}

bool ComponentAtcModel::submitAll()
{
    if (!d->_sql->submitAll()) {
        LOG_ERROR("Unable to submit model");
        return false;
    }
    LOG("Model correctly saved");
    // fetchAll();
    return true;
}

/**
 * Removes all unreviewed molecules from the currently selected component drug database. You must select
 * your database first \sa selectDatabase().
 */
int ComponentAtcModel::removeUnreviewedMolecules()
{
    const QStringList &uids = databaseUids();
    if (uids.isEmpty())
        return 0;

    QHash<int, QString> where;
    where.insert(Constants::COMPO_ISREVIEWED, "=0");
    where.insert(Constants::COMPO_DRUGDB_UID1, QString("='%1'").arg(d->_dbUid1));
    if (!d->_dbUid2.isEmpty())
        where.insert(Constants::COMPO_DRUGDB_UID2, QString("='%1'").arg(d->_dbUid2));
    QString removeUnreviewed = ddiCore()->database().prepareDeleteQuery(Constants::Table_COMPONENTS, where);
    qWarning() << removeUnreviewed;
    QSqlDatabase db = ddiCore()->database().database();
    ddiCore()->database().executeSQL(removeUnreviewed, db);
    if (uids.count() == 2)
        selectDatabase(uids.at(0), uids.at(1));
    else
        selectDatabase(uids.at(0));
    return 0;
}

