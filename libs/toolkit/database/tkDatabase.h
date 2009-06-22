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
#ifndef TKDATABASE_H
#define TKDATABASE_H
class tkDatabasePrivate;

// include toolkit headers
#include <tkExporter.h>

// include Qt headers
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariant>
#include <QObject>

/**
 * \file tkDatabase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.3
 * \date 27 Feb 2009
*/

class Q_TK_EXPORT tkDatabase : public QObject
{
    Q_OBJECT
public:

    /** */
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

    /** \brief Constructor */
    tkDatabase( QObject * parent = 0 );
    /** \brief Destructor */
    virtual ~tkDatabase();

    // connection
    virtual bool createConnection( const QString & connectionName, const QString & dbName,
                                   const QString & pathOrHostName,
                                   TypeOfAccess access = ReadWrite, AvailableDrivers driver = SQLite,
                                   const QString & login = QString::null, const QString & pass = QString::null,
                                   CreationOption createOption = WarnOnly );

    /**
        \brief This member is called by createConnection() if the asked database does not exists.
               By default it does nothing and return an error state (false).
    */
    virtual bool createDatabase(  const QString & /*connectionName*/ , const QString & /*dbName*/,
                                  const QString & /*pathOrHostName*/,
                                  TypeOfAccess /*access*/, AvailableDrivers /*driver*/,
                                  const QString & /*login*/, const QString & /*pass*/,
                                  CreationOption /*createOption*/
                                  ) { return false; }

    virtual QSqlDatabase database() const;
    virtual QString connectionName() const;


    // manage database scheme (use enums for the int references)
    virtual int addTable( const int & ref, const QString & name );

    virtual int addField( const int & tableref, const int & fieldref, const QString & name, TypeOfField type = FieldUndefined, const QString & defaultValue = "NULL" );

    virtual bool checkDatabaseScheme();

    /** \brief Get field \e fieldref from table \e tableref. */
    virtual QString field( const int & tableref, const int & fieldref) const;
    /** \brief Get all fields from table \e tableref. */
    virtual QStringList fields( const int & tableref ) const;
    /** \brief Get table \e tableref from database scheme. */
    virtual QString table( const int & tableref ) const;
    /** \brief Get all tables from the database scheme. */
    virtual QStringList tables() const;

    virtual QString getWhereClause( const int & tableref, const QHash<int, QString> & conditions ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString select( const int & tableref, const int & fieldref, const QHash<int, QString> & conditions ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString select( const int & tableref, const QList<int> &fieldsref, const QHash<int, QString> & conditions ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString select( const int & tableref, const QList<int> &fieldsref ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString select( const int & tableref, const QHash<int, QString> & conditions ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString select( const int & tableref ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString selectDistinct( const int & tableref, const int & fieldref, const QHash<int, QString> & conditions ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString prepareInsertQuery( const int & tableref ) const;

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString prepareUpdateQuery( const int & tableref, int fieldref, QHash<int, QString> conditions );
    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString prepareUpdateQuery( const int & tableref, QHash<int, QString> conditions );

    /** \brief return the SQL query according to the driver selected when constructing tkDatabase object. */
    virtual QString prepareDeleteQuery( const int tableref, const QHash<int,QString> & conditions );

    /** \brief Create the table \e tableref into the database. Return error state. */
    virtual bool createTable( const int & tableref ) const;
    /** \brief Create all the tables into the database.  Return error state. */
    virtual bool createTables() const;

    /** \brief execute many SQL queries. */
    virtual bool executeSQL( const QStringList & list, const QSqlDatabase & DB ) const;
    /** \brief execute a SQL query. */
    virtual bool executeSQL( const QString & req, const QSqlDatabase & DB ) const;

    virtual void warn() const;

protected:
    virtual void setConnectionName( const QString & c );

private:
    tkDatabasePrivate *d;
};

#endif // TKDATABASESCHEMA_H
