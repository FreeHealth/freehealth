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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERRIGHTSWIDGET_H
#define USERRIGHTSWIDGET_H

#include <usermanagerplugin/constants.h>

#include <QListView>
#include <QAbstractListModel>
class QEvent;

#include <QDebug>

/**
 * \file userrightswidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 16 June 2011
*/

namespace UserPlugin {
namespace Internal {

class UserRightsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    UserRightsModel(QObject *parent);
    ~UserRightsModel() {}

    void setRights(const int r)     { m_Rights = r; }
    int  getRights()                { return m_Rights; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void retranslate();

private:
    QStringList m_RightsName;
    QHash<int, int> m_NameToRole;
    int m_Rights;
};

class UserRightsWidget : public QListView
{
    Q_OBJECT
    Q_PROPERTY(int rights    READ getRights    WRITE setRights    USER true)
public:
    UserRightsWidget(QWidget * parent = 0);

    void setRights(const int r)     { m_Model->setRights(r); }
    int  getRights()                { return m_Model->getRights(); }

private:
    void changeEvent(QEvent *e);

private:
    UserRightsModel *m_Model;
};

}  // End Internal
}  // End UserPlugin

#endif // USERRIGHTSWIDGET_H
