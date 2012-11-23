/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TKSTRINGLISTMODEL_H
#define TKSTRINGLISTMODEL_H

#include <listviewplugin/listview_exporter.h>

#include <QAbstractListModel>
#include <QObject>
class QStringList;

/**
 * \file tkStringListModel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.4
 * \date 25 April 2009
*/

namespace Views {
namespace Internal {
class StringListModelPrivate;
}

class LISTVIEW_EXPORT StringListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    StringListModel( QObject *parent = 0,
                       const bool stringEditable = false,
                       const bool checkStateEditable = true );
    ~StringListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual QVariant data( const QModelIndex & index, int role ) const;
    virtual bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

    void setReadOnly(bool state);
    void setCheckable( bool state );
    void setStringEditable( bool state );
    void setStringList( const QStringList & strings );
    QStringList getStringList() const;

    QStringList getCheckedItems() const;
    void setCheckedItems( const QStringList & list );

    bool moveUp( const QModelIndex & item );
    bool moveDown( const QModelIndex & item );

private:
    Internal::StringListModelPrivate *d;
};

}  // End Views

#endif // TKSTRINGLISTMODEL_H
