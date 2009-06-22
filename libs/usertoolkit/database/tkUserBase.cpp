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
#include "tkUserBase.h"

// include toolkit headers
#include <tkLog.h>
#include <tkSettings.h>
#include <tkGlobal.h>
#include <tkTextDocumentExtra.h>

// include usertoolkit headers
#include <tkUser.h>

// include Qt headers
#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDateTime>
#include <QUuid>

Q_TKUSER_USING_CONSTANTS

// Initializing static datas
bool tkUserBase::m_initialized      = false;
QString  tkUserBase::m_LastUuid = "";
QString  tkUserBase::m_LastLogin = "";
QString  tkUserBase::m_LastPass = "";
tkUserBase* tkUserBase::m_Instance = 0;

/**
  \brief Returns the unique instance of tkUserBase. If the instance does not exist it is created.
  You should never construct a instance of this object using the constructor.
*/
tkUserBase * tkUserBase::instance()
{
    if (!m_Instance)
        m_Instance = new tkUserBase(qApp);
    return m_Instance;
}

/**
  \brief Constructor, inform tkDatabase of the database scheme.
*/
tkUserBase::tkUserBase( QObject * parent )
        : tkDatabase( parent )
{
    setObjectName( "tkUserBase" );
    m_initialized = false;
    // populate tables and fields of database
    addTable( Table_USERS,  "USERS" );
    addTable( Table_DATAS,  "DATAS" );
    addTable( Table_RIGHTS, "RIGHTS" );

    addField( Table_USERS, USER_ID,           "USER_ID",         FieldIsUniquePrimaryKey );
    addField( Table_USERS, USER_UUID,         "USER_UUID",       FieldIsUUID );
    addField( Table_USERS, USER_VALIDITY,     "VALIDITY",        FieldIsBoolean );
    addField( Table_USERS, USER_LOGIN,        "LOGIN",           FieldIsShortText );
    addField( Table_USERS, USER_PASSWORD,     "PASSWORD",        FieldIsShortText );
    addField( Table_USERS, USER_LASTLOG,      "LASTLOGIN",       FieldIsDate );
    addField( Table_USERS, USER_NAME,         "NAME",            FieldIsShortText );
    addField( Table_USERS, USER_SECONDNAME,   "SECONDNAME",      FieldIsShortText );
    addField( Table_USERS, USER_SURNAME,      "SURNAME",         FieldIsShortText );
    addField( Table_USERS, USER_MAIL,         "MAIL",            FieldIsShortText );
    addField( Table_USERS, USER_LANGUAGE,     "LANGUAGE",        FieldIsLanguageText );
    addField( Table_USERS, USER_LOCKER,       "LOCKER",          FieldIsBoolean );

    addField( Table_DATAS, DATAS_ID,          "DATAS_ID",        FieldIsUniquePrimaryKey );
    addField( Table_DATAS, DATAS_USER_UUID,   "USER_UUID",       FieldIsUUID );
    addField( Table_DATAS, DATAS_DATANAME,    "DATANAME",        FieldIsShortText );
    addField( Table_DATAS, DATAS_STRING,      "DATA_STRING",     FieldIsShortText );
    addField( Table_DATAS, DATAS_LONGSTRING,  "DATA_LONGSTRING", FieldIsLongText );
    addField( Table_DATAS, DATAS_FILE,        "DATA_FILE",       FieldIsBlob );
    addField( Table_DATAS, DATAS_NUMERIC,     "DATA_NUMERIC",    FieldIsInteger);
    addField( Table_DATAS, DATAS_DATE,        "DATA_DATE",       FieldIsDate );
    addField( Table_DATAS, DATAS_LANGUAGE,    "DATA_LANGUAGE",   FieldIsShortText );
    addField( Table_DATAS, DATAS_LASTCHANGE,  "LASTCHANGE",      FieldIsDate );
    addField( Table_DATAS, DATAS_TRACE_ID,    "TRACE_ID",        FieldIsInteger );

    addField( Table_RIGHTS, RIGHTS_ID,        "RIGHTS_ID",       FieldIsUniquePrimaryKey );
    addField( Table_RIGHTS, RIGHTS_USER_UUID, "USER_UUID",       FieldIsUUID );
    addField( Table_RIGHTS, RIGHTS_ROLE,      "RIGHTS_ROLE",     FieldIsShortText );
    addField( Table_RIGHTS, RIGHTS_RIGHTS,    "RIGHTS_RIGHTS",   FieldIsInteger );

    initialize( tkSettings::instance() );
}

/**
  \brief Initialize users base using the \e settings values.
  \sa tkSettings, tkSettings::ReadWriteDatabasesPath
*/
bool tkUserBase::initialize( tkSettings * settings )
{
    Q_ASSERT_X( settings, "tkUserBase", "initialize : you must specify a tkSettings pointer" );
    if ( ! settings )
        return false;

    if ( m_initialized )
        return true;

    // retreive databases path (tkSettings manages OS specific paths and debug compilation)
    QString pathToDb = settings->databasePath();
    pathToDb = QDir::cleanPath( pathToDb + "/users" );

    // test connection (create DB if not exists)
    if ( ! createConnection( USER_DB_CONNECTION, QString("%1.db").arg( USER_DB_CONNECTION ),
                             pathToDb, ReadWrite, SQLite, "log", "pass", CreateDatabase ) )
        return false;
    m_initialized = true;
    return true;
}


//--------------------------------------------------------------------------------------------------------
//------------------------------------------- Datas retreivers -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Retreive all users datas from the users' database. If an error occurs, it returns 0. */
tkUser * tkUserBase::getUser( const QHash<int, QString> & conditions ) const
{
    QSqlDatabase DB = QSqlDatabase::database( USER_DB_CONNECTION );
    if ( !DB.isOpen() )
        DB.open();

    // get USERS table
    QString req = select( Table_USERS, conditions );
    tkUser *toReturn = 0;
    QString uuid = "";
    {
        QSqlQuery q( req , DB );
        if ( q.isActive() ) {
            if ( q.next() ) {
                if ( q.record().count() != USER_MaxParam )
                    tkLog::addError( "tkUserBase", QCoreApplication::translate( "tkUserBase",
                                                                                "ERROR : will retreiving %1. Wrong number of fields" )
                                     .arg( USER_DB_CONNECTION ) );
                int i = 0;
                toReturn = new tkUser();
                for ( i = 0; i < USER_MaxParam; ++i )
                    toReturn->setValue( Table_USERS, i , q.value( i ) );
                uuid = q.value( USER_UUID ).toString();
            }
        }
        else
            tkLog::addQueryError( "tkUserBase", q );
    }

    // get RIGHTS table  ***************************************** -1
    QHash<int, QString> where;
    where.insert( RIGHTS_USER_UUID, QString( "='%1'" ).arg( uuid ) );
    req = select( Table_RIGHTS, where );
    {
        QSqlQuery q( req , DB );
        if ( q.isActive() ) {
            while ( q.next() ) {
                if ( q.record().count() != RIGHTS_MaxParam )
                    tkLog::addError( "tkUserBase", QCoreApplication::translate( "tkUserBase",
                                                                                "ERROR : will retreiving %1. Wrong number of fields" )
                                     .arg( "users' rights" ) );
                int i = 0;
                QByteArray id = q.value( RIGHTS_ROLE ).toByteArray();
                for ( i = 0; i < RIGHTS_MaxParam; ++i )
                    toReturn->addRightsFromDatabase( id , i , q.value( i ) );
            }
        }
        else
            tkLog::addQueryError( "tkUserBase", q );
    }

    // get DATAS table  ***************************************** -1
    where.clear();
    where.insert( DATAS_USER_UUID, QString( "='%1'" ).arg( uuid ) );
    req = select( Table_DATAS, where );
    QList<UserDynamicData*> list;
    {
        QSqlQuery q( req , DB );
        if ( q.isActive() ) {
            while ( q.next() ) {
                if ( q.record().count() != DATAS_MaxParam )
                    tkLog::addError( "tkUserBase", QCoreApplication::translate( "tkUserBase",
                                                                                "ERROR : will retreiving %1. Wrong number of fields" )
                                     .arg( "users' rights" ) );
                int i = 0;
                UserDynamicData *data = new UserDynamicData();
                for ( i = 0; i < DATAS_MaxParam; ++i ) {
                    data->feedFromSql(i, q.value(i));
                }
                list << data;
            }
        }
        else
            tkLog::addQueryError( "tkUserBase", q );
    }
    if (list.count())
        toReturn->addDynamicDatasFromDatabase(list);

    if (toReturn)
        toReturn->setModified(false);
    return toReturn;
}

/**
  \brief Retreive all users datas from the users' database. If an error occurs, it returns 0.
  \sa getUser()
*/
tkUser *tkUserBase::getUserById( const QVariant & _id ) const
{
    // retreive corresponding user
    // construct the where clause
    QString req = QString( "=%1" ).arg( _id.toInt() ) ;
    QHash<int, QString> where;
    where.insert( USER_ID, req );
    // get the asked user
    return getUser( where );
}

/**
  \brief Retreive all users datas from the users' database. If an error occurs, it returns 0.
  \sa getUser()
*/
tkUser * tkUserBase::getUserByUuid( const QString & uuid ) const
{
    // retreive corresponding user
    // construct the where clause
    QString req = QString( "='%1'" ).arg( uuid ) ;
    QHash<int, QString> where;
    where.insert( USER_UUID, req );
    // get the asked user
    return getUser( where );
}


/**
  \brief Retreive all users datas from the users' database. If an error occurs, it returns 0.
  \sa getUser()
*/
tkUser *tkUserBase::getUserByLoginPassword( const QVariant & login, const QVariant & cryptedPassword ) const
{
    // retreive corresponding user
    // construct the where clause
    QHash<int, QString> where;
    where.insert( USER_LOGIN, QString( "='%1'" ).arg( login.toString() ) );
    where.insert( USER_PASSWORD, QString( "='%1'" ).arg( cryptedPassword.toString() ) );
    // get the asked user
    return getUser( where );
}

/**
  \brief Check the couple login/password passed as params.
*/
bool tkUserBase::checkLogin( const QString & login, const QString & cryptedPassword ) const
{
    m_LastUuid.clear();
    m_LastLogin.clear();
    m_LastPass.clear();
    QSqlDatabase DB = QSqlDatabase::database( USER_DB_CONNECTION );
    if ( !DB.isOpen() )
        DB.open();

    // create query
    QList<int> list;
    list << USER_UUID << USER_LOGIN << USER_PASSWORD;
    QHash<int, QString> where;
    where.insert( USER_LOGIN, QString( "='%1'" ).arg( login ) );
    where.insert( USER_PASSWORD, QString( "='%1'" ).arg( cryptedPassword ) );
    QString req = select( Table_USERS, list, where );

    QSqlQuery q( req , DB );
    if ( q.isActive() ) {
        if ( q.next() ){
            m_LastUuid = q.value( 0 ).toInt();
            m_LastLogin = q.value( 0 ).toString();
            m_LastPass = q.value( 0 ).toString();
        }
    }
    else
        tkLog::addQueryError( "tkUserBase", q );

    return ( ! m_LastUuid.isEmpty() );
}

/**
  \brief Return the Uuid of the user identified with couple login/password.
*/
QString tkUserBase::getUuid( const QString & log64, const QString cryptpass64 )
{
    if ( ( log64 == m_LastLogin ) && ( cryptpass64 == m_LastPass ) )
        return m_LastUuid;
    // create query
    QHash<int, QString> where;
    where.insert( USER_LOGIN, QString( "='%1'" ).arg( log64 ) );
    where.insert( USER_PASSWORD, QString( "='%1'" ).arg( cryptpass64 ) );
    QString req = select( Table_USERS, USER_UUID, where );
    QSqlQuery q( req , database() );
    if ( q.isActive() ) {
        if ( q.next() ) {
            m_LastUuid = q.value(0).toString();
        }
    }
    else {
        tkLog::addError("tkUserBase", QApplication::translate("tkUserBase", "Can not create a new user's UUID, database access error"));
        tkLog::addQueryError( "tkUserBase", q );
        m_LastUuid.clear();
    }
    return m_LastUuid;
}

/**
  \brief Return a new Uuid assuming that it is not already used in base.
*/
QString tkUserBase::createNewUuid()
{
    QString tmp;
    while (tmp.isEmpty()) {
        tmp = QUuid::createUuid().toString();
        // create query
        QHash<int, QString> where;
        where.insert( USER_UUID, QString( "='%1'" ).arg( tmp ) );
        QString req = select( Table_USERS, USER_UUID, where );
        QSqlQuery q( req , database() );
        if ( q.isActive() ) {
            if ( q.next() ) {
                tmp.clear();
            }
        }
        else {
            tkLog::addError("tkUserBase", QApplication::translate("tkUserBase", "Can not create a new user's UUID, database access error"));
            tkLog::addQueryError( "tkUserBase", q );
            break;
        }
    }
    return tmp;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------- Datas savers ---------------------------------------------
//--------------------------------------------------------------------------------------------------------
/**
  \brief Create the default users database if it does not exists.
  Actually this function only supports SQLite database.
*/
bool tkUserBase::createDatabase(  const QString & connectionName, const QString & dbName,
                                  const QString & pathOrHostName,
                                  TypeOfAccess access, AvailableDrivers driver,
                                  const QString & login, const QString & pass,
                                  CreationOption createOption
                                  )
{
    Q_UNUSED(access);
    Q_UNUSED(login);
    Q_UNUSED(pass);
    Q_UNUSED(createOption);
    tkLog::addMessage( "tkUserBase", QCoreApplication::translate( "tkUserBase", "Trying to create empty user database. \nLocation : %1 \nFileName: %2" )
                       .arg( pathOrHostName, dbName ) );

    // create an empty database and connect
    QSqlDatabase DB;
    if ( driver == SQLite ) {
        DB = QSqlDatabase::addDatabase( "QSQLITE" , connectionName );
        DB.setDatabaseName( QDir::cleanPath( pathOrHostName + QDir::separator() + dbName ) );
        DB.open();
    } else if ( driver == MySQL ) {
        // TODO : how to create a new mysql database ???
    }

    // create db structure
    // before we need to inform tkDatabase of the connectionName to use
    setConnectionName( connectionName );
    createTables();

    // add general administrator
    tkUser * user = new tkUser( DEFAULT_USER_UUID );
    user->setLogin( DEFAULT_USER_LOGIN );
    user->setCryptedPassword( DEFAULT_USER_PASSWORD );
    user->setValidity( true );
    user->setName( DEFAULT_USER_NAME );
    user->setLanguage( QLocale().name().left( 2 ) );
    user->setSpecialty( QStringList() << DEFAULT_USER_SPECIALTY );

    user->setExtraDocument(new tkTextDocumentExtra(DEFAULT_USER_HEADER), User::UserGenericHeader );
    user->setExtraDocument(new tkTextDocumentExtra(DEFAULT_USER_FOOTER), User::UserGenericFooter );

    user->setAdress(DEFAULT_USER_ADRESS);
    user->setSurname(DEFAULT_USER_SURNAME);
    user->setRights( USER_ROLE_USERMANAGER, User::ReadAll | User::WriteAll | User::Create | User::Delete | User::Print );
    saveUser( user );
    delete user;
    // database is readable/writable
    tkLog::addMessage( "tkUserBase", QCoreApplication::translate( "tkUserBase", "User database created : File %1" ).arg( pathOrHostName + QDir::separator() + dbName ) );

    return true;
}

/**
  \brief Record the last login date for the user identified by couple login/password. The date is returned.
  \todo add a locker for users
*/
QDateTime tkUserBase::recordLastLogin( const QString & log, const QString & pass )
{
    // change last login value
    QDateTime now = QDateTime::currentDateTime();
    QHash< int, QString >  where;
    where.insert( USER_LOGIN, QString( "='%1'" ).arg( log ) );
    where.insert( USER_PASSWORD, QString( "='%1'" ).arg( pass ) );
    QSqlDatabase DB = database();
    DB.open();
    QSqlQuery q( DB );
    q.prepare( prepareUpdateQuery( Table_USERS, USER_LASTLOG, where ) );
    q.bindValue( 0 , now );
    if ( ! q.exec() ) {
        tkLog::addQueryError( this, q );
        return QDateTime();
    }
    // TODO add locker
    tkLog::addMessage( this, tr( "Recorded User Last Login : %1 " ).arg( now.toString() ) );
    return now;
}

/**
  \brief Save users datas to the database.
  You can use this function to save a newly created user or to update an already existing user. This function
  manages both cases.
*/
bool tkUserBase::saveUser( tkUser *user )
{
    // connect user database
    QSqlDatabase DB = database();
    if ( ! DB.open() )
        tkLog::addError( "tkUserBase", QCoreApplication::translate( "tkUserBase", "Unable to open database %1").arg( DB.connectionName() ) );

    // if user already exists ==> update   else ==> insert
    bool toUpdate = false;
    QHash<int, QString> where;
    if ( user->id() != -1 ) {
        where.insert( USER_UUID, QString( "='%1'" ).arg( user->uuid() ) );
        QString querychecker = select( Table_USERS, USER_UUID, where );
        {
            QSqlQuery q( querychecker, DB );
            if ( q.isActive() ) {
                if ( q.next() )
                    toUpdate = true;
            }
            else
                tkLog::addQueryError( "tkUserBase", q );
        }
    }
    // construct query
    QStringList req;
    bool error = false;
    if ( toUpdate ) {
        // update Table_USERS
        {
            QSqlQuery q( DB );
            q.prepare( prepareUpdateQuery( Table_USERS, where ) );
            q.bindValue( USER_ID, user->id() );
            q.bindValue( USER_UUID, user->uuid() );
            q.bindValue( USER_VALIDITY, user->validity() );
            q.bindValue( USER_LOGIN, user->login() );
            q.bindValue( USER_PASSWORD, user->cryptedPassword() );
            q.bindValue( USER_LASTLOG, user->lastLogin() );
            q.bindValue( USER_NAME, user->name() );
            q.bindValue( USER_SECONDNAME, user->secondName() );
            q.bindValue( USER_SURNAME, user->surname() );
            q.bindValue( USER_MAIL, user->mail() );
            q.bindValue( USER_LANGUAGE, user->language() );
            q.bindValue( USER_LOCKER, user->locker() );
            if ( ! q.exec() ) {
                error = true;
                tkLog::addQueryError( "tkUserBase", q );
            }
        }
        // update dynamic datas
        if ( user->hasModifiedDynamicDatasToStore() ) {
            const QList<UserDynamicData*> & datasToUpdate = user->modifiedDynamicDatas();
            foreach( const UserDynamicData *dyn, datasToUpdate ) {
                QSqlQuery q ( DB );
                if ( dyn->id() == -1 ) {
                    // create the dynamic data
                    q.prepare( prepareInsertQuery( Table_DATAS ) );
                    q.bindValue( DATAS_ID, QVariant() ); // auto-id
                } else {
                    // update the dynamic data
                    QHash<int , QString> w;
                    w.insert( DATAS_USER_UUID, QString( "='%1'" ).arg( user->uuid() ) );
                    w.insert( DATAS_ID, QString ("=%1").arg( dyn->id()) );
                    w.insert( DATAS_DATANAME, QString( "='%1'" ).arg( dyn->name() ) );
                    q.prepare( prepareUpdateQuery( Table_DATAS, w ) );
                    q.bindValue(DATAS_ID, dyn->id());
                }
                dyn->prepareQuery(q);
                if (!q.exec()) {
                    error = true;
                    tkLog::addQueryError( "tkUserBase", q );
                }
            }
        }
        // TODO --> update rights
        if ( ! error )
            tkLog::addMessage( "tkUserBase", QCoreApplication::translate( "tkUserBase", "User %1 correctly updated." ).arg( user->uuid() ) );
    } else {
        // INSERT USER
        // add Table USERS
        {
            QSqlQuery q ( DB );
            q.prepare( prepareInsertQuery( Table_USERS ) );
            q.bindValue( USER_ID,           QVariant() );
            q.bindValue( USER_UUID,         user->uuid() );
            q.bindValue( USER_VALIDITY ,    user->validity() );
            q.bindValue( USER_LOGIN ,       user->login() );
            q.bindValue( USER_PASSWORD ,    user->cryptedPassword() );
            q.bindValue( USER_NAME ,        user->name() );
            q.bindValue( USER_SURNAME ,     user->surname() );
            q.bindValue( USER_SECONDNAME ,  user->secondName() );
            q.bindValue( USER_MAIL ,        user->mail() );
            q.bindValue( USER_LASTLOG ,     user->lastLogin() );
            q.bindValue( USER_LANGUAGE,     user->language() );
            q.bindValue( USER_LOCKER,       user->locker() );
            q.exec();
            if ( ! q.isActive() )
                tkLog::addQueryError( "tkUserBase", q );
        }
        // add dynamic datas
        if ( user->hasModifiedDynamicDatasToStore() ) {
            const QList<UserDynamicData*> & datasToUpdate = user->modifiedDynamicDatas();
            foreach( const UserDynamicData *dyn, datasToUpdate ) {
                QSqlQuery q ( DB );
                q.prepare( prepareInsertQuery( Table_DATAS ) );
                q.bindValue( DATAS_ID,         QVariant() );
                dyn->prepareQuery(q);
                q.exec();
                if ( ! q.isActive() )
                    tkLog::addQueryError( "tkUserBase", q );
            }
        }

        // add Table RIGHTS
        if ( user->hasModifiedRightsToStore() ) {
            const QStringList & rolesToUpdate = user->modifiedRoles();
            foreach( const QString & s, rolesToUpdate ) {
                QSqlQuery q ( DB );
                q.prepare( prepareInsertQuery( Table_RIGHTS ) );
                q.bindValue( RIGHTS_ID,         QVariant() );
                q.bindValue( RIGHTS_USER_UUID,  user->uuid() );
                q.bindValue( RIGHTS_ROLE ,      s );
                q.bindValue( RIGHTS_RIGHTS ,    user->rightsValue( s, RIGHTS_RIGHTS ) );
                q.exec();
                if ( ! q.isActive() )
                    tkLog::addQueryError( "tkUserBase", q );
            }
        }
    }
    user->setModified( false );
    return true;
}

/**
  \brief Delete an user identified by ots \e uuid from the database.
  \todo delete
  \todo delete table RIGHTS
*/
bool tkUserBase::deleteUser( const QString & uuid )
{
    // connect user database
    QSqlDatabase DB = database();
    if ( ! DB.open() )
        tkLog::addError( "tkUserBase", QCoreApplication::translate( "tkUserBase", "Unable to open database %1").arg( DB.connectionName() ) );

    QSqlQuery q( DB );

    // delete table USERS
    QHash<int,QString> where;
    where.insert( USER_UUID, QString( "='%1'" ).arg( uuid ) );
    if ( ! q.exec( prepareDeleteQuery( Table_USERS, where ) ) )
    {
        tkLog::addQueryError( this, q );
        return false;
    }
    return true;
}

