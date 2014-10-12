/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Guard::Internal::GuardianTableModel
 */

#include "guardiantablemodel.h"
#include <guardplugin/guardcore.h>
#include <guardplugin/database/constants_db.h>
#include <guardplugin/database/guardbase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_patient.h>

#include <QFont>
#include <QLocale>
#include <QDate>
#include <QColor>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QDebug>

using namespace Guard;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Guard::GuardCore &guardCore() {return Guard::GuardCore::instance();}
static inline Guard::Internal::GuardBase &guardBase() {return guardCore().guardBase();}

namespace Guard {
namespace Internal {
class GuardianTableModelPrivate
{
public:
    GuardianTableModelPrivate(GuardianTableModel *parent) :
        _sql(0),
        q(parent)
    {
    }

    ~GuardianTableModelPrivate()
    {
    }

    void createSqlModel()
    {
        _sql = new QSqlTableModel(q, guardBase().database());
        _sql->setTable(guardBase().table(Constants::Table_Guardian));
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
        case GuardianTableModel::Id: sql = Constants::GUARDIAN_ID; break;
        case GuardianTableModel::Uid: sql = Constants::GUARDIAN_UID; break;
        case GuardianTableModel::IsValid: sql = Constants::GUARDIAN_ISVALID; break;
        case GuardianTableModel::FullName: sql = Constants::GUARDIAN_FULLNAME; break;
        case GuardianTableModel::EMail: sql = Constants::GUARDIAN_EMAIL; break;
        case GuardianTableModel::MobilePhoneNumber: sql = Constants::GUARDIAN_ACCEPT_MAILS; break;
        case GuardianTableModel::AcceptMails: sql = Constants::GUARDIAN_MOBILEPHONE; break;
        case GuardianTableModel::AcceptSMS: sql = Constants::GUARDIAN_ACCEPT_SMS; break;
        };
        return sql;
    }

public:
    QSqlTableModel *_sql;

private:
    GuardianTableModel *q;
};
}  // End namespace Internal
}  // End namespace Guard

/**
 * Creates a Drug-drug interaction table model database wrapper.
 * The model is empty, you can populate it using intialize()
 */
GuardianTableModel::GuardianTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::GuardianTableModelPrivate(this))
{
    setObjectName("GuardianTableModel");
    d->createSqlModel();
}

GuardianTableModel::~GuardianTableModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Initialize the model (fetch all interactors from database). */
bool GuardianTableModel::initialize()
{
    // d->_rowErrors.clear();
    d->_sql->select();

    // Fetch all data in the source model
    while (d->_sql->canFetchMore(index(d->_sql->rowCount(), 0)))
        d->_sql->fetchMore(index(d->_sql->rowCount(), 0));

    return true;
}

bool GuardianTableModel::onGuardDatabaseChanged()
{
    delete d->_sql;
    d->_sql = 0;
    d->createSqlModel();
    return initialize();
}

int GuardianTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int GuardianTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void GuardianTableModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool GuardianTableModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

QVariant GuardianTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), d->modelColumnToSqlColumn(index.column()));
        return d->_sql->data(sqlIndex, role);
    } else if (role==Qt::ForegroundRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), d->modelColumnToSqlColumn(IsValid));
        if (!sqlIndex.data().toBool())
            return QColor(20,250,20).dark();
    }

    return QVariant();
}

bool GuardianTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    // Read-only cols
    switch (index.column()) {
    case GuardianTableModel::Id:
    case GuardianTableModel::Uid:
        return false;
    }

    int sql = d->modelColumnToSqlColumn(index.column());
    if (role == Qt::EditRole) {
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        switch (index.column()) {
        case IsValid:
        case AcceptMails:
        case AcceptSMS:
            ok = d->_sql->setData(sqlIndex, value.toBool()?1:0, role);
            break;
        default: ok = d->_sql->setData(sqlIndex, value, role); break;
        }

        return ok;
    } else if (role == Qt::CheckStateRole) {
        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);
        switch (index.column()) {
        case IsValid:
        case AcceptMails:
        case AcceptSMS:
            ok = d->_sql->setData(sqlIndex, value.toInt()==Qt::Checked?1:0, role);
            break;
        default: ;
        }
        return ok;
    }
    return false;
}

bool GuardianTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool ok = d->_sql->insertRows(row, count, parent);
    if (!ok)
        LOG_ERROR(d->_sql->lastError().text());
    return ok;
}

bool GuardianTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    for(int i = 0; i < count; ++i)
        d->_sql->setData(d->_sql->index(row+i, IsValid), 0);
    endRemoveRows();
    return true;
}

QVariant GuardianTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return tr("Id");
        case Uid: return tr("Uid");
        case IsValid: return tr("Is valid");
        case FullName: return tkTr(Trans::Constants::FULLNAME);
        case EMail: return tkTr(Trans::Constants::EMAIL);
        case MobilePhoneNumber: return tkTr(Trans::Constants::MOBILEPHONE);
        case AcceptMails: return tr("Accept emails");
        case AcceptSMS: return tr("Accept SMS");
        }
    }
    return QVariant();
}

Qt::ItemFlags GuardianTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    switch (index.column()) {
    case IsValid:
    case AcceptMails:
    case AcceptSMS:
        f |= Qt::ItemIsUserCheckable;
        break;
    }
    return f;
}

/** React at primeInsert() signal, populate the newly created row with the default values */
void GuardianTableModel::populateNewRowWithDefault(int row, QSqlRecord &record)
{
    Q_UNUSED(row);
//    record.clearValues();
//    for(int i = 0; i < d->_sql->columnCount(); ++i) {
//        record.setGenerated(i, true);
//        record.setValue(i, QVariant());
//    }
//    // We need to force the DDI_ID in the record (we can not let the db chose the ID value)
//    record.setValue(Constants::DDI_ID, ddiBase().max(Constants::Table_DDI, Constants::DDI_ID).toInt() + 1);
//    record.setValue(Constants::DDI_UID, Utils::createUid());
//    record.setValue(Constants::DDI_ISREVIEWED, 0);
//    record.setValue(Constants::DDI_ISVALID, 1);
//    record.setValue(Constants::DDI_LEVELCODE, "P");
//    record.setValue(Constants::DDI_DATECREATE, QDate::currentDate());
//    record.setValue(Constants::DDI_DATEUPDATE, QDate::currentDate());
//    record.setValue(Constants::DDI_SOURCE, "FreeMedForms");
//    record.setValue(Constants::DDI_FIRSTDOSEUSEFROM, 0);
//    record.setValue(Constants::DDI_FIRSTDOSEUSESTO, 0);
//    record.setValue(Constants::DDI_SECONDDOSEUSEFROM, 0);
//    record.setValue(Constants::DDI_SECONDDOSEUSESTO, 0);
}

/** Submit changes directly to the database */
bool GuardianTableModel::submitAll()
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

void GuardianTableModel::setSqlFilter(const QString &filter)
{
    d->_sql->setFilter(filter);
    d->_sql->select();
}
