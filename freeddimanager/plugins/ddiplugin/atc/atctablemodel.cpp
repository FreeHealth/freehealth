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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DDI::AtcTableModel
 * Table model over the ATC database. The model is sorted by code and can be used
 * to create a DDI::AtcTreeProxyModel. \n
 * Do not use insertRow(), removeRow() to create or remove an ATC code. You must
 * instead use the createAtcCode() and removeAtcCode().
 */

#include "atctablemodel.h"
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QString>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DDI::DDICore *ddiCore()  { return DDI::DDICore::instance(); }
static inline DDI::Internal::DDIDatabase &ddiBase()  { return DDI::DDICore::instance()->database(); }

namespace DDI {
namespace Internal {

class AtcTableModelPrivate
{
public:
    AtcTableModelPrivate(AtcTableModel *parent) :
        _sql(0),
        q(parent)
    {
    }

    ~AtcTableModelPrivate()
    {}

    void createSqlModel()
    {
        _sql = new QSqlTableModel(q, ddiBase().database());
        _sql->setTable(ddiBase().table(Constants::Table_ATC));
        _sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
        _sql->setSort(Constants::ATC_CODE, Qt::AscendingOrder);

        // QObject::connect(_sql, SIGNAL(primeInsert(int,QSqlRecord&)), q, SLOT(populateNewRowWithDefault(int, QSqlRecord&)));
        QObject::connect(_sql, SIGNAL(layoutAboutToBeChanged()), q, SIGNAL(layoutAboutToBeChanged()));
        QObject::connect(_sql, SIGNAL(layoutChanged()), q, SIGNAL(layoutChanged()));
        QObject::connect(_sql, SIGNAL(modelAboutToBeReset()), q, SIGNAL(modelAboutToBeReset()));
        QObject::connect(_sql, SIGNAL(modelReset()), q, SIGNAL(modelReset()));
    }

public:
    QSqlTableModel *_sql;

private:
    AtcTableModel *q;
};

}  // namespace Internal
}  // namespace DDI

AtcTableModel::AtcTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new AtcTableModelPrivate(this))
{
    setObjectName("AtcTableModel");
    d->createSqlModel();
}

AtcTableModel::~AtcTableModel()
{}

/** Initialize the model */
bool AtcTableModel::initialize()
{
    d->_sql->select();
    while (d->_sql->canFetchMore(index(d->_sql->rowCount(), 0)))
        d->_sql->fetchMore(index(d->_sql->rowCount(), 0));
    return true;
}

bool AtcTableModel::onDdiDatabaseChanged()
{
    delete d->_sql;
    d->_sql = 0;
    d->createSqlModel();
    return initialize();
}

int AtcTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int AtcTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void AtcTableModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool AtcTableModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

QVariant AtcTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int sql = -1;
    switch (index.column()) {
    case Id: sql = Constants::ATC_ID; break;
    case IsValid: sql = Constants::ATC_ISVALID; break;
    case Code: sql = Constants::ATC_CODE; break;
    case Uid: sql = Constants::ATC_UID; break;
    case LabelFr: sql = Constants::ATC_FR; break;
    case LabelEn: sql = Constants::ATC_EN; break;
    case LabelDe: sql = Constants::ATC_DE; break;
    case LabelSp: sql = Constants::ATC_SP; break;
    case DateCreation: sql = Constants::ATC_DATECREATE; break;
    case DateUpdate: sql = Constants::ATC_DATEUPDATE; break;
    case PreviousCode: sql = Constants::ATC_PREVIOUSCODE; break;
    case WhoUpdateYear: sql = Constants::ATC_WHOYEARUPDATE; break;
    case Comment: sql = Constants::ATC_COMMENT; break;
    };
    QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

    return d->_sql->data(sqlIndex, role);
}

bool AtcTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (role != Qt::EditRole)
        return false;
    int sql = -1;
    switch (index.column()) {
    case Id: sql = Constants::ATC_ID; break;
    case IsValid: sql = Constants::ATC_ISVALID; break;
    case Code: sql = Constants::ATC_CODE; break;
    case Uid: sql = Constants::ATC_UID; break;
    case LabelFr: sql = Constants::ATC_FR; break;
    case LabelEn: sql = Constants::ATC_EN; break;
    case LabelDe: sql = Constants::ATC_DE; break;
    case LabelSp: sql = Constants::ATC_SP; break;
    case DateCreation: sql = Constants::ATC_DATECREATE; break;
    case DateUpdate: sql = Constants::ATC_DATEUPDATE; break;
    case PreviousCode: sql = Constants::ATC_PREVIOUSCODE; break;
    case WhoUpdateYear: sql = Constants::ATC_WHOYEARUPDATE; break;
    case Comment: sql = Constants::ATC_COMMENT; break;
    };

    // Save data to sql model
    QModelIndex sqlIndex = d->_sql->index(index.row(), sql);
    bool ok = d->_sql->setData(sqlIndex, value, role);
    if (!ok)
        LOG_QUERY_ERROR(d->_sql->query());
    else
        Q_EMIT dataChanged(index, index);

    // Update the DateUpdate of the sql & current model
    sqlIndex = d->_sql->index(index.row(), Constants::ATC_DATEUPDATE);
    ok = d->_sql->setData(sqlIndex, QDateTime::currentDateTime());
    QModelIndex up = this->index(index.row(), DateUpdate);
    if (!ok)
        LOG_QUERY_ERROR(d->_sql->query());
    else
        Q_EMIT dataChanged(up, up);
    return ok;
}

QVariant AtcTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return tr("Id");
        case IsValid: return tr("Is valid");
        case Code: return tr("Code");
        case Uid: return tr("Uid");
        case LabelFr: return tr("French");
        case LabelEn: return tr("English");
        case LabelDe: return tr("Deustch");
        case LabelSp: return tr("Spanish");
        case DateCreation: return tr("Date of creation");
        case DateUpdate: return tr("Date of update");
        case PreviousCode: return tr("Previuous code");
        case WhoUpdateYear: return tr("WHO update year");
        case Comment: return tr("Comment");
        }
    }
    return QVariant();
}

Qt::ItemFlags AtcTableModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

/** \sa DDI::Internal::DDIDatabase::isAtcCodeExists() */
bool AtcTableModel::isCodeExists(const QString &code) const
{
    return ddiCore()->database().isAtcCodeExists(code);
}

/** \sa DDI::Internal::DDIDatabase::isAtcUidExists() */
bool AtcTableModel::isUidExists(const QString &uid) const
{
    return ddiCore()->database().isAtcUidExists(uid);
}

/** \sa DDI::Internal::DDIDatabase::createAtcItem() */
bool AtcTableModel::createAtcCode(const QString &code, const QString &uid)
{
    beginResetModel();
    if (!ddiCore()->database().createAtcItem(code, uid)) {
        LOG_ERROR(tr("Unable to create new ATC %1:%2").arg(code).arg(uid));
        return false;
    }
    initialize();
    endResetModel();
    return true;
}

bool AtcTableModel::submit()
{
    bool ok = d->_sql->submitAll();
    if (ok)
        d->_sql->database().commit();
    return ok;
}
