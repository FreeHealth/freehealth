/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef UTILS_DATABASE_H
#define UTILS_DATABASE_H

#include <utils/global_exporter.h>

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariant>
#include <QObject>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file database.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 23 Fev 2010
*/
namespace Utils {

namespace Internal {
class DatabasePrivate;
}


class UTILS_EXPORT Database
{
public:

    /** \brief Describe the desired type of database access (read only, read and write...). */
    enum TypeOfAccess
    {
        ReadOnly = 0,
        ReadWrite
    };

    /**
       \brief Simpified Enum for database driver selection.
       Actually, only SQLite is take into account.
    */
    enum AvailableDrivers
    {
        SQLite,   /*!< \brief Uses SQLite driver. */
        MySQL,    /*!< \brief Uses MySQL driver. */
        PostSQL   /*!< \brief Uses PostGre driver. */
    };

    /**
       \brief Specifies what to do when database does not exists.
       \sa createConnection(), createDatabase
    */
    enum CreationOption  // if database doesnot exist :
    {
        WarnOnly,        /*!< \brief Only warn user, don't try to create the database. */
        CreateDatabase   /*!< \brief Don't warn user, try to create automatically the database. \sa createDatabase()*/
    };

    /**
      \brief Determine the type of the field into the Database scheme .
      \sa addField()
    */
    enum TypeOfField
    {
        FieldUndefined = 0,
        FieldIsInteger,
        FieldIsLongInteger,
        FieldIsLongText,
        FieldIsShortText,
        FieldIsLanguageText,
        FieldIsDate,
        FieldIsBlob,
        FieldIsUUID,
        FieldIsBoolean,
        FieldIsUniquePrimaryKey,
        FieldIsReal
    };

    /** \brief Describe the grants on database/server (mainly used for MySQL connections). */
    enum Grant {
        Grant_Select           = 0x00001,
        Grant_Update           = 0x00002,
        Grant_Insert           = 0x00004,
        Grant_Delete           = 0x00008,
        Grant_Create           = 0x00010,
        Grant_Drop             = 0x00020,
        Grant_Index            = 0x00040,
        Grant_Alter            = 0x00080,
        Grant_CreateTmpTables  = 0x00100,
        Grant_LockTables       = 0x00200,
        Grant_Execute          = 0x00400,
        Grant_CreateView       = 0x00800,
        Grant_ShowView         = 0x01000,
        Grant_CreateRoutine    = 0x02000,
        Grant_AlterRoutine     = 0x04000,
        Grant_CreateUser       = 0x08000,
        Grant_Options          = 0x10000,
        Grant_Process          = 0x20000,
        Grant_Trigger          = 0x40000,
        Grant_ShowDatabases    = 0x80000,
        Grant_All              = 0xFFFFF
    };
    Q_DECLARE_FLAGS(Grants, Grant);

    Database();
    virtual ~Database();

    static void logAvailableDrivers();

    // connection
    virtual bool createConnection( const QString & connectionName, const QString & dbName,
                                   const QString & pathOrHostName,
                                   TypeOfAccess access = ReadWrite, AvailableDrivers driver = SQLite,
                                   const QString & login = QString::null, const QString & pass = QString::null,
                                   const int port = 0,
                                   CreationOption createOption = WarnOnly );

    /**
        \brief This member is called by createConnection() if the asked database does not exists.
               By default it does nothing and return an error state (false).
    */
    virtual bool createDatabase(  const QString & /*connectionName*/ , const QString & /*dbName*/,
                                  const QString & /*pathOrHostName*/,
                                  TypeOfAccess /*access*/, AvailableDrivers /*driver*/,
                                  const QString & /*login*/, const QString & /*pass*/,
                                  const int /*port*/,
                                  CreationOption /*createOption*/
                                  ) { return false; }

    virtual QSqlDatabase database() const;
    virtual QString connectionName() const;

    Grants grants(const QString &connectionName) const;


    // manage database scheme (use enums for the int references)
    virtual int addTable( const int & ref, const QString & name );

    virtual int addField( const int & tableref, const int & fieldref, const QString & name, TypeOfField type = FieldUndefined, const QString & defaultValue = "NULL" );

    virtual bool checkDatabaseScheme();

    virtual QString field( const int & tableref, const int & fieldref) const;
    virtual QStringList fields( const int & tableref ) const;
    virtual QString table( const int & tableref ) const;
    virtual QStringList tables() const;

    virtual QString getWhereClause( const int & tableref, const QHash<int, QString> & conditions ) const;
    virtual QString select( const int & tableref, const int & fieldref, const QHash<int, QString> & conditions ) const;
    virtual QString select( const int & tableref, const QList<int> &fieldsref, const QHash<int, QString> & conditions ) const;
    virtual QString select( const int & tableref, const QList<int> &fieldsref ) const;
    virtual QString select( const int & tableref, const QHash<int, QString> & conditions ) const;
    virtual QString select( const int & tableref ) const;
    virtual QString selectDistinct( const int & tableref, const int & fieldref, const QHash<int, QString> &conditions ) const;

    virtual QString prepareInsertQuery( const int & tableref ) const;

    virtual QString prepareUpdateQuery( const int & tableref, int fieldref, const QHash<int, QString> &conditions);
    virtual QString prepareUpdateQuery( const int & tableref, int fieldref );
    virtual QString prepareUpdateQuery( const int & tableref, const QHash<int, QString> &conditions );
    virtual QString prepareUpdateQuery( const int & tableref );

    virtual QString prepareDeleteQuery( const int tableref, const QHash<int,QString> & conditions );

    virtual bool createTable( const int & tableref ) const;
    virtual bool createTables() const;

    virtual QString total(const int tableRef, const int fieldRef, const QHash<int, QString> &where) const;
    virtual QString total(const int tableRef, const int fieldRef) const;

    virtual bool executeSQL( const QStringList & list, const QSqlDatabase & DB ) const;
    virtual bool executeSQL( const QString & req, const QSqlDatabase & DB ) const;

    virtual void warn() const;
    virtual void toTreeWidget(QTreeWidget *tree);

    virtual void setConnectionName( const QString & c );

protected:
    virtual void setDriver(const AvailableDrivers &d);

private:
    Internal::DatabasePrivate *d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Utils::Database::Grants);

#endif // UTILS_DATABASE_H
