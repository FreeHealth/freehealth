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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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

namespace DDI {
namespace Internal {

class AtcTableModelPrivate
{
public:
    AtcTableModelPrivate(AtcTableModel *parent) :
        _sql(0),
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~AtcTableModelPrivate()
    {}

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
    d->_sql = new QSqlTableModel(this, ddiCore()->database().database());
    d->_sql->setTable(ddiCore()->database().table(Constants::Table_ATC));
    d->_sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
    Utils::linkSignalsFromFirstModelToSecondModel(d->_sql, this, true);
}

AtcTableModel::~AtcTableModel()
{}

/** Initialize ATC Model. Read the ATC CSV file in the global_resources */
bool AtcTableModel::initialize()
{
    d->_sql->select();
    return true;
}

int AtcTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int AtcTableModel::columnCount(const QModelIndex &parent) const
{
    return d->_sql->columnCount(parent);
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

    qWarning() << index << sqlIndex << value << role;

    return d->_sql->setData(sqlIndex, value, role);
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

bool AtcTableModel::submit()
{
    bool ok = d->_sql->submitAll();
    if (ok)
        d->_sql->database().commit();
    return ok;
}
