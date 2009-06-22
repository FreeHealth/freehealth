/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKUSERMODEL_H
#define TKUSERMODEL_H

class tkTextDocumentExtra;

#include <tkUserExporter.h>
#include <tkUserGlobal.h>
#include <tkUserConstants.h>

#include <QSqlTableModel>
#include <QAbstractListModel>
#include <QHash>
#include <QStringList>

class tkUserModelPrivate;

using namespace tkUserConstants;

/**
 * \file tkUserModel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.11
 * \date 08 March 2009
*/

/**
  \brief Users are represented into a table model. Each row represents a user, each column a value.
  \sa User, tkUserConstants
  \todo write documentation+++
  \todo code LOCKER
  \todo when QDataWidgetMapper (tkUserViewer) is setted, it calls ALL the datas of the user, even for the hidden widgets. This causes an important memory usage. This is to improve ++++
  Set filter with setFilter().
  \ingroup usertoolkit object_usertoolkit
  \ingroup usermanager
*/
class Q_TKUSER_EXPORT tkUserModel : public QSqlTableModel
{
    Q_OBJECT
    friend void initLib();

protected:
    tkUserModel( QObject * parent );

public:
    static tkUserModel * instance( QObject *parent = 0 );
    ~tkUserModel();

    bool isCorrectLogin( const QString & logbase64, const QString & cryptpassbase64 );

    bool setCurrentUser( const QString & log64, const QString & cryptpass64 );
    bool hasCurrentUser();
    QModelIndex currentUserIndex() const;
    QVariant currentUserData( const int column ) const { return index( currentUserIndex().row(), column ).data(); }

    bool hasUserToSave();

    virtual QSqlDatabase database() const;

    virtual int columnCount( const QModelIndex & = QModelIndex() ) { return User::UserMaxParam; }
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual QVariant data( const QModelIndex & item, int role = Qt::DisplayRole ) const;
    virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex() );
    virtual void revertRow( int row );

    virtual void setSort( int column, Qt::SortOrder order );
    virtual void sort( int column, Qt::SortOrder order );
    virtual void setTable ( const QString & tableName );
    virtual void setFilter ( const QHash<int,QString> & conditions );
    virtual void setFilter ( const QString & filter );

    QString tableName() const;

    QModelIndex index( int row, int column, const QModelIndex & = QModelIndex() ) const
    { return createIndex( row, column ); }

    QStringList titles() const
    { return QStringList() << tr("Mister") << tr("Miss") << tr("Misses") << tr("Child") << tr("Doctor") << tr("Professor") << tr("Director"); }
    QStringList genders() const
    { return QStringList() << tr("F") << tr("M"); }

    int numberOfUsersInMemory();

    tkTextDocumentExtra *currentUserDocument( const int column );
    void setCurrentUserDocument( tkTextDocumentExtra *extra, const int column );

    void warn();

public Q_SLOTS:
    virtual bool submitAll();
    virtual bool submitUser( const QString & uuid );
    virtual bool submitRow( const int row ) { return submitUser( index(row,User::UserUuid).data().toString() ); }
    virtual bool revertAll();

Q_SIGNALS:
    void memoryUsageChanged() const;

private:
    QModelIndex createIndex ( int row, int column, void * ptr = 0 ) const;
    QModelIndex createIndex ( int row, int column, quint32 id ) const;

private:
    tkUserModelPrivate * d;
    static tkUserModel * m_Instance;
};

#endif // TKUSERMODEL_H
