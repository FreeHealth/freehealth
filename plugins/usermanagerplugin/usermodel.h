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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERMODEL_H
#define USERMODEL_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <usermanagerplugin/constants.h>

#include <coreplugin/iuser.h>

#include <QAbstractTableModel>
#include <QAbstractListModel>
#include <QHash>
#include <QStringList>

/**
 * \file usermodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 15 May 2011
*/

namespace Print {
class PrinterPreviewer;
class TextDocumentExtra;
}

namespace UserPlugin {
class UserModel;
class UserCore;
class UserViewer;

namespace Internal {
class UserModelPrivate;
class UserManagerPlugin; // for tests
}  // End Internal

class USER_EXPORT UserModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class UserPlugin::UserCore;
    friend class UserPlugin::UserViewer;
#ifdef WITH_TESTS
    friend class UserPlugin::Internal::UserManagerPlugin;
#endif

protected:
    UserModel(QObject *parent);
    bool initialize();

public:
    ~UserModel();

    bool isCorrectLogin(const QString &clearLog, const QString &clearPassword);

    bool setCurrentUser(const QString &clearLog, const QString &clearPassword, bool refreshCache = false, bool checkPrefValidity = true);
    bool setCurrentUserIsServerManager();

    bool hasCurrentUser() const;
    QModelIndex currentUserIndex() const;

    bool isDirty() const;

    virtual void forceReset();
    virtual void clear();
    virtual void refresh();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant currentUserData(const int column) const;
    virtual QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool setPaper(const QString &uuid, const int ref, Print::TextDocumentExtra *extra);
    virtual Print::TextDocumentExtra *paper(const int row, const int ref);

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual void revertRow(int row);

    virtual void setSort(int column, Qt::SortOrder order);
    virtual void sort(int column, Qt::SortOrder order);
    virtual void setTable(const QString &tableName);
    virtual void setFilter(const QHash<int,QString> &conditions);
    virtual void setFilter(const QString &filter);

    QString tableName() const;

    static QHash<QString, QString> getUserNames(const QStringList &uid);
    bool createVirtualUsers(const int count);

    int numberOfUsersInMemory() const;

public Q_SLOTS:
    virtual bool submitAll();
    virtual bool submitUser(const QString &uuid);
    virtual bool submitRow(const int row);
    virtual bool revertAll();

Q_SIGNALS:
    void userAboutToConnect(const QString &uuid);
    void userConnected(const QString &uuid);
    void userAboutToDisconnect(const QString &uuid);
    void userDisconnected(const QString &uuid);
    void userDocumentsChanged();

protected:
    void emitUserConnected();
    void checkUserPreferencesValidity();

public Q_SLOTS:
    void updateUserPreferences();

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    Internal::UserModelPrivate *d;
};

}  // End UserPlugin


#endif // End USERMODEL_H
