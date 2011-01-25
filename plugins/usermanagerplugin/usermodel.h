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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERMODEL_H
#define USERMODEL_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <usermanagerplugin/global.h>
#include <usermanagerplugin/constants.h>

#include <QSqlTableModel>
#include <QAbstractListModel>
#include <QHash>
#include <QStringList>


/**
 * \file usermodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 16 June 2010
*/

namespace Print {
class PrinterPreviewer;
class TextDocumentExtra;
}


namespace UserPlugin {
namespace Internal {
class UserModelPrivate;
}  // End Internal


class USER_EXPORT UserModel : public QSqlTableModel
{
    Q_OBJECT
protected:
    UserModel(QObject *parent);

public:
    static UserModel *instance(QObject *parent = 0);
    ~UserModel();

    bool isCorrectLogin(const QString &logbase64, const QString &cryptpassbase64);

    bool setCurrentUser(const QString &log64, const QString &cryptpass64);
    bool hasCurrentUser();
    QModelIndex currentUserIndex() const;
    QVariant currentUserData(const int column) const { return index(currentUserIndex().row(), column).data(); }

    bool hasUserToSave();

    virtual QSqlDatabase database() const;

    virtual void clear();

    virtual int columnCount(const QModelIndex & = QModelIndex());

    virtual QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool setPaper(const QString &uuid, const int ref, Print::TextDocumentExtra *extra);

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual void revertRow(int row);

    virtual void setSort(int column, Qt::SortOrder order);
    virtual void sort(int column, Qt::SortOrder order);
    virtual void setTable(const QString &tableName);
    virtual void setFilter(const QHash<int,QString> &conditions);
    virtual void setFilter(const QString &filter);

    QString tableName() const;

    QModelIndex index(int row, int column, const QModelIndex & = QModelIndex()) const
    { return createIndex(row, column); }

    QList<int> practionnerLkIds(const QString &uid);
    int practionnerLkId(const QString &uid);

    int numberOfUsersInMemory();

    void warn();

public Q_SLOTS:
    virtual bool submitAll();
    virtual bool submitUser(const QString &uuid);
    virtual bool submitRow(const int row);
    virtual bool revertAll();

Q_SIGNALS:
    void memoryUsageChanged() const;
    void userAboutToConnect(const QString &uuid) const;
    void userConnected(const QString &uuid) const;
    void userAboutToDisconnect(const QString &uuid) const;
    void userDisconnected(const QString &uuid) const;
    void userDocumentsChanged() const;

private:
    QModelIndex createIndex(int row, int column, void * ptr = 0) const;
    QModelIndex createIndex(int row, int column, quint32 id) const;

private:
    Internal::UserModelPrivate *d;
    static UserModel *m_Instance;
};

}  // End UserPlugin


#endif // End USERMODEL_H
