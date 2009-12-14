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

namespace DrugsDB {
namespace Internal {

class GlobalDrugsModelPrivate
{
public:
    GlobalDrugsModelPrivate()
    {}

    void updateCachedAvailableDosage()
    {
        m_CachedAvailableDosageForCIS.clear();
        m_CachedAvailableDosageForCIS = drugsBase()->getAllCISThatHaveRecordedDosages();
    }

    bool CISHasRecordedDosage(const int CIS)
    {
        return m_CachedAvailableDosageForCIS.contains(CIS);
    }

private:
    QList<int> m_CachedAvailableDosageForCIS;

};
}  // End Internal
}  // End DrugsDB

using namespace DrugsDB;

DrugsDB::GlobalDrugsModel *DrugsDB::GlobalDrugsModel::m_Instance = 0;
DrugsDB::GlobalDrugsModel *DrugsDB::GlobalDrugsModel::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new DrugsDB::GlobalDrugsModel(parent);
    return m_Instance;
}

GlobalDrugsModel::GlobalDrugsModel(QObject *parent) :
        QSqlTableModel(parent, QSqlDatabase::database(Constants::DRUGS_DATABASE_NAME)),
        d(0)
{
    d = new Internal::GlobalDrugsModelPrivate();
    d->updateCachedAvailableDosage();
    setTable(drugsBase()->table(Constants::Table_CIS));
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert(Constants::CIS_COMMERCIALISATION, "='O'");
    setFilter(drugsBase()->getWhereClause(Constants::Table_CIS, where));
    select();
}

QVariant GlobalDrugsModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (role == Qt::BackgroundRole) {
        if (item.column() == Constants::CIS_DENOMINATION) {
            if (settings()->value(DrugsDB::Constants::S_MARKDRUGSWITHAVAILABLEDOSAGES).toBool()) {
                QModelIndex cis = index(item.row(), Constants::CIS_CIS);
                if (d->CISHasRecordedDosage(cis.data().toInt())) {
                    QColor c = QColor(settings()->value(Constants::S_AVAILABLEDOSAGESBACKGROUNGCOLOR).toString());
                    c.setAlpha(125);
                    return c;
                }
            }
        }
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
