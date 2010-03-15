/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "globaldrugsmodel.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/drugsdatabaseselector.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants.h>

#include <QList>
#include <QColor>

#include <QDebug>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

/**
  \todo no singleton otherwise search filter is applied to all views.
*/

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    GlobalDrugsModelPrivate(GlobalDrugsModel *parent) : q(parent)
    {
        Q_ASSERT(q);
    }

    static void updateCachedAvailableDosage()
    {
        m_CachedAvailableDosageForUID.clear();
        m_CachedAvailableDosageForUID = drugsBase()->getAllUIDThatHaveRecordedDosages();
    }

    static bool UIDHasRecordedDosage(const int uid)
    {
        return m_CachedAvailableDosageForUID.contains(uid);
    }

    QString getConstructedDrugName(const int row)
    {
        if (drugsBase()->actualDatabaseInformations()) {
            QString tmp = drugsBase()->actualDatabaseInformations()->drugsNameConstructor;
            if (!tmp.isEmpty()) {
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_NAME),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_NAME)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_FORM),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_FORM)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_ROUTE),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_ROUTE)).toString());
                tmp.replace(drugsBase()->field(Constants::Table_DRUGS, Constants::DRUGS_STRENGTH),
                            q->QSqlTableModel::data(q->index(row, Constants::DRUGS_STRENGTH)).toString());
                return tmp;
            }
        }
        return q->QSqlTableModel::data(q->index(row, Constants::DRUGS_NAME)).toString();
    }


private:
    static QList<int> m_CachedAvailableDosageForUID;
    GlobalDrugsModel *q;
};

/** \todo clear static cachedCISDosage when no entity of this object left */
QList<int> GlobalDrugsModelPrivate::m_CachedAvailableDosageForUID;

}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

void GlobalDrugsModel::updateAvailableDosages()
{
    Internal::GlobalDrugsModelPrivate::updateCachedAvailableDosage();
}

GlobalDrugsModel::GlobalDrugsModel(QObject *parent) :
        QSqlTableModel(parent, QSqlDatabase::database(Constants::DRUGS_DATABASE_NAME)),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate(this);
    d->updateCachedAvailableDosage();
    setTable(drugsBase()->table(Constants::Table_DRUGS));
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert(Constants::DRUGS_MARKET, "=1");
    setFilter(drugsBase()->getWhereClause(Constants::Table_DRUGS, where));
    select();
}

QVariant GlobalDrugsModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (item.column() == DrugsDB::Constants::DRUGS_NAME) {
            return d->getConstructedDrugName(item.row());
        }
    } else if (role == Qt::BackgroundRole) {
        if (item.column() == Constants::DRUGS_NAME) {
            if (settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool()) {
                QModelIndex cis = index(item.row(), Constants::DRUGS_UID);
                if (d->UIDHasRecordedDosage(cis.data().toInt())) {
                    QColor c = QColor(settings()->value(Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString());
                    c.setAlpha(125);
                    return c;
                }
            }
        }
    } else if (role == Qt::ToolTipRole) {
        QString tmp = d->getConstructedDrugName(item.row()) + "\n";
        tmp += "    " + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_FORM)).toString() + "\n";
        tmp += "    " + QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_ROUTE)).toString() + "\n";
        QString atc = QSqlTableModel::data(index(item.row(), DrugsDB::Constants::DRUGS_ATC)).toString();
        if (atc.isEmpty())
            tmp += "    No ATC found";
        else
            tmp += "    " + atc;
        return tmp;
    }
    return QSqlTableModel::data(item,role);
}

Qt::ItemFlags GlobalDrugsModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GlobalDrugsModel::updateCachedAvailableDosage()
{
    d->updateCachedAvailableDosage();
}
