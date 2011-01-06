/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright(C) 2008-2009 by Eric MAEKER                                *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
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
