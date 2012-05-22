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
#ifndef STRINGLISTVIEW_H
#define STRINGLISTVIEW_H

#include <listviewplugin/listview_exporter.h>
#include <listviewplugin/listview.h>

#include <QObject>

/**
 * \file stringlistview.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.1
 * \date 06 Apr 2012
*/

namespace Views {

class LISTVIEW_EXPORT StringListView : public ListView
{
    Q_OBJECT
    Q_PROPERTY(QVariant stringList         READ getStringList         WRITE setStringList        USER true)
    Q_PROPERTY(QVariant checkedStringList  READ getCheckedStringList  WRITE setCheckedStringList USER true)

public:
    StringListView(QWidget * parent = 0);
    ~StringListView();

    QVariant getStringList() const;
    void setStringList(const QVariant &list);

    void setItemsCheckable(bool state);
    QVariant getCheckedStringList() const;
    void setCheckedStringList(const QVariant &list);

Q_SIGNALS:
    void stringListChanged();

};

}  // End Views

#endif // STRINGLISTVIEW_H
