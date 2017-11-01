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
#include "druginteractortablemodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/interactors/druginteractor.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QFont>
#include <QLocale>
#include <QDate>
#include <QColor>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QDebug>

using namespace DDI;
using namespace Internal;

static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
static inline DDI::Internal::DDIDatabase &ddiBase() {return DDI::DDICore::instance()->database();}

namespace DDI {
namespace Internal {
class DrugInteractorTableModelPrivate
{
public:
    DrugInteractorTableModelPrivate(DrugInteractorTableModel *parent) :
        _sql(0),
        q(parent)
    {
    }

    ~DrugInteractorTableModelPrivate()
    {
    }

    void createSqlModel()
    {
        _sql = new QSqlTableModel(q, ddiBase().database());
        _sql->setTable(ddiBase().table(Constants::Table_INTERACTORS));
        _sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
        // _sql->setSort(Constants::INTERACTOR_UID, Qt::AscendingOrder);

        QObject::connect(_sql, SIGNAL(primeInsert(int,QSqlRecord&)), q, SLOT(populateNewRowWithDefault(int, QSqlRecord&)));
        QObject::connect(_sql, SIGNAL(layoutAboutToBeChanged()), q, SIGNAL(layoutAboutToBeChanged()));
        QObject::connect(_sql, SIGNAL(layoutChanged()), q, SIGNAL(layoutChanged()));
        QObject::connect(_sql, SIGNAL(modelAboutToBeReset()), q, SIGNAL(modelAboutToBeReset()));
        QObject::connect(_sql, SIGNAL(modelReset()), q, SIGNAL(modelReset()));
    }

    int sqlColumn(const QModelIndex &index)
    {
        int sql = -1;
        switch (index.column()) {
        case DrugInteractorTableModel::Id: sql = Constants::INTERACTOR_ID; break;
        case DrugInteractorTableModel::Uuid: sql = Constants::INTERACTOR_UID; break;
        case DrugInteractorTableModel::IsValid: sql = Constants::INTERACTOR_ISVALID; break;
        case DrugInteractorTableModel::IsInteractingClass: sql = Constants::INTERACTOR_ISCLASS; break;
        case DrugInteractorTableModel::IsReviewed: sql = Constants::INTERACTOR_ISREVIEWED; break;
        case DrugInteractorTableModel::IsAutoFound: sql = Constants::INTERACTOR_ISAUTOFOUND; break;
        case DrugInteractorTableModel::DoNotWarnDuplicated: sql = Constants::INTERACTOR_WARNDUPLICATES; break;
        case DrugInteractorTableModel::TranslatedLabel:
        {
            switch (QLocale().language()) {
            case QLocale::French: sql = Constants::INTERACTOR_FR; break;
            case QLocale::English: sql = Constants::INTERACTOR_EN; break;
            case QLocale::German: sql = Constants::INTERACTOR_DE; break;
            default: sql = Constants::INTERACTOR_EN; break;
            }
            break;
        }
        case DrugInteractorTableModel::EnLabel: sql = Constants::INTERACTOR_EN; break;
        case DrugInteractorTableModel::FrLabel: sql = Constants::INTERACTOR_FR; break;
        case DrugInteractorTableModel::DeLabel: sql = Constants::INTERACTOR_DE; break;
        case DrugInteractorTableModel::ClassInformationFr: sql = Constants::INTERACTOR_INFO_FR; break;
        case DrugInteractorTableModel::ClassInformationEn: sql = Constants::INTERACTOR_INFO_EN; break;
        case DrugInteractorTableModel::ClassInformationDe: sql = Constants::INTERACTOR_INFO_DE; break;
        case DrugInteractorTableModel::ATCCodeStringList: sql = Constants::INTERACTOR_ATC; break;
        case DrugInteractorTableModel::DateOfCreation: sql = Constants::INTERACTOR_DATECREATE; break;
        case DrugInteractorTableModel::DateLastUpdate: sql = Constants::INTERACTOR_DATEUPDATE; break;
        case DrugInteractorTableModel::DateReview: sql = Constants::INTERACTOR_DATEREVIEW; break;
        case DrugInteractorTableModel::PMIDStringList: sql = Constants::INTERACTOR_PMIDS; break;
        case DrugInteractorTableModel::ChildrenUuid: sql = Constants::INTERACTOR_CHILDREN; break;
        case DrugInteractorTableModel::Reference: sql = Constants::INTERACTOR_REF; break;
        case DrugInteractorTableModel::Comment: sql = Constants::INTERACTOR_COMMENT; break;
        };
        return sql;
    }

public:
    QSqlTableModel *_sql;
    QStringList _distinctUids;

private:
    DrugInteractorTableModel *q;
};
}  // End namespace Internal
}  // End namespace DDI

/** Table Model over the database interactor table. The model is uid sorted */
DrugInteractorTableModel::DrugInteractorTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::DrugInteractorTableModelPrivate(this))
{
    setObjectName("DrugInteractorTableModel");
    d->createSqlModel();
}

DrugInteractorTableModel::~DrugInteractorTableModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Initialize the model (fetch all interactors from database). */
bool DrugInteractorTableModel::initialize()
{
    // Select
    d->_sql->select();

    // Fetch all data in the source model
    while (d->_sql->canFetchMore(index(d->_sql->rowCount(), 0)))
        d->_sql->fetchMore(index(d->_sql->rowCount(), 0));

    d->_distinctUids = ddiBase().interactorDistinctUids();

    return true;
}

bool DrugInteractorTableModel::onDdiDatabaseChanged()
{
    delete d->_sql;
    d->_sql = 0;
    d->createSqlModel();
    return initialize();
}

int DrugInteractorTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int DrugInteractorTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void DrugInteractorTableModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool DrugInteractorTableModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

QVariant DrugInteractorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int sql = d->sqlColumn(index);
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);
        QVariant value = d->_sql->data(sqlIndex, role);
        // If it is a langage value -> test if a value is defined or switch to another language
        if ((sql == Constants::INTERACTOR_FR
                || sql == Constants::INTERACTOR_EN
                || sql == Constants::INTERACTOR_DE)
                && value.toString().isEmpty()) {
            QModelIndex testIndex = d->_sql->index(index.row(), Constants::INTERACTOR_FR);
            QVariant test = d->_sql->data(testIndex, role);
            if (!test.toString().isEmpty())
                return test;

            testIndex = d->_sql->index(index.row(), Constants::INTERACTOR_EN);
            test = d->_sql->data(testIndex, role);
            if (!test.toString().isEmpty())
                return test;

            testIndex = d->_sql->index(index.row(), Constants::INTERACTOR_DE);
            test = d->_sql->data(testIndex, role);
            if (!test.toString().isEmpty())
                return test;

            testIndex = d->_sql->index(index.row(), Constants::INTERACTOR_UID);
            return d->_sql->data(testIndex, role);
        }
        return value;
    } else if (role == Qt::FontRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ISCLASS);
        if (d->_sql->data(sqlIndex).toBool()) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    } else if (role==Qt::ForegroundRole) {
        QModelIndex classIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ISCLASS);
        QModelIndex childIndex = d->_sql->index(index.row(), Constants::INTERACTOR_CHILDREN);
        //QModelIndex atcIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ATC);
        bool isClass = d->_sql->data(classIndex).toBool();
        const QVariant &children = d->_sql->data(childIndex); // list ; seperated
        //const QVariant &atc = d->_sql->data(atcIndex);

        if (isClass) {
            // Class without children?
            if (children.isNull())
                return QColor(255,50,50,150);

            // Children all exist
            const QStringList &list = children.toString().split(";", QString::SkipEmptyParts);
            foreach(const QString &child, list) {
                if (!ddiCore()->drugInteractorTableModel()->interactorUidExists(child))
                    return QColor(255,50,50,150);
            }

        } else {
            // Children without being a class?
            if (!children.isNull()) {
                // qWarning() << "DrugInteractor: children defined without isclass" << d->_sql->index(index.row(), Constants::INTERACTOR_FR).data();
                // qWarning() << children;
                return QColor(255,50,50,150);
            }
        }

        // Not reviewed?
        QModelIndex rev = d->_sql->index(index.row(), Constants::INTERACTOR_ISREVIEWED);
        if (!d->_sql->data(rev).toBool())
            return QColor(50,250,50,150);

        // No Reference?
        QModelIndex ref = d->_sql->index(index.row(), Constants::INTERACTOR_REF);
        if (d->_sql->data(ref).toString().simplified().isEmpty())
            return QColor(250,0,0,150);

    } else if (role == Qt::CheckStateRole) {
        int sql = -1;
        switch (index.column()) {
        case IsValid: sql = Constants::INTERACTOR_ISVALID; break;
        case IsInteractingClass: sql = Constants::INTERACTOR_ISCLASS; break;
        case IsReviewed: sql = Constants::INTERACTOR_ISREVIEWED; break;
        case IsAutoFound: sql = Constants::INTERACTOR_ISAUTOFOUND; break;
        case DoNotWarnDuplicated: sql = Constants::INTERACTOR_WARNDUPLICATES; break;
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

bool DrugInteractorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (role == Qt::EditRole) {
        int sql = d->sqlColumn(index);
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        switch (index.column()) {
        case IsValid:
        case IsInteractingClass:
        case IsReviewed:
        case IsAutoFound:
        case DoNotWarnDuplicated:
            ok = d->_sql->setData(sqlIndex, value.toBool()?1:0, role);
            break;
        case Uuid:
            ok = d->_sql->setData(sqlIndex, Constants::correctedUid(value.toString()), role);
            break;
        default: ok = d->_sql->setData(sqlIndex, value, role); break;
        }

        if (!ok)
            LOG_QUERY_ERROR(d->_sql->query());
        else
            Q_EMIT dataChanged(index, index);

        // set the date of review
        if (index.column() == IsReviewed) {
            QModelIndex reviewDateIndex = d->_sql->index(index.row(), Constants::INTERACTOR_DATEREVIEW);
            if (!d->_sql->setData(reviewDateIndex, QDate::currentDate())) {
                LOG_ERROR("Unable to set date of review");
                return false;
            } else {
                QModelIndex idx = this->index(index.row(), DateReview);
                Q_EMIT dataChanged(idx, idx);
            }
        }
        // set the date update
        if (ok) {
            sqlIndex = d->_sql->index(index.row(), Constants::INTERACTOR_DATEUPDATE);
            ok = d->_sql->setData(sqlIndex, QDateTime::currentDateTime());
            if (ok) {
                QModelIndex idx = this->index(index.row(), DateLastUpdate);
                Q_EMIT dataChanged(idx, idx);
            }
        }

        return ok;
    }
    return false;
}

bool DrugInteractorTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count);
    bool ok = d->_sql->insertRows(row, count, parent);
    endInsertRows();
    return ok;
}

bool DrugInteractorTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    for(int i = 0; i < count; ++i)
        d->_sql->setData(d->_sql->index(row+i, IsValid), 0);
    endRemoveRows();
    return true;
}

QVariant DrugInteractorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return tr("Id");
        case Uuid: return tr("Uuid");
        case IsValid: return tr("Is valid");
        case IsInteractingClass: return tr("Is interaction class");
        case IsReviewed: return tr("Is reviewed");
        case IsAutoFound: return tr("Is auto-found");
        case DoNotWarnDuplicated: return tr("DoNotWarnDuplicated");
        case TranslatedLabel: return tr("Translated label");
        case EnLabel: return tr("English label");
        case FrLabel: return tr("French label");
        case DeLabel: return tr("Deustch label");
        case ClassInformationFr: return tr("Class information (french)");
        case ClassInformationEn: return tr("Class information (english)");
        case ClassInformationDe: return tr("Class information (deustch)");
        case ATCCodeStringList: return tr("ATC codes");
        case DateOfCreation: return tr("Date of creation");
        case DateLastUpdate: return tr("Date of update");
        case DateReview: return tr("Date of review");
        case PMIDStringList: return tr("PMID");
        case ChildrenUuid: return tr("Children");
        case Reference: return tr("Reference");
        case Comment: return tr("Comment");
        }
    }
    return QVariant();
}

Qt::ItemFlags DrugInteractorTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    switch (index.column()) {
    case IsValid:
    case IsInteractingClass:
    case IsReviewed:
    case IsAutoFound:
    case DoNotWarnDuplicated:
        f |= Qt::ItemIsUserCheckable;
        break;
    }
    return f;
}

/** Returns the number of unreviewed DDI::DrugInteractor from the database. */
int DrugInteractorTableModel::numberOfUnreviewed() const
{
    // directly ask the database instead of screening the model
    QHash<int, QString> where;
    where.insert(Constants::INTERACTOR_ISREVIEWED, "=0");
    return ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
}

/** Returns the number of ATC unlinked DDI::DrugInteractor from the database. */
int DrugInteractorTableModel::numberOfUnlinked() const
{
    // directly ask the database instead of screening the model
    QHash<int, QString> where;
    where.insert(Constants::INTERACTOR_ISCLASS, "=0");
    where.insert(Constants::INTERACTOR_ATC, "IS NULL");
    return ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
}

/** Wrapper to DDI::Internal::interactorLabel() */
QString DrugInteractorTableModel::interactorLabel(const QString &uid) const
{
    return ddiBase().interactorLabel(uid);
}

/**
 * Returns \e true if the interactor with the \e uid exists.
 */
bool DrugInteractorTableModel::interactorUidExists(const QString &uid) const
{
    return d->_distinctUids.contains(uid);
}

/** Submit changes directly to the database */
bool DrugInteractorTableModel::submit()
{
#if QT_VERSION >= 0x050000
    if (!d->_sql->isDirty())
        return true;
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

/** React at primeInsert() signal, populate the newly created row with the default values */
void DrugInteractorTableModel::populateNewRowWithDefault(int row, QSqlRecord &record)
{
    Q_UNUSED(row);
    record.clearValues();
    for(int i = 0; i < d->_sql->columnCount(); ++i) {
        record.setGenerated(i, true);
        record.setValue(i, QVariant());
    }
    // We need to force the INTERACTOR_ID in the record (we can not let the db chose the ID value)
    record.setValue(Constants::INTERACTOR_ID, ddiBase().max(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID).toInt() + 1);
    record.setValue(Constants::INTERACTOR_UID, Utils::createUid());
    record.setValue(Constants::INTERACTOR_ISVALID, 1);
    record.setValue(Constants::INTERACTOR_ISREVIEWED, 0);
    record.setValue(Constants::INTERACTOR_ISAUTOFOUND, 0);
    record.setValue(Constants::INTERACTOR_ISCLASS, 0);
    record.setValue(Constants::INTERACTOR_WARNDUPLICATES, 0);
    record.setValue(Constants::INTERACTOR_DATECREATE, QDate::currentDate());
    record.setValue(Constants::INTERACTOR_DATEUPDATE, QDate::currentDate());
}

void DrugInteractorTableModel::setSqlFilter(const QString &filter)
{
    d->_sql->setFilter(filter);
    d->_sql->select();
}

/**
 * Transform all Drug interactor information into a human readable PDF file.
 * This file can be used to ease the checking process (for those you wants
 * to work on papers).
 */
void DrugInteractorTableModel::toPdfFile() const
{
    // Create cache data
    QMultiHash<QString, QString> classesUidsWithChildren;
    QHash<QString, QString> uidToName;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex isClass = index(i, IsInteractingClass);
        QModelIndex uidIndex = index(i, Uuid);
        QString uid = uidIndex.data().toString();
        QModelIndex nameIndex = index(i, FrLabel);
        uidToName.insert(uid, nameIndex.data().toString());

        if (isClass.data().toBool()) {
            QModelIndex childrenIndex = index(i, ChildrenUuid);
            QStringList children = childrenIndex.data().toString().split(";", QString::SkipEmptyParts);
            foreach(const QString &c, children)
                classesUidsWithChildren.insertMulti(uid, c);
        }
    }

    // Create output
    QStringList classOutput, interactorOutput;
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex isClass = index(i, IsInteractingClass);
        QModelIndex uidIndex = index(i, Uuid);
        QString uid = uidIndex.data().toString();
        QString name = uidToName.value(uid);

        // Class output
        if (isClass.data().toBool()) {
            const QStringList &children = classesUidsWithChildren.values(uid);
            QString out;
            out += QString("<b>%1</b><br><small>").arg(name);
            // TODO: add class info
            QString out2;
            foreach(const QString &c, children) {
                out2.prepend(QString("%1 ; ").arg(uidToName.value(c)));
            }
            out += out2;
            out += QString("</small><br><br>");
            classOutput << out;
        } else {
            // Interactor output

        }
    }
    qSort(classOutput);

    // TODO: finish the code
    Utils::saveStringToFile(classOutput.join("\n"), "/Users/eric/FreeDDIManager/Documents/classes.html", Utils::Overwrite, Utils::DontWarnUser);
}



DrugInteractorFilteredTableModel::DrugInteractorFilteredTableModel(QObject *parent) :
    DrugInteractorTableModel(parent)
{}

DrugInteractorFilteredTableModel::~DrugInteractorFilteredTableModel()
{}

bool DrugInteractorFilteredTableModel::initialize()
{
    return DrugInteractorTableModel::initialize();
}

void DrugInteractorFilteredTableModel::filterLastUpdated(const QDate &since)
{
    QHash<int, QString> where;
    int table = Constants::Table_INTERACTORS;
    where.insert(Constants::INTERACTOR_ISVALID, "=1");
    where.insert(Constants::INTERACTOR_ISREVIEWED, "=1");
    where.insert(Constants::INTERACTOR_DATEUPDATE, Constants::SQL_ISNOTNULL);
    where.insert(Constants::INTERACTOR_DATEUPDATE, QString("> '%1'").arg(since.toString(Qt::ISODate)));
    where.insert(Constants::INTERACTOR_DATECREATE, QString("< '%1'").arg(since.toString(Qt::ISODate)));
    setSqlFilter(ddiBase().getWhereClause(table, where));
}

void DrugInteractorFilteredTableModel::filterNewItems(const QDate &since)
{
    QHash<int, QString> where;
    int table = Constants::Table_INTERACTORS;
    where.insert(Constants::INTERACTOR_ISVALID, "=1");
    where.insert(Constants::INTERACTOR_ISREVIEWED, "=1");
    where.insert(Constants::INTERACTOR_DATECREATE, Constants::SQL_ISNOTNULL);
    where.insert(Constants::INTERACTOR_DATECREATE, QString("> '%1'").arg(since.toString(Qt::ISODate)));
    setSqlFilter(ddiBase().getWhereClause(table, where));
}

void DrugInteractorFilteredTableModel::filterLastUpdatedAndNewItems(const QDate &since)
{
    Q_UNUSED(since);
    // TODO: code this
}

bool DrugInteractorFilteredTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}

bool DrugInteractorFilteredTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

bool DrugInteractorFilteredTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

bool DrugInteractorFilteredTableModel::submit()
{
    // TODO: code this?
    return true;
}
