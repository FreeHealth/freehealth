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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "qabstractxmltreemodel.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constants.h>

#include <QDomNode>
#include <QDomElement>
#include <QDomDocument>
#include <QFile>
#include <QDir>

#include <QDebug>

using namespace Utils;
using namespace Internal;

namespace Utils {
namespace Internal {

class DomItem
{
public:
    DomItem(QDomNode &node, int row, DomItem *parent = 0) :
        domNode(node),
        parentItem(parent),
        rowNumber(row)
    {
    }

    ~DomItem()
    {
        QHash<int,DomItem*>::iterator it;
        for (it = childItems.begin(); it != childItems.end(); ++it)
            delete it.value();
    }

    QDomNode node() const
    {
        return domNode;
    }

    DomItem *parent()
    {
        return parentItem;
    }

    void refreshChildren()
    {
        childItems.clear();
    }

    DomItem *child(int i)
    {
        if (childItems.contains(i))
            return childItems[i];

        if (i >= 0 && i < domNode.childNodes().count()) {
            QDomNode childNode = domNode.childNodes().item(i);
            DomItem *childItem = new DomItem(childNode, i, this);
            childItems[i] = childItem;
            return childItem;
        }
        return 0;
    }

    int row()
    {
        return rowNumber;
    }

private:
    QDomNode domNode;
    QHash<int,DomItem*> childItems;
    DomItem *parentItem;
    int rowNumber;
};

class QAbstractXmlTreeModelPrivate
{
public:
    QAbstractXmlTreeModelPrivate(const QString &fileName, const QString &mainTag, QAbstractXmlTreeModel *parent) :
            m_RootItem(0), q(parent)
    {
        m_FileName = fileName;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                Utils::Log::addError(q, QApplication::translate("QAbstractXmlTreeModel","Can not read XML file content %1").arg(file.fileName()), __FILE__, __LINE__);
                Utils::Log::addError(q, QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error), __FILE__, __LINE__);
            } else {
                Utils::Log::addMessage(q, QApplication::translate("QAbstractXmlTreeModel","Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            Utils::Log::addError(q, QApplication::translate("QAbstractXmlTreeModel","Can not open XML file %1").arg(file.fileName()), __FILE__, __LINE__);
        }

        m_RootNode = domDocument.firstChildElement(mainTag);

        m_RootItem = new DomItem(m_RootNode, 0);
    }

public:
    QDomDocument domDocument;
    QDomNode m_RootNode;
    DomItem *m_RootItem;
    QString m_FileName;

private:
    QAbstractXmlTreeModel *q;
};

}  // End namespace Internal
}  // End namespace DrugInfos


QAbstractXmlTreeModel::QAbstractXmlTreeModel(const QString &fileName, const QString &mainTag, QObject *parent) :
        QAbstractItemModel(parent), dqaxtm(0)
{
    setObjectName("QAbstractXmlTreeModel");
    dqaxtm = new Internal::QAbstractXmlTreeModelPrivate(fileName, mainTag, this);
}

QAbstractXmlTreeModel::~QAbstractXmlTreeModel()
{
    if (dqaxtm)
        delete dqaxtm;
    dqaxtm = 0;
}

bool QAbstractXmlTreeModel::setSubMainTag(const QString &childToMainTag)
{
    if (childToMainTag.isEmpty())
        return false;
    beginResetModel();
    dqaxtm->m_RootNode = dqaxtm->m_RootNode.firstChildElement(childToMainTag);
    delete  dqaxtm->m_RootItem;
    dqaxtm->m_RootItem = new DomItem(dqaxtm->m_RootNode, 0);
    endResetModel();
    return true;
}

bool QAbstractXmlTreeModel::saveModel()
{
    QFile file(dqaxtm->m_FileName);
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write(dqaxtm->domDocument.toString(2).toUtf8());
        file.close();
        return true;
    }
    return false;
}

QModelIndex QAbstractXmlTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DomItem *parentItem;

    if (!parent.isValid())
        parentItem = dqaxtm->m_RootItem;
    else
        parentItem = static_cast<DomItem*>(parent.internalPointer());

    DomItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QAbstractXmlTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    DomItem *childItem = static_cast<DomItem*>(child.internalPointer());
    DomItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == dqaxtm->m_RootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QAbstractXmlTreeModel::rowCount(const QModelIndex &parent) const
{
    DomItem *item = 0;

    if (!parent.isValid())
        item = dqaxtm->m_RootItem;
    else
        item = static_cast<DomItem*>(parent.internalPointer());

    return item->node().childNodes().count();
}


//bool QAbstractXmlTreeModel::canFetchMore(const QModelIndex &parent) const
//{
//    int nbItemRows = 0;
//    DomItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = static_cast<DomItem*>(parent.internalPointer());

//    if (parentItem)
//        nbItemRows = parentItem->node().childNodes().count();

////        qWarning() << Q_FUNC_INFO << (d->m_FetchedRows < nbItemRows) << parent << nbItemRows << d->m_FetchedRows;

//    return (d->m_FetchedRows < nbItemRows);
//}

//void QAbstractXmlTreeModel::fetchMore(const QModelIndex &parent)
//{
//    int nbItemRows = 0;
//    DomItem *parentItem = 0;
//    if (!parent.isValid())
//        parentItem = d->m_RootItem;
//    else
//        parentItem = static_cast<DomItem*>(parent.internalPointer());

//    if (parentItem)
//        nbItemRows = parentItem->node().childNodes().count();

//    int remainder = nbItemRows - d->m_FetchedRows;
//    int itemsToFetch = qMin(MODEL_PREFETCH, remainder);

////    qWarning() << Q_FUNC_INFO << parent << d->m_FetchedRows << itemsToFetch;

//    beginInsertRows(parent, d->m_FetchedRows, d->m_FetchedRows + itemsToFetch);
//    d->m_FetchedRows += itemsToFetch;
//    endInsertRows();
//}

QDomNode QAbstractXmlTreeModel::node(const QModelIndex &index) const
{
    if (!index.isValid())
        return dqaxtm->m_RootNode;

    DomItem *domItem = static_cast<DomItem*>(index.internalPointer());

    if (!domItem)
        return dqaxtm->m_RootNode;

    return domItem->node();
}

