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
    QStandardItemModel(parent),
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
    foreach(const int sid, icdBase()->getHeadersSID(SID)) {
        if (d->m_ExcludedSIDs.contains(sid)) {
            qWarning() << "asso header excluded" << d->m_ExcludedSIDs << sid;
            return false;
        }
    }

    // code daget ?
    if (!icdBase()->codeCanBeUsedAlone(SID))
        return false;

    return true;
}

bool IcdCollectionModel::canAddThisAssociation(const Internal::IcdAssociation &asso) const
{
    // Association valid ?
    if (!asso.isValid()) {
        qWarning() << "not valid";
        return false;
    }

    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(asso.mainSid().toInt())) {
        qWarning() << "main excluded" << d->m_ExcludedSIDs << asso.mainSid().toInt();
        return false;
    }
    foreach(const int sid, icdBase()->getHeadersSID(asso.mainSid().toInt())) {
        if (d->m_ExcludedSIDs.contains(sid)) {
            qWarning() << "main header excluded" << d->m_ExcludedSIDs << sid;
            return false;
        }
    }

    // already included ?
    if (d->m_SIDs.contains(asso.associatedSid().toInt())) {
        qWarning() << "asso already";
        return false;
    }
    // in exclusions ?
    if (d->m_ExcludedSIDs.contains(asso.associatedSid().toInt())) {
        qWarning() << "asso excluded" << d->m_ExcludedSIDs << asso.associatedSid().toInt();
        return false;
    }
    foreach(const int sid, icdBase()->getHeadersSID(asso.associatedSid().toInt())) {
        if (d->m_ExcludedSIDs.contains(sid)) {
            qWarning() << "asso header excluded" << d->m_ExcludedSIDs << sid;
            return false;
        }
    }

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

    QStandardItem *parentItem = invisibleRootItem();
    QList<QStandardItem *> list;
    list
            << new QStandardItem(icdBase()->getIcdCode(SID).toString())
            << new QStandardItem(icdBase()->getSystemLabel(SID))
            << new QStandardItem(icdBase()->getIcdCode(SID).toString())  // Code without daget
            << new QStandardItem(QString())            // Human readable daget
            << new QStandardItem(QString())            // DagCode
            << new QStandardItem(SID.toString());  // SID
    parentItem->appendRow(list);

    // get all exclusions
    d->m_ExcludedSIDs << icdBase()->getExclusions(SID);
    return true;
}

bool IcdCollectionModel::addAssociation(const Internal::IcdAssociation &asso)
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

    // Find root item (mainItem) based on the SID
    QStandardItem *parentItem = 0;
    QStandardItem *main = 0;
    QList<QStandardItem *> list;
    if (asso.mainIsDag()) {
        list = findItems(asso.mainCodeWithDagStar(), Qt::MatchExactly, 0);
    } else {
        list = findItems(asso.associatedCodeWithDagStar(), Qt::MatchExactly, 0);
    }
    if (list.count()==0) {
        parentItem = invisibleRootItem();
        if (asso.mainIsDag()) {
            main = new QStandardItem(asso.mainCodeWithDagStar());
            list
                    << main
                    << new QStandardItem(asso.mainLabel())
                    << new QStandardItem(asso.mainCode())     // Code without daget
                    << new QStandardItem(asso.mainDaget())    // Human readable daget
                    << new QStandardItem(asso.dagCode())      // DagCode
                    << new QStandardItem(asso.mainSid().toString())
                    ;
        } else {
            main = new QStandardItem(asso.associatedCodeWithDagStar());
            list
                    << main
                    << new QStandardItem(asso.associatedLabel())
                    << new QStandardItem(asso.associatedCode())     // Code without daget
                    << new QStandardItem(asso.associatedDaget())    // Human readable daget
                    << new QStandardItem(icdBase()->invertDagCode(asso.dagCode()))
                    << new QStandardItem(asso.associatedSid().toString())
                    ;
        }
        parentItem->appendRow(list);
        parentItem = main;
    } else {
        parentItem = list.at(0);
    }
    list.clear();
    if (asso.mainIsDag()) {
        list
                << new QStandardItem(asso.associatedCodeWithDagStar())
                << new QStandardItem(asso.associatedLabel())
                << new QStandardItem(asso.associatedCode())     // Code without daget
                << new QStandardItem(asso.associatedDaget())    // Human readable daget
                << new QStandardItem(icdBase()->invertDagCode(asso.dagCode()))
                << new QStandardItem(asso.associatedSid().toString())
                ;
    } else {
        list
                << new QStandardItem(asso.mainCodeWithDagStar())
                << new QStandardItem(asso.mainLabel())
                << new QStandardItem(asso.mainCode())     // Code without daget
                << new QStandardItem(asso.mainDaget())    // Human readable daget
                << new QStandardItem(asso.dagCode())      // DagCode
                << new QStandardItem(asso.mainSid().toString())
                ;
    }
    parentItem->appendRow(list);

    // get all exclusions
    if (asso.mainIsDag()) {
        d->m_ExcludedSIDs << icdBase()->getExclusions(asso.mainSid());
    } else {
        d->m_ExcludedSIDs << icdBase()->getExclusions(asso.associatedSid());
    }

    return true;
}


//QVariant IcdCollectionModel::data(const QModelIndex &index, int role) const
//{
//    if (!index.isValid())
//        return QVariant();

//    QStandardItemModel::data(index, role);

//    return QVariant();
//}

//bool IcdCollectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//}

void IcdCollectionModel::clearCollection()
{
    d->m_SIDs.clear();
    d->m_ExcludedSIDs.clear();
    QStandardItemModel::clear();
}

Qt::ItemFlags IcdCollectionModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QString IcdCollectionModel::toXml() const
{
}

bool IcdCollectionModel::fromXml(const QString &xml)
{
}
