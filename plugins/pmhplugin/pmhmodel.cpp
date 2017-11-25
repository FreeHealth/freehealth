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
#include "pmhmodel.h"
#include "pmhbase.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <icdplugin/icdcollectionmodel.h>
#include <icdplugin/icdio.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QFont>
#include <QSqlQuery>
#include <QSqlDatabase>

#include <QDebug>

using namespace PMH;
using namespace Trans::ConstantTranslations;


static inline Internal::PmhBase *base() {return Internal::PmhBase::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}


enum PrivateDataRepresentation {
    Id = 0,
    PatientUid,
    Label,
    Type,
    DateStart,
    DateEnd,
    ConfidenceIndex,
    XmlIcdCoding,
    Icon,
    FontBold,
    ColumnCount
};

//<?xml version='1.0' encoding='UTF-8'?>
//<IcdCollection dbversion="ICD10 OFSV24" date="2010-12-28T00:02:53">
//  <IcdCode sid="3806" code="I11.9" label="cardiopathie hypertensive, sans insuffisance cardiaque congestive"/>
//</IcdCollection>

// {73752057-a7aa-4bc9-8b9f-41c3b9db4e1d}

namespace {

    class TreeItem
    {
    public:
        TreeItem(const QHash<int, QVariant> &datas, TreeItem *parent = 0) :
                m_Parent(parent),
                m_Datas(datas),
                m_IsModified(false)
        {
            if (m_Parent)
                m_Parent->addChildren(this);
        }
        ~TreeItem() { qDeleteAll(m_Children); }

        // Genealogy management
        TreeItem *child(int number) { return m_Children.value(number); }
        int childCount() const { return m_Children.count(); }
        int columnCount() const { return ColumnCount; }
        TreeItem *parent() { return m_Parent; }
        void setParent(TreeItem *parent) { m_Parent = parent; }
        bool addChildren(TreeItem *child)
        {
            if (!m_Children.contains(child))
                m_Children.append(child);
            return true;
        }
        bool insertChild(const int row, TreeItem *child)
        {
            if (row > m_Children.count())
                return false;
            m_Children.insert(row, child);
            return true;
        }
        int childNumber() const
        {
            qDebug() << Q_FUNC_INFO;
            if (m_Parent) {
                qDebug() << m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
                return m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
            }
            return 0;
        }
    //    void sortChildren()
    //    {
    //        qSort(m_Children.begin(), m_Children.end(), TreeItem::lessThan);
    //    }

        bool removeChild(TreeItem *child)
        {
            if (m_Children.contains(child)) {
                m_Children.removeAll(child);
                return true;
            }
            return false;
        }

        // For data management
        QVariant data(int column) const
        {
            return m_Datas.value(column, QVariant());
        }

        bool setData(int column, const QVariant &value)
        {
            qDebug()<< data(column) << value << (data(column)==value);
            if (data(column)==value)
                return true;
            m_Datas.insert(column, value);
            m_IsModified = true;
            if (!m_DirtyRows.contains(column))
                m_DirtyRows.append(column);
            return true;
        }

        QVector<int> dirtyRows() const
        {
            return m_DirtyRows;
        }

        // For sort functions
    //    static bool lessThan(TreeItem *item1, TreeItem *item2)
    //    {
    //        // category goes first
    //        // then sort by name
    //        bool sameType = (((item1->isTemplate()) && (item2->isTemplate())) || ((!item1->isTemplate()) && (!item2->isTemplate())));
    //        if (sameType)
    //            return item1->data(Constants::Data_Label).toString() < item2->data(Constants::Data_Label).toString();
    //        return item2->isTemplate();
    //    }

    private:
        TreeItem *m_Parent;
        QList<TreeItem*> m_Children;
        QHash<int, QVariant> m_Datas;
        QVector<int> m_DirtyRows;
        bool m_IsTemplate, m_IsModified;
    };

}


namespace PMH {
namespace Internal {


class PmhModelPrivate
{
public:
    PmhModelPrivate(PmhModel *parent) :
            m_Root(0),
            q(parent)
    {
        // create root item
        QHash<int, QVariant> datas;
        datas.insert(Label, "ROOT");
        m_Root = new TreeItem(datas, 0);
    }

    ~PmhModelPrivate ()
    {
        if (m_Root) {
            delete m_Root; m_Root=0;
        }
    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_Root;
    }

    /** \brief retrieve from database the label and id only of MH, and create a root item with. */
    void getHeaders()
    {
        // Select all MH labels from the DB
        QSqlQuery query(base()->database());
        QHash<int, QString> where;
        where.insert(Constants::MASTER_PATIENT_UID, QString("='%1'").arg(patient()->data(Core::IPatient::Uid).toString()));
        QString req = base()->select(Constants::Table_MASTER,
                                     QList<int>()
                                     << Constants::MASTER_ID
                                     << Constants::MASTER_LABEL
                                     << Constants::MASTER_TYPE
                                     << Constants::MASTER_STATE,
                                     where
                                     );
        if (query.exec(req)) {
            while (query.next()) {
                QHash<int, QVariant> datas;
                datas.insert(Id, query.value(0));
                datas.insert(Label, query.value(1));
                datas.insert(Type, query.value(2));
                datas.insert(FontBold, 1);
                TreeItem *item = new TreeItem(datas, m_Root);

                // Get Type and status
                datas.clear();
                datas.insert(Type, query.value(2));
                datas.insert(Label, QString("%1 - %2")
                             .arg(Constants::typeToString(query.value(2).toInt()))
                             .arg(Constants::statusToString(query.value(3).toInt())));
                new TreeItem(datas, item);

                getChildrenItem(item);
            }
        } else {
            Utils::Log::addQueryError(q,query,__FILE__,__LINE__);
        }
    }

    bool getChildrenItem(TreeItem *parent)
    {
//        if (parent->childCount() > 1)
//            return true;

        // Get Episodes
        QSqlQuery query(base()->database());
        QHash<int, QString> where;
        where.insert(Constants::MASTER_PATIENT_UID, QString("='%1'").arg(patient()->data(Core::IPatient::Uid).toString()));
        QString whereClause = base()->getWhereClause(Constants::Table_MASTER, where);
        QString req = QString("%1, `%2` WHERE %3 AND %4")
                      .arg(base()->select(Constants::Table_EPISODE))
                      .arg(base()->table(Constants::Table_MASTER))
                      .arg(whereClause)
                      .arg(base()->fieldEquality(Constants::Table_MASTER, Constants::MASTER_EPISODE_ID,
                                                 Constants::Table_EPISODE, Constants::EPISODE_MASTER_ID));
        if (query.exec(req)) {
            while (query.next()) {
                QHash<int, QVariant> datas;
                // Get dates
                datas.insert(Id, query.value(0));
                QDate start = QDate::fromString(query.value(Constants::EPISODE_DATE_START).toString(), Qt::ISODate);
                QDate end = QDate::fromString(query.value(Constants::EPISODE_DATE_END).toString(), Qt::ISODate);
                datas.insert(DateStart, start);
                datas.insert(DateEnd, end);
                if (query.value(Constants::EPISODE_DATE_END).isNull()) {
                    datas.insert(Label, start.toString(QLocale().dateFormat()));
                } else {
                    datas.insert(Label, QString("%1 - %2")
                                 .arg(start.toString(QLocale().dateFormat()))
                                 .arg(end.toString(QLocale().dateFormat())));
                }
                datas.insert(Label, QString("%1 : %2")
                             .arg(datas.value(Label).toString())
                             .arg(query.value(Constants::EPISODE_LABEL).toString()));
                TreeItem *item = new TreeItem(datas, parent);

                // Get Confidence Index
                datas.insert(ConfidenceIndex, query.value(Constants::EPISODE_CONF_INDEX));
                datas.insert(Label, QCoreApplication::translate(Constants::MH_CONSTANTS_TR_CONTEXT, "Confidence Index") + " " + query.value(Constants::EPISODE_CONF_INDEX).toString());
                new TreeItem(datas, item);

                // Get ICD Coding
                QHash<int, QString> icdwhere;
                icdwhere.insert(Constants::ICD_ID, QString("='%1'").arg(query.value(Constants::EPISODE_ICD_CODES).toString()));
                QString icdWhereClause = base()->getWhereClause(Constants::Table_EPISODE, icdwhere);
                QString icd = QString("%1, `%2` WHERE %3 AND %4")
                              .arg(base()->select(Constants::Table_ICD))
                              .arg(base()->table(Constants::Table_EPISODE))
                              .arg(icdWhereClause)
                              .arg(base()->fieldEquality(Constants::Table_ICD, Constants::ICD_ID,
                                                         Constants::Table_EPISODE, Constants::EPISODE_ID));
                QSqlQuery icdQuery(icd, base()->database());
                if (icdQuery.isActive()) {
                    if (icdQuery.next()) {
                        // Create an ICD model
                        ICD::IcdCollectionModel icdmodel(q);
                        ICD::IcdIO().icdCollectionFromXml(&icdmodel, icdQuery.value(Constants::ICD_CONTENT).toString());
                        // Get all rows label and code
                        for(int i = 0; i < icdmodel.rowCount(); ++i) {
                            QModelIndex label = icdmodel.index(i, ICD::IcdCollectionModel::Label);
                            QHash<int, QVariant> dt;
                            dt.insert(Label, QString("%1 - %2")
                                      .arg(icdmodel.index(i, ICD::IcdCollectionModel::CodeWithDaget).data().toString())
                                      .arg(label.data().toString()));
                            TreeItem *icdItem = new TreeItem(dt, item);
                            // associations ?
                            for(int j = 0; j < icdmodel.rowCount(label); ++j) {
                                QHash<int, QVariant> dt2;
                                dt2.insert(Label, QString("%1 - %2")
                                          .arg(icdmodel.index(j, ICD::IcdCollectionModel::CodeWithDaget, label).data().toString())
                                          .arg(icdmodel.index(j, ICD::IcdCollectionModel::Label, label).data().toString()));
                                TreeItem *icdChildItem = new TreeItem(dt2, icdItem);
                            }
                        }
                    }
                } else {
                    Utils::Log::addQueryError(q,icdQuery,__FILE__,__LINE__);
                }
                // Get Contacts
            }
        } else {
            Utils::Log::addQueryError(q,query,__FILE__,__LINE__);
        }
        qDebug() << req;

        return true;
    }


public:
    TreeItem *m_Root;

private:
    PmhModel *q;
};
}  // End namespace Internal
}  // End namespace PMH


PmhModel::PmhModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::PmhModelPrivate(this))
{
    // Create headers
    d->getHeaders();

    // Connect to language change
}

PmhModel::~PmhModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

QModelIndex PmhModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     TreeItem *parentItem = d->getItem(parent);
     TreeItem *childItem = parentItem->child(row);

     if (childItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex PmhModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     TreeItem *childItem = d->getItem(index);
     TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_Root)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int PmhModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int PmhModel::columnCount(const QModelIndex &parent) const
{
    return MH_ColumnCount;
}

void PmhModel::fetchMore(const QModelIndex &parent)
{
}

bool PmhModel::canFetchMore(const QModelIndex &parent) const
{
    return false;
}

QVariant PmhModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == MH_EmptyColumn)
        return QVariant();

    const TreeItem *it = d->getItem(index);

    if (role==Qt::DisplayRole) {
        if (index.column()==MH_Label)
            return it->data(Label);
    } else if (role==Qt::FontRole) {
        if (it->data(FontBold).toInt() == 1) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    }

    return QVariant();
}

bool PmhModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}

Qt::ItemFlags PmhModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//QVariant PmhModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return QVariant();
//}

//bool PmhModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    return true;
//}

//bool PmhModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    return true;
//}

