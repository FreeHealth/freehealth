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
 ***************************************************************************/
#include "icdsearchmodel.h"
#include "icddatabase.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QString>
#include <QLocale>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

enum { WarnFilter=false };

using namespace ICD;

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}

namespace ICD {
namespace Internal {

    enum QueryFields {
        Query_SID = 0,
        Query_LID
    };

class IcdSearchModelPrivate
{
public:
    IcdSearchModelPrivate(IcdSearchModel */*parent*/) :
        m_IcdMaster(0),
        m_SearchMode(IcdSearchModel::SearchByLabel) //,q(parent)
    {
        // Master -> valid=1, level=4, code like '%search%'
        // Libelle -> XX_OMS like 'search', valid=1
        // †  -->  &#134;
    }

    QString searchQuery()
    {
        QString req;
        QString fields;
        if (m_SearchMode == IcdSearchModel::SearchByLabel) {
            fields = QString("`%1`.`%2`, `%3`.`%4` ")
                     .arg(icdBase()->table(Constants::Table_System))
                     .arg(icdBase()->fieldName(Constants::Table_System, Constants::SYSTEM_SID))
                     .arg(icdBase()->table(Constants::Table_System))
                     .arg(icdBase()->fieldName(Constants::Table_System, Constants::SYSTEM_LID));

            req = QString("SELECT %1 \nFROM %2\n"
                          "LEFT JOIN %3 ON %2.%4=%3.%5\n")
                    .arg(fields)
                    .arg(icdBase()->table(Constants::Table_System))
                    // join libelle
                    .arg(icdBase()->table(Constants::Table_Libelle))
                    .arg(icdBase()->fieldName(Constants::Table_System, Constants::SYSTEM_LID))
                    .arg(icdBase()->fieldName(Constants::Table_Libelle, Constants::LIBELLE_LID));
        } else {
            // Search by ICD Codes
//            SELECT `master`.`SID`, system.LID
//            FROM master, system
//            WHERE  (`master`.`code` like 'B5%') AND (system.SID=master.SID)
            fields = QString("`%1`.`%2`, `%3`.`%4` ")
                     .arg(icdBase()->table(Constants::Table_Master))
                     .arg(icdBase()->fieldName(Constants::Table_Master, Constants::MASTER_SID))
                     .arg(icdBase()->table(Constants::Table_System))
                     .arg(icdBase()->fieldName(Constants::Table_System, Constants::SYSTEM_LID));
            req = QString("SELECT %1\n"
                          "FROM %2, %3\n"
                          "WHERE %2.%5 = %3.%6\n")
                    .arg(fields)
                    .arg(icdBase()->table(Constants::Table_Master))
                    .arg(icdBase()->table(Constants::Table_System))
                    // Where clause
                    .arg(icdBase()->fieldName(Constants::Table_Master, Constants::MASTER_SID))
                    .arg(icdBase()->fieldName(Constants::Table_System, Constants::SYSTEM_SID));
        }

        if (WarnFilter) {
            qWarning() << req;
        }
        return req;
    }

    ~IcdSearchModelPrivate() {}

public:
    QSqlQueryModel *m_IcdMaster;
    IcdSearchModel::SearchModes m_SearchMode;
    QString m_LastFilterRequiered;
    int m_LangField;

private:
//    IcdSearchModel *q;
};


}  // End namespace Internal
}  // End namespace ICD

IcdSearchModel::IcdSearchModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::IcdSearchModelPrivate(this))
{
    languageChanged();
    init();
    connect(Core::ICore::instance()->translators(), SIGNAL(languageChanged()), this, SLOT(languageChanged()));
    connect(icdBase(), SIGNAL(databaseChanged()), this, SLOT(refreshDatabase()));
}

IcdSearchModel::~IcdSearchModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void IcdSearchModel::init()
{
    d->m_IcdMaster = new QSqlQueryModel(this);
    d->m_IcdMaster->setQuery(d->searchQuery(), icdBase()->database());

    connect(d->m_IcdMaster,SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
    connect(d->m_IcdMaster,SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
    connect(d->m_IcdMaster,SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), this, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(rowsInserted(QModelIndex, int, int)), this, SIGNAL(rowsInserted(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    connect(d->m_IcdMaster,SIGNAL(modelAboutToBeReset()), this, SIGNAL(modelAboutToBeReset()));
    connect(d->m_IcdMaster,SIGNAL(modelReset()), this, SIGNAL(modelReset()));
}

void IcdSearchModel::setSearchMethod(SearchModes mode)
{
    if (mode==d->m_SearchMode)
        return;
    d->m_SearchMode = mode;
    // modify the query
    // update model ?
}

int IcdSearchModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_IcdMaster->rowCount();
}

int IcdSearchModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void IcdSearchModel::fetchMore(const QModelIndex &parent)
{
    d->m_IcdMaster->fetchMore(parent);
}

bool IcdSearchModel::canFetchMore(const QModelIndex &parent) const
{
    return d->m_IcdMaster->canFetchMore(parent);
}

QVariant IcdSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role==Qt::DisplayRole || role==Qt::EditRole) {
        switch (index.column()) {
        case SID_Code: return d->m_IcdMaster->index(index.row(), Internal::Query_SID).data();
        case ICD_Code: return icdBase()->getIcdCode(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        case ICD_CodeWithDagetAndStar: return icdBase()->getIcdCodeWithDagStar(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        case Daget: return icdBase()->getHumanReadableIcdDaget(d->m_IcdMaster->index(index.row(), Internal::Query_SID).data());
        case Label: return icdBase()->getLabelFromLid(d->m_IcdMaster->index(index.row(), Internal::Query_LID).data());
        }
    }
    return QVariant();
}

bool IcdSearchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}


QVariant IcdSearchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

bool IcdSearchModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

bool IcdSearchModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

void IcdSearchModel::setFilter(const QString &searchLabel)
{
    d->m_LastFilterRequiered = searchLabel;
    QHash<int, QString> where;
    QString req;
    if (d->m_SearchMode==SearchByLabel) {
        where.insert(d->m_LangField, QString("like '%1%'").arg(searchLabel));
        req = d->searchQuery() + " WHERE " + icdBase()->getWhereClause(Constants::Table_Libelle, where);
    } else {
        where.insert(Constants::MASTER_CODE, QString("like '%1%'").arg(searchLabel));
        req = d->searchQuery() + " AND " + icdBase()->getWhereClause(Constants::Table_Master, where);
    }
    if (WarnFilter)
        qWarning() << req;

    beginResetModel();
    d->m_IcdMaster->setQuery(req, icdBase()->database());
    endResetModel();
}

void IcdSearchModel::refreshDatabase()
{
    beginResetModel();
    delete d->m_IcdMaster;
    d->m_IcdMaster = 0;
    init();
    endResetModel();
}

void IcdSearchModel::languageChanged()
{
    const QString &lang = QLocale().name().left(2);
    d->m_LangField = Constants::LIBELLE_EN;
    if (lang=="fr") {
        d->m_LangField = Constants::LIBELLE_FR;
    } else if (lang=="de") {
        d->m_LangField = Constants::LIBELLE_DE_DIMDI;
    }
}
