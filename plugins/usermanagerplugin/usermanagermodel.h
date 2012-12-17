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
 *   Main developers : Eric Maeker                                         *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERPLUGIN_INTERNAL_USERMANAGERMODEL_H
#define USERPLUGIN_INTERNAL_USERMANAGERMODEL_H

#include <QStandardItemModel>

/**
 * \file usermanagermodel.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 16 Dec 2012
*/

namespace UserPlugin {
class IUserViewerPage;

namespace Internal {
class UserManagerModelFilter {
public:
    UserManagerModelFilter(const QString &birthName = QString::null, const QString &firstName = QString::null) :
        _birth(birthName), _first(firstName)
    {}

    QString _birth, _first;
};

class UserManagerModelPrivate;

class UserManagerModel : public QStandardItemModel
{
    Q_OBJECT

    enum UserDataRepresentation {
        Name = 0,
        SecondName,
        FirstName,
        Uuid,
        Title,
        LastLogin,
        Gender,

        NumberOfColumns
    };

public:
    explicit UserManagerModel(QObject *parent = 0);
    ~UserManagerModel();
    bool initialize();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setFilter(const UserManagerModelFilter &filter);

    int pageIndexFromIndex(const QModelIndex &index) const;
    QList<IUserViewerPage *> pages() const;
    QString userUuid(const QModelIndex &index) const;
    int genderIndex(const QModelIndex &index) const;
    QString lastLogin(const QModelIndex &index) const;
    QString title(const QModelIndex &index) const;

Q_SIGNALS:
    
private Q_SLOTS:
    void pluginManagerObjectAdded(QObject *o);
    void pluginManagerObjectRemoved(QObject *o);

private:
    Internal::UserManagerModelPrivate *d;
};

} // namespace Internal
} // namespace UserPlugin

#endif // USERPLUGIN_INTERNAL_USERMANAGERMODEL_H

