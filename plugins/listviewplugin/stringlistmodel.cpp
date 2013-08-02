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

/**
  \class Views::StringListModel
  * \brief Provides a stringlist model to use with tkStringListView.
  * You can set items checkable and/or string editable with constructor or with setCheckable(), setStringEditable().
  * There are functions to moveUp() moveDown() items.
  * You can set/get checkedItems with getCheckedItems() and setCheckedItems().
  * You can set/get all strings using setStringList().
*/

#include "stringlistmodel.h"

#include <QStringList>
#include <QList>

#include <QDebug>

namespace Views {
namespace Internal {

class StringListModelPrivate
{
public:
    struct Data {
        QString str;
        int     checked;
    };

    bool         m_Checkable;
    bool         m_StringEditable;
    QList<Data>  m_StringList;
};

}  // End Internal
}  // End Views

using namespace Views;
using namespace Views::Internal;

StringListModel::StringListModel(QObject *parent,
                                 const bool stringEditable,
                                 const bool checkStateEditable) :
    QAbstractListModel(parent),
    d(new StringListModelPrivate)
{
    setObjectName("StringListModel");
    d->m_Checkable = checkStateEditable;
    d->m_StringEditable = stringEditable;
}

StringListModel::~StringListModel()
{
    if (d)
        delete d;
    d=0;
}

void StringListModel::setReadOnly(bool state)
{
    setStringEditable(!state);
}

void StringListModel::setCheckable(bool state)
{
    beginResetModel();
    d->m_Checkable = state;
    endResetModel();
}

bool StringListModel::isCheckable() const
{
    return d->m_Checkable;
}
void StringListModel::setStringEditable(bool state)
{
    beginResetModel();
    d->m_StringEditable = state;
    endResetModel();
}

void StringListModel::setStringList(const QStringList &strings)
{
    beginResetModel();
    d->m_StringList.clear();
    foreach( const QString &s, strings) {
        StringListModelPrivate::Data dt;
        dt.str = s;
        dt.checked = Qt::Unchecked;
        d->m_StringList << dt;
    }
    endResetModel();
}

QStringList StringListModel::getStringList() const
{
    QStringList list;
    foreach( const StringListModelPrivate::Data &dt, d->m_StringList )
        list << dt.str;
    return list;
}

QStringList StringListModel::getCheckedItems() const
{
    QStringList list;
    foreach( const StringListModelPrivate::Data & dt, d->m_StringList )
        if ( dt.checked )
            list << dt.str;
    return list;
}

void StringListModel::setCheckedItems(const QStringList &list)
{
    beginResetModel();
    QList<StringListModelPrivate::Data>::iterator i;
    for (i = d->m_StringList.begin(); i != d->m_StringList.end(); ++i) {
       if ( list.indexOf((*i).str) != -1)
           (*i).checked = Qt::Checked;
       else
           (*i).checked = Qt::Unchecked;
    }
    endResetModel();
}

int StringListModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid())
        return 0;

    return d->m_StringList.count();
}

Qt::ItemFlags StringListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (d->m_Checkable)
        f |= Qt::ItemIsUserCheckable;
    if (d->m_StringEditable)
        f |= Qt::ItemIsEditable;
    return f;
}

bool StringListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( ! index.isValid() )
        return false;
    if ( index.row() > rowCount() )
        return false;

    if ((role == Qt::EditRole) && (d->m_StringEditable)) {
        d->m_StringList[index.row()].str = value.toString();
        Q_EMIT dataChanged(index, index);
    } else if (role == Qt::CheckStateRole && (d->m_Checkable)) {
        d->m_StringList[index.row()].checked = value.toInt();
        Q_EMIT dataChanged(index, index);
    }
    return true;
}

QVariant StringListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() > rowCount())
        return QVariant();

    if (role == Qt::CheckStateRole && d->m_Checkable) {
        return d->m_StringList.at(index.row()).checked;
    } else if (role == Qt::DisplayRole) {
        return d->m_StringList.at(index.row()).str;
    } else if (role == Qt::EditRole) {
        if (d->m_StringEditable)
            return d->m_StringList.at(index.row()).str;
    }
    return QVariant();
}

bool StringListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent,row,row+count);
    int i;
    for(i=0; i<count;++i) {
        StringListModelPrivate::Data data;
        data.str = QString();
        data.checked = Qt::Unchecked;
        d->m_StringList.insert(i+row,data);
    }
    endInsertRows();
    return true;
}

bool StringListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent,row,row+count);
    int i;
    for(i=0; i<count;++i) {
        if ((i+row) < d->m_StringList.count())
            d->m_StringList.removeAt(i+row);
    }
    endRemoveRows();
    return true;
}

bool StringListModel::moveUp(const QModelIndex &item)
{
    if ( !item.isValid() )
        return false;

    if (item.row() >= 1) {
        beginResetModel();
        d->m_StringList.move( item.row(), item.row()-1);
        endResetModel();
        return true;
    }
    return false;
}

bool StringListModel::moveDown(const QModelIndex &item)
{
    if ( !item.isValid() )
        return false;

    if (item.row() < (rowCount()-1)) {
        beginResetModel();
        d->m_StringList.move( item.row(), item.row()+1);
        endResetModel();
        return true;
    }
    return false;
}

