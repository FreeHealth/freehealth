/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developpers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADRESS>                                                 *
 ***************************************************************************/
#include "movementmodel.h"
#include "accountbase.h"
#include "constants.h"

#include <utils/log.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <QSqlTableModel>

enum { WarnDebugMessage = true };

using namespace AccountDB;

static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
static inline AccountBase *base() { return  AccountBase::instance(); }

namespace AccountDB {
namespace Internal {

class MovementModelPrivate
{
public:
    MovementModelPrivate(MovementModel *parent) :
        m_SqlTable(0), m_IsDirty(false),
        q(parent)
    {
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_Movement));
    }
    ~MovementModelPrivate () {}

    void applySqlFilter()
    {
        QHash<int, QString> where;
        if (m_DateStart.isValid() && m_DateEnd.isValid()) {
            where.insert(Constants::MOV_DATEOFVALUE, QString("BETWEEN '%1' AND '%2'")
                         .arg(m_DateStart.toString(Qt::ISODate))
                         .arg(m_DateEnd.toString(Qt::ISODate)));
        }
        where.insert(Constants::MOV_USER_UID, QString("='%1'").arg(user()->uuid()));
        m_SqlTable->setFilter(base()->getWhereClause(Constants::Table_Movement, where));
        m_SqlTable->select();
        if (WarnDebugMessage)
            qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_SqlTable->filter();
    }

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;
    int m_YearFilter;
    QDateTime m_DateStart, m_DateEnd;

private:
    MovementModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



MovementModel::MovementModel(QObject *parent) :
        QAbstractTableModel(parent),
        d(new Internal::MovementModelPrivate(this))
{
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->setSort(Constants::MOV_DATEOFVALUE, Qt::AscendingOrder);
    setUserUuid(user()->uuid());
}

MovementModel::~MovementModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

int MovementModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->rowCount(parent);
}

int MovementModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

void MovementModel::setUserUuid(const QString &uuid)
{
    d->applySqlFilter();
    d->m_SqlTable->select();
    reset();
}

QString MovementModel::currentUserUuid() const
{
    return user()->uuid();
}

static int modelColumnToSqlColumn(const int modelCol)
{
    int sqlColumn = -1;
    switch (modelCol) {
    case MovementModel::Label: sqlColumn = Constants::MOV_LABEL; break;
    case MovementModel::Date: sqlColumn = Constants::MOV_DATE; break;
    case MovementModel::DateOfValue: sqlColumn = Constants::MOV_DATEOFVALUE; break;
    case MovementModel::Amount: sqlColumn = Constants::MOV_AMOUNT; break;
    case MovementModel::Validity: sqlColumn = Constants::MOV_VALIDITY; break;
    case MovementModel::IsValid: sqlColumn = Constants::MOV_ISVALID; break;
    case MovementModel::Details: sqlColumn = Constants::MOV_DETAILS; break;
    case MovementModel::Id: sqlColumn = Constants::MOV_ID; break;
    case MovementModel::AvailableMovementId: sqlColumn = Constants::MOV_AV_MOVEMENT_ID; break;
    case MovementModel::UserUid: sqlColumn = Constants::MOV_USER_UID; break;
    case MovementModel::AccountId: sqlColumn = Constants::MOV_ACCOUNT_ID; break;
    case MovementModel::Type: sqlColumn = Constants::MOV_TYPE; break;
    case MovementModel::Trace: sqlColumn = Constants::MOV_TRACE; break;
    case MovementModel::Comment: sqlColumn = Constants::MOV_COMMENT; break;
    }
    return sqlColumn;
}

QVariant MovementModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::EditRole || role==Qt::DisplayRole || role==Qt::ToolTipRole) {
        int sqlColumn = modelColumnToSqlColumn(index.column());
        if (sqlColumn != -1) {
            if (role==Qt::DisplayRole) {
                switch (sqlColumn) {
                case Constants::MOV_ISVALID:
                {
                    if (d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn), role).toInt() == 1)
                        return tr("Valid");
                    return tr("Not valid");
                }
                case Constants::MOV_DATE:
                {
                    QDateTime dt = d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn), role).toDateTime();
                    return dt.toString(QLocale().dateFormat(QLocale::ShortFormat));
                }
                case Constants::MOV_DATEOFVALUE:
                {
                    QDateTime dt = d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn), role).toDateTime();
                    return dt.toString(QLocale().dateFormat(QLocale::ShortFormat));
                }
                default: return d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn), role);
                }
            } else if (role==Qt::ToolTipRole) {
                switch (sqlColumn) {
                case Constants::MOV_DATE:
                {
                    QDateTime dt = d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn)).toDateTime();
                    return dt.toString(QLocale().dateFormat(QLocale::LongFormat));
                }
                case Constants::MOV_DATEOFVALUE:
                {
                    QDateTime dt = d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn)).toDateTime();
                    return dt.toString(QLocale().dateFormat(QLocale::LongFormat));
                }
                default: return d->m_SqlTable->data(d->m_SqlTable->index(index.row(), sqlColumn));
                }
            }
        }
    }
    return QVariant();
}

bool MovementModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role==Qt::EditRole) {
        int sqlColumn = modelColumnToSqlColumn(index.column());
        if (sqlColumn != -1) {
            bool ret = d->m_SqlTable->setData(d->m_SqlTable->index(index.row(), sqlColumn), value, role);
            d->m_IsDirty = d->m_SqlTable->isDirty(index);
            Q_EMIT dataChanged(index, index);
            return ret;
        }
    }
    return false;
}


QVariant MovementModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal && role==Qt::DisplayRole) {
        switch (section) {
        case Label: return tr("Label");
        case Date: return tr("Date");
        case DateOfValue: return tr("Date of value");
        case Amount: return tr("Amount");
        case Validity: return tr("Validity");
        case IsValid: return tr("Valid");
        case Details: return tr("Details");
        case Id: return "Id";
        case AvailableMovementId: return "AvMovId";
        case UserUid: return "UserUuid";
        case AccountId: return "AccountID";
        case Type: return ("Type");
        case Trace: return ("Trace");
        case Comment: return tr("Comment");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool MovementModel::setHeaderData(int section, 
                                  Qt::Orientation orientation, 
                                  const QVariant & value, 
                                  int role){
    bool ret = true;
    ret = d->m_SqlTable->setHeaderData(section,orientation,value,role);
    return ret;
}

bool MovementModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count);
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    endInsertRows();
    return ret;
}

bool MovementModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);
    d->m_IsDirty = true;
    bool ret = d->m_SqlTable->removeRows(row, count, parent);
    endRemoveRows();
    d->m_SqlTable->submitAll();
    return ret;
}

/** Removes a specific movement from the database using the \e id and \e bankId. All other fields are ignored in the filter. This function overwrites the current filter (user, year...). */
bool MovementModel::setMovementFilter(const QString &id)
{
    QHash<int, QString> where;
    where.insert(Constants::MOV_ID, QString("='%1'").arg(id));
    d->m_SqlTable->setFilter(base()->getWhereClause(Constants::Table_Movement, where));
    reset();
    return true;
}

bool MovementModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    LOG_ERROR("Model was not submitted correctly. Error: "+d->m_SqlTable->lastError().text());
    return false;
}

void MovementModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool MovementModel::isDirty() const
{
    return d->m_IsDirty;
}

QSqlError MovementModel::lastError()
{
    return d->m_SqlTable->lastError();
}

// Avoid direct SQL injection from outside this model
//void MovementModel::setFilter(const QString &filter)
//QString MovementModel::filter()

/** Clear all filtering information but the user uuid. */
void MovementModel::clearAllFilters()
{
    d->m_DateStart = QDateTime();
    d->m_DateEnd = QDateTime();
    d->applySqlFilter();
    reset();
}

/** Set the filter for all movements of the specified \e year. */
void MovementModel::setYearFilter(const int year)
{
    WARN_FUNC << year;
    d->m_DateStart = QDateTime(QDate(year, 01, 01), QTime(00,00));
    d->m_DateEnd = QDateTime(QDate(year, 12, 31), QTime(23,59,59));
    d->applySqlFilter();
    reset();
}

/** Set the filter for all movements recorded from \e start to \e end. */
void MovementModel::setDateFilter(const QDateTime &start, const QDateTime &end)
{
    d->m_DateStart = start;
    d->m_DateEnd = end;
    d->applySqlFilter();
    reset();
}

/** Set the filter for all movements recorded from \e start to \e end. */
void MovementModel::setDateFilter(const QDate &start, const QDate &end)
{
    d->m_DateStart = QDateTime(start, QTime(0,0));
    d->m_DateEnd = QDateTime(end, QTime(23,59,59));
    d->applySqlFilter();
    reset();
}
