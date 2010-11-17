/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "icdcollectionmodel.h"
#include "icdassociation.h"
#include "icddatabase.h"

#include <utils/log.h>


using namespace ICD;

static inline ICD::IcdDatabase *icdBase() {return ICD::IcdDatabase::instance();}


namespace ICD {
namespace Internal {

struct ModelItem {
    ModelItem() : sid(-1) {}

    int sid;
    QString code, label;
    Internal::IcdAssociation association;
};

class IcdCollectionModelPrivate {
public:
    IcdCollectionModelPrivate(IcdCollectionModel *parent) :
            q(parent)
    {
    }
    ~IcdCollectionModelPrivate()
    {
        qDeleteAll(m_Rows);
        m_Rows.clear();
    }

public:
    QVector<int> m_ExcludedSIDs;
    QVector<int> m_SIDs;
    QList<ModelItem *> m_Rows;

private:
    IcdCollectionModel *q;
};


}  // End namespace Internal
}  // End namespace ICD

IcdCollectionModel::IcdCollectionModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::IcdCollectionModelPrivate(this))
{
    setObjectName("IcdCollectionModel");
}


bool IcdCollectionModel::canAddThisCode(const QVariant &SID) const
{
    // already included ?
    if (d->m_SIDs.contains(SID.toInt()))
        return false;

    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(SID.toInt()))
        return false;

    // code daget ?
    if (!icdBase()->codeCanBeUsedAlone(SID))
        return false;

    return true;
}

bool IcdCollectionModel::canAddThisAssociation(const Internal::IcdAssociation &asso) const
{
    // Association valid ?
    if (!asso.isValid())
        return false;

    // already included ?
    if (d->m_SIDs.contains(asso.mainSid().toInt()))
        return false;

    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(asso.mainSid().toInt()))
        return false;

    // already included ?
    if (d->m_SIDs.contains(asso.associatedSid().toInt()))
        return false;

    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(asso.associatedSid().toInt()))
        return false;

    return true;
}

bool IcdCollectionModel::addCode(const QVariant &SID)
{
    // Can add this code ?
    if (!canAddThisCode(SID)) {
        Utils::Log::addError(this, tr("Can not add this code: %1").arg(icdBase()->getIcdCode(SID).toString()));
        return false;
    }

    // add Code to model
    d->m_SIDs.append(SID.toInt());
    Internal::ModelItem *item = new Internal::ModelItem;
    item->code = icdBase()->getIcdCode(SID).toString();
    item->label = icdBase()->getSystemLabel(SID);
    d->m_Rows.append(item);

    // get all exclusions
    d->m_ExcludedSIDs << icdBase()->getExclusions(SID);
    return true;
}

bool IcdCollectionModel::addAssociation(const Internal::IcdAssociation &asso) const
{
    // Can add this association ?
    if (!canAddThisAssociation(asso)) {
        Utils::Log::addError(this, tr("Can not add this Association: %1-%2")
                             .arg(icdBase()->getIcdCode(asso.mainSid()).toString())
                             .arg(icdBase()->getIcdCode(asso.associatedSid()).toString()));
        return false;
    }

    // add Code to model
    d->m_SIDs.append(asso.mainSid().toInt());
    d->m_SIDs.append(asso.associatedSid().toInt());
    Internal::ModelItem *item = new Internal::ModelItem;
    item->association = asso;
    d->m_Rows.append(item);

    // get all exclusions
    d->m_ExcludedSIDs << icdBase()->getExclusions(asso.mainSid()) << icdBase()->getExclusions(asso.associatedSid());
    return true;
}

int IcdCollectionModel::rowCount(const QModelIndex &parent) const
{
    return d->m_Rows.count();
}

int IcdCollectionModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}


QVariant IcdCollectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > d->m_Rows.count())
        return QVariant();

    if (role==Qt::DisplayRole) {
        Internal::ModelItem *item = d->m_Rows.at(index.row());
        if (item->sid > 0) {
            switch (index.column()) {
            case 0: return item->code;
            case 1: return item->label;
            }
        } else {
            switch (index.column()) {
            case 0: return item->association.mainCodeWithDagStar() + " / " + item->association.associatedCodeWithDagStar();
            case 1: return icdBase()->getSystemLabel(item->association.mainSid()) + " / " +
                           icdBase()->getSystemLabel(item->association.associatedSid());
            }
        }
    }

    return QVariant();
}

bool IcdCollectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
}

Qt::ItemFlags IcdCollectionModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


// XML import/export
QString IcdCollectionModel::toXml() const
{
}

bool IcdCollectionModel::fromXml(const QString &xml)
{
}
