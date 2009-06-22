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

#include "tkUserModel.h"

// include toolkit headers
#include <tkGlobal.h>
#include <tkLog.h>
#include <tkTranslators.h>
#include <tkTextDocumentExtra.h>

// include usertoolkit headers
#include <tkUserBase.h>
#include <tkUser.h>

// include Qt headers
#include <QApplication>
#include <QUuid>
#include <QColor>
#include <QByteArray>
#include <QFont>
#include <QMessageBox>
#include <QHash>

using namespace tkUserConstants;

/**
  \brief Private Part.
  \internal
  \ingroup usertoolkit widget_usertoolkit usermanager
*/
class tkUserModelPrivate
{
public:
    tkUserModelPrivate() {}
    ~tkUserModelPrivate()
    {
        if ( m_Uuid_UserList.count() > 0 ) {
            qDeleteAll(m_Uuid_UserList);
            m_Uuid_UserList.clear();
        }
    }

    /**
      \brief Retreive all users datas and store it to the cache of the model.
      \sa numberOfUsersInMemory(), m_Uuid_UserList
    */
    bool addUserFromDatabase( const QString & uuid )
    {
        // make sure it is not already in the hash
        if ( m_Uuid_UserList.keys().contains( uuid ) )
            return true;

        // get user from database
        tkUser * un = tkUserBase::instance()->getUserByUuid( uuid );
        m_Uuid_UserList.insert( uuid, un );
        return true;
    }

    /**
      \brief Retreive all users datas and store it to the cache of the model. Return the created uuid.
      \sa numberOfUsersInMemory(), m_Uuid_UserList
    */
    QString addUserFromDatabase( const QString & log64, const QString & pass64 )
    {
        // get user from database
        QString uuid = tkUserBase::instance()->getUuid( log64, pass64 );
        // make sure it is not already in the hash
        if ( m_Uuid_UserList.keys().contains( uuid ) ) {
            return QString::null;
        }
        m_Uuid_UserList.insert( uuid, tkUserBase::instance()->getUserByLoginPassword( log64, pass64 ) );
        return uuid;
    }

    /** \brief Create and empty user into the model. The uuid of the user is automatically setted and returned. */
    QString createNewEmptyUser( tkUserModel * model, const int createdRow )
    {
        // 1. create an empty user into the hash
        QString uuid = tkUserBase::instance()->createNewUuid();
        m_Uuid_UserList.insert( uuid, new tkUser(uuid) );
        return uuid;
    }

public:
    QHash<QString, tkUser* > m_Uuid_UserList;
    QString m_CurrentUserUuid;
    User::UserRights m_CurrentUserRights;
};

tkUserModel * tkUserModel::m_Instance = 0;

tkUserModel * tkUserModel::instance( QObject *parent )
{
    if (!m_Instance) {
        if (parent)
            m_Instance = new tkUserModel(parent);
        else
            m_Instance = new tkUserModel(qApp);
    }
    return m_Instance;
}

/** \brief Constructor */
tkUserModel::tkUserModel( QObject * parent )
        : QSqlTableModel( parent, tkUserBase::instance()->database() ), d( 0 )
{
    d = new tkUserModelPrivate();
    if (!parent )
        setParent( qApp );
    QSqlTableModel::setTable( tkUserBase::instance()->table( Table_USERS ) );
    setEditStrategy( QSqlTableModel::OnManualSubmit );
    select();
//    m_Instance = this;
}

/** \brief Destructor */
tkUserModel::~tkUserModel()
{
    if (d) delete d;
    d=0;
}

/** \brief Should not be used. */
QModelIndex tkUserModel::createIndex ( int /*row*/, int /*column*/, quint32 /*id*/ ) const
{
    qWarning() << "tkUserModel::createIndex ( int row, int column, quint32 id ) const  --> OBSOLETE";
    return QModelIndex();
}

/** \brief Should not be used. \sa index() */
QModelIndex tkUserModel::createIndex(int row, int col, void * /*ptr*/ ) const
{
    if ( ( col >= 0 ) && ( col < User::UserMaxParam ) )
        return QAbstractItemModel::createIndex( row, col, 0 );
    return QModelIndex();
}

/**
  \brief Defines the current user using its login and password. There can be only one current user.
  The date and time of loggin are trace into database.
  \sa tkUser::setLastLogin(), tkUser::addLoginToHistory(), tkUserBase::saveUser()
  \todo save ONLY the login trace to database, not the whole user.
*/
bool tkUserModel::setCurrentUser( const QString & log64, const QString & cryptpass64 )
{
    d->m_CurrentUserUuid.clear();
    d->m_CurrentUserRights = User::NoRights;
    QString uuid = d->addUserFromDatabase( log64, cryptpass64 );
    if ( uuid.isEmpty() ) {
        tkLog::addError( this, tr( "Unable to retreive user into the model using login and password." ) );
        return false;
    }
    else
        tkLog::addMessage( this, tr( "Setting current user uuid to %1" ).arg( uuid ) );
    // change current user
    d->m_CurrentUserUuid = uuid;
    foreach( tkUser *u, d->m_Uuid_UserList.values() )
        u->setCurrent( false );
    // trace log
    tkUser *user = d->m_Uuid_UserList[d->m_CurrentUserUuid];
    user->setCurrent(true);
    user->setLastLogin( QDateTime::currentDateTime() );
    user->addLoginToHistory();
    tkUserBase::instance()->saveUser( user );
    user->setModified( false );
    d->m_CurrentUserRights = User::UserRights( user->rightsValue( USER_ROLE_USERMANAGER ).toInt() );
    emit memoryUsageChanged();
    return true;
}

/** \brief Return true if a current user has been defined. */
bool tkUserModel::hasCurrentUser()
{
    return (!d->m_CurrentUserUuid.isEmpty());
}

/** \brief Return the index of the current user. */
QModelIndex tkUserModel::currentUserIndex() const
{
    if ( d->m_CurrentUserUuid.isEmpty() )
        return QModelIndex();
    QModelIndexList list = match( createIndex( 0, User::UserUuid ), Qt::DisplayRole, d->m_CurrentUserUuid, 1 );
    if ( list.count() == 1 )
        return list.at(0);
    return QModelIndex();
}

/** \brief Returns the current database */
QSqlDatabase tkUserModel::database() const
{
    return tkUserBase::instance()->database();
}

/** \brief Check login/password validity. \sa tkUserBase::checkLogin(). */
bool tkUserModel::isCorrectLogin( const QString & logbase64, const QString & cryptpassbase64 )
{
    return tkUserBase::instance()->checkLogin( logbase64, cryptpassbase64 );
}

/**
 \todo alert when user to delete is modified ?
 \todo pb when userviewer is showing the index to delete
*/
bool tkUserModel::removeRows( int row, int count, const QModelIndex & parent )
{
    Q_UNUSED(parent);
    if ( ! d->m_CurrentUserRights & User::Delete )
        return false;
    bool noError = true;
    beginRemoveRows( QModelIndex(), row, row+count );
    // TODO --> alert when user to delete is modified ?
    // TODO --> pb when userviewer is showing the index to delete
    int i = 0;
    for ( i=0; i < count ; i++ ) {
        QString uuid = QSqlTableModel::index( row+i , USER_UUID ).data().toString();

        if ( uuid == d->m_CurrentUserUuid ) {
            tkLog::addMessage( this, tr( "Default user can not be deleted from database." ));
            tkGlobal::okCancelMessageBox( tr("User can not be deleted."),
                                          tr( "You can not delete super-administrator user."),
                                          tr( "The super-administrator user is defined by default, "
                                              "if you want to protect access to "
                                              "the super-administrator you'd better change its password." ),
                                          qApp->applicationName() );
            /** \todo add a button (change su password) */
            continue;
        }

        // Delete from QHash
        if ( d->m_Uuid_UserList.keys().contains( uuid ) ) {
            if ( d->m_Uuid_UserList.value( uuid )->isModified() )  {
                tkLog::addError( this, tr( "You can not delete a modified user, save it before." ) );
                noError = false;
            } else {
                delete d->m_Uuid_UserList[uuid];
                d->m_Uuid_UserList[uuid] = 0;
                d->m_Uuid_UserList.remove( uuid );
            }
        }

        // Delete from real database
        if ( ! tkUserBase::instance()->deleteUser( uuid ) ) {
            tkLog::addError( this, tr( "User can not be deleted from database." ) );
            noError = false;
        }
    }
    endRemoveRows();
    QSqlTableModel::select();
    reset(); // needed
    emit memoryUsageChanged();
    return noError;
}

/** \brief Create a new row (new user) into the model. */
bool tkUserModel::insertRows( int row, int count, const QModelIndex & parent )
{
    if ( ! d->m_CurrentUserRights & User::Create )
        return false;
    int i=0;
    for (i=0;i<count;i++)
    {
        // create a new empty tkUser
        if ( ! QSqlTableModel::insertRows( row + i, 1, parent) ) {
            tkLog::addError( this, QString( "Can not create a new user into SQL Table.") );
            return i;
        }
        QString uuid = d->createNewEmptyUser( this, row+i );
        // feed the QSqlTableModel with uuid and crypted empty password
        QModelIndex newIndex = index( row+i, User::UserUuid );
        if ( ! QSqlTableModel::setData( newIndex, uuid, Qt::EditRole ) ) {
            tkLog::addError( "tkUserModelPrvivate", QString( "Can not add user's uuid into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg( row+i ).arg( uuid ) );
            return i;
        }
        newIndex = index( row+i, User::UserPassword );
        if ( ! QSqlTableModel::setData( newIndex, tkUserGlobal::crypt( "" ), Qt::EditRole ) ) {
            tkLog::addError( "tkUserModelPrvivate", QString( "Can not add user's login into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg( row+i ).arg( uuid ) );
            return i;
        }
    }
    emit memoryUsageChanged();
    return i;
}


/** \brief Define the datas of users. \sa User::UserModel */
bool tkUserModel::setData( const QModelIndex & item, const QVariant & value, int role )
{
    if ( ! value.isValid() )
        return false;
    if( ! item.isValid() )
        return false;
    // work only for EditRole
    if ( role != Qt::EditRole )
        return false;
    // get uuid from real database
    QString uuid = QSqlTableModel::data( QSqlTableModel::index( item.row(), USER_UUID ), Qt::DisplayRole ).toString();
    if ( ! d->m_Uuid_UserList.keys().contains( uuid ) ) {
        d->addUserFromDatabase( uuid );
        emit memoryUsageChanged();
    }
    tkUser *user = d->m_Uuid_UserList[uuid];
    // check user write rights
    if ( user->isCurrent() ) {
        if ( ! d->m_CurrentUserRights & User::WriteOwn )
            return false;
    } else if ( ! d->m_CurrentUserRights & User::WriteAll )
        return false;
    // TODO --> if user if a delegate of current user

    // set datas directly into database using QSqlTableModel if possible
    if ( item.column() < tkUserConstants::USER_MaxParam ) {
        // prepare SQL update
        if ( ! QSqlTableModel::setData( item, value, role ) ) {
            tkLog::addError( this, QString( "enable to setData to SqlModel. Row %1, col %2, data %3" )
                             .arg( item.row()).arg( item.column() ).arg( value.toString() ) );
            return false;
        }
        // poursue to feed tkUser (need to know if it is modified)
    }

    switch (item.column())
    {
        case User::UserId :  user->setId( value ); break;
        case User::UserUuid :  user->setUuid( value ); break;
        case User::UserValidity :  user->setValidity( value ); break;
        case User::UserLogin :  user->setLogin( value ); break;
        case User::UserDecryptedLogin : user->setLogin( value.toString().toAscii().toBase64() ); break;
        case User::UserPassword :  user->setCryptedPassword( value ); break;
        case User::UserLastLogin :  user->setLastLogin( value ); break;
        case User::UserGenderIndex : user->setGender( value ); break;
        case User::UserTitleIndex : user->setTitle( value ); break;
        case User::UserName :  user->setName( value ); break;
        case User::UserSecondName :  user->setSecondName( value ); break;
        case User::UserSurname :  user->setSurname( value ); break;
        case User::UserMail :  user->setMail( value ); break;
        case User::UserLanguage :  user->setLanguage( value ); break;
        case User::UserLanguageIndex :  user->setLanguage( tkTranslators::availableLocales().at(value.toInt()) ); break;
        case User::UserAdress :  user->setAdress( value ); break;
        case User::UserZipcode :  user->setZipcode( value ); break;
        case User::UserCity :  user->setCity( value ); break;
        case User::UserCountry :  user->setCountry( value ); break;
        case User::UserTel1 :  user->setTel1( value ); break;
        case User::UserTel2 :  user->setTel2( value ); break;
        case User::UserTel3 :  user->setTel3( value ); break;
        case User::UserFax :  user->setFax( value ); break;
        case User::UserPractitionerId :  user->setPractitionerIdentifiant( value.toStringList() ); break;
        case User::UserSpecialities :  user->setSpecialty( value.toStringList() ); break;
        case User::UserQualifications :  user->setQualification( value.toStringList() ); break;
        case User::UserPreferences :  user->setPreferences( value ); break;
//        case User::UserGenericHeader : user->setExtraDocumentHtml( value, User::UserGenericHeader ); break;
//        case User::UserGenericFooter :  user->setExtraDocumentHtml( value, User::UserGenericFooter ); break;
//        case User::UserAdministrativeHeader : user->setAdminHeader( value ); break;
//        case User::UserAdministrativeFooter : user->setAdminFooter( value ); break;
//        case User::UserPrescriptionHeader : user->setPrescriptionHeader( value ); break;
//        case User::UserPrescriptionFooter : user->setPrescriptionFooter( value ); break;
        case User::UserManagerRights : user->setRights( USER_ROLE_USERMANAGER, User::UserRights( value.toInt() ) ); break;
        case User::UserMedicalRights : user->setRights( USER_ROLE_MEDICAL, User::UserRights( value.toInt() ) ); break;
        case User::UserDrugsRights : user->setRights( USER_ROLE_DOSAGES, User::UserRights( value.toInt() ) ); break;
        case User::UserParamedicalRights : user->setRights( USER_ROLE_PARAMEDICAL, User::UserRights( value.toInt() ) ); break;
        case User::UserAdministrativeRights : user->setRights( USER_ROLE_ADMINISTRATIVE, User::UserRights( value.toInt() ) ); break;
        default : return false;
    };
    emit dataChanged( index( item.row(), 0 ), index( item.row(), this->columnCount() ) );
    return true;
}

/** \brief Returns the datas of users. \sa User::UserModel */
QVariant tkUserModel::data( const QModelIndex & item, int role ) const
{
    if ( ! item.isValid() )
        return QVariant();

    // get user
    QString uuid = QSqlTableModel::data( QSqlTableModel::index( item.row(), USER_UUID ), Qt::DisplayRole ).toString();

    // First manage decoration WITHOUT retreiving any user from database
    if ( role == Qt::FontRole )
    {
        QFont font;
        if ( d->m_Uuid_UserList.keys().contains(uuid) ) {
            if ( d->m_Uuid_UserList.value(uuid)->isModified() )
                 font.setBold(true);
             else
                 font.setBold(false);
         } else
             font.setBold(false);
        return font;
    }
    else if ( role == Qt::BackgroundRole )
    {
        QColor c;
        if ( d->m_Uuid_UserList.keys().contains(uuid) ) {
            if ( d->m_Uuid_UserList.value(uuid)->isModified() )
                c = QColor(Qt::red);
             else
                 c = QColor(Qt::white);
         } else
            c = QColor(Qt::white);
         return c;
    }


    // Manage table USERS using the QSqlTableModel WITHOUT retreiving whole user from database
    if ( ( item.column() < User::UserLanguageIndex ) ) {
        // here we suppose that it is the currentUser the ask for datas
        // TODO had delegates rights
        if ( d->m_CurrentUserRights & User::ReadAll )
            return QSqlTableModel::data( item, role );
        else if ( ( d->m_CurrentUserRights & User::ReadOwn ) &&
                  ( d->m_CurrentUserUuid == uuid ) )
            return QSqlTableModel::data( item, role );
    }

    // Here we must get values from complete user, so retreive it from database if necessary
    if ( ! d->m_Uuid_UserList.keys().contains( uuid ) ) {
        d->addUserFromDatabase( uuid );
        emit memoryUsageChanged();
    }
    const tkUser * user = d->m_Uuid_UserList.value(uuid);
    // check user write rights
    if ( user->isCurrent() ) {
        if ( ! d->m_CurrentUserRights & User::ReadOwn )
            return QVariant();
    } else if ( ! d->m_CurrentUserRights & User::ReadAll )
            return QVariant();
    // TODO --> if user if a delegate of current user

    // get the requiered data
    QVariant toReturn;

    if ( ( role == Qt::DisplayRole ) || ( role == Qt::EditRole ) )
    {
        // get datas directly from database using QSqlTableModel if possible
        if ( item.column() < tkUserConstants::USER_LANGUAGE )
            return QSqlTableModel::data( item, role );

        switch (item.column())
        {
            case User::UserId : toReturn = user->id(); break;
            case User::UserUuid : toReturn = user->uuid(); break;
            case User::UserValidity : toReturn = user->validity(); break;
            case User::UserLogin : toReturn = user->login(); break;
            case User::UserDecryptedLogin : toReturn = user->decryptedLogin(); break;
            case User::UserPassword : toReturn = user->cryptedPassword(); break;
            case User::UserLastLogin : toReturn = user->lastLogin(); break;
            case User::UserGenderIndex : toReturn = user->gender(); break;
            case User::UserTitleIndex : toReturn = user->title(); break;
            case User::UserGender : toReturn = genders().at( user->gender() ); break;
            case User::UserTitle : toReturn = titles().at( user->title() ); break;
            case User::UserName : toReturn = user->name(); break;
            case User::UserSecondName : toReturn = user->secondName(); break;
            case User::UserSurname : toReturn = user->surname(); break;
            case User::UserMail : toReturn = user->mail(); break;
            case User::UserLanguage : toReturn = user->language(); break;
            case User::UserLanguageIndex : toReturn = tkTranslators::availableLocales().indexOf( user->language() ); break;
            case User::UserAdress : toReturn = user->adress(); break;
            case User::UserZipcode : toReturn = user->zipcode(); break;
            case User::UserCity : toReturn = user->city(); break;
            case User::UserCountry : toReturn = user->country(); break;
            case User::UserTel1 : toReturn = user->tels().at(0); break;
            case User::UserTel2 : toReturn = user->tels().at(1); break;
            case User::UserTel3 : toReturn = user->tels().at(2); break;
            case User::UserFax : toReturn = user->fax(); break;
            case User::UserPractitionerId : toReturn = user->practitionerId(); break;
            case User::UserSpecialities : toReturn = user->specialty(); break;
            case User::UserQualifications : toReturn = user->qualifications(); break;
            case User::UserPreferences : toReturn = user->preferences(); break;
            case User::UserGenericHeader : toReturn = user->extraDocument(USER_DATAS_GENERICHEADER); break;
//            case User::UserGenericFooter : toReturn = user->genericFooter(); break;
//            case User::UserAdministrativeHeader : toReturn = user->adminPapers().at(0); break;
//            case User::UserAdministrativeFooter : toReturn = user->adminPapers().at(1); break;
//            case User::UserPrescriptionHeader : toReturn = user->prescrPapers().at(0); break;
//            case User::UserPrescriptionFooter : toReturn = user->prescrPapers().at(1); break;
            case User::UserIsModified : toReturn = user->isModified(); break;
            case User::UserManagerRights : toReturn = user->rightsValue( USER_ROLE_USERMANAGER ); break;
            case User::UserMedicalRights : toReturn = user->rightsValue( USER_ROLE_MEDICAL ); break;
            case User::UserDrugsRights : toReturn = user->rightsValue( USER_ROLE_DOSAGES ); break;
            case User::UserParamedicalRights : toReturn = user->rightsValue( USER_ROLE_PARAMEDICAL ); break;
            case User::UserAdministrativeRights : toReturn = user->rightsValue( USER_ROLE_ADMINISTRATIVE ); break;
            case User::UserLoginHistory : toReturn = user->loginHistory(); break;
            case User::UserWarn : user->warn(); break;
            case User::UserWarnText : toReturn = user->warnText(); break;
            default : toReturn = QVariant();
        };
    }

//    qWarning() << "end data" << toReturn;

    return toReturn;
}

/** Not implemented */
void tkUserModel::setSort( int /*column*/, Qt::SortOrder /*order*/ )
{
}
/** Not implemented */
void tkUserModel::sort( int /*column*/, Qt::SortOrder /*order*/ )
{
}
/** Not implemented */
void tkUserModel::setTable( const QString & /*tableName*/ )
{
}
/** Not implemented */
void tkUserModel::setFilter( const QString & /*filter*/ )
{
}

/** \brief Returns true if model has dirty rows that need to be saved into database. */
bool tkUserModel::hasUserToSave()
{
    foreach( const tkUser *u, d->m_Uuid_UserList.values() )
        if ( u->isModified() )
            return true;
    return false;
}

/** \brief Submit all changes of the model into database */
bool tkUserModel::submitAll()
{
    bool toReturn = true;
    int i = 0;
    foreach( const QString & s, d->m_Uuid_UserList.keys() ) {
        if ( ! submitUser( s ) )
            toReturn = false;
        else
            i++;
    }
    emit memoryUsageChanged();
    return toReturn;
}

/** \brief Submit only one user changes of the model into database */
bool tkUserModel::submitUser( const QString & uuid )
{
    bool toReturn = true;
    QModelIndexList list = match( createIndex( 0, User::UserUuid ), Qt::DisplayRole, uuid, 1 );
    if ( list.count() != 1 )
        return false;
    // act only on modified users
    if ( d->m_Uuid_UserList.value(uuid)->isModified() ) {
        tkUser * user = d->m_Uuid_UserList[uuid];
        // check user write rights
        if ( ( user->isCurrent() ) &&
             ( ! d->m_CurrentUserRights & User::WriteOwn ) )
            toReturn = false;
        else if ( ( ! user->isCurrent() ) &&
                  ( ! d->m_CurrentUserRights & User::WriteAll ) )
            toReturn = false;
        else if ( ! tkUserBase::instance()->saveUser( user ) )
            toReturn = false;
    }
    emit dataChanged( index(list.at(0).row(),0) , index(list.at(0).row(), User::UserMaxParam) );
    return toReturn;
}

/** \brief Reverts the model. */
bool tkUserModel::revertAll()
{
    // TODO ?? --> ASSERT failure in QSqlTableModelPrivate::revertCachedRow(): "Invalid entry in cache map", file models\qsqltablemodel.cpp, line 151
    int i = 0;
    for( i=0; i < rowCount() ; i++ )
        revertRow( i );
    select();
    reset();
    return true;
}

/** \brief Revert a row */
void tkUserModel::revertRow( int row )
{
    QString uuid = QSqlTableModel::index( row, USER_UUID ).data().toString();
    QSqlTableModel::revertRow( row );
    if ( d->m_Uuid_UserList.keys().contains( uuid ) )
    {
        delete d->m_Uuid_UserList[uuid];
        d->m_Uuid_UserList[uuid] = 0;
        d->m_Uuid_UserList.remove( uuid );
    }
//    select();
    reset();
    emit memoryUsageChanged();
}

/**
  \brief Define the filter of the model.
  \sa tkDatabase::getWhereClause()
*/
void tkUserModel::setFilter ( const QHash<int,QString> & conditions )
{
    // TODO filter by name AND surname at the same time
    QString filter = "";
    const tkUserBase * b = tkUserBase::instance();
    foreach( const int r, conditions.keys() ) {
        QString baseField = "";
        switch (r)
        {
        case User::UserName : baseField = b->field( Table_USERS, USER_NAME ); break;
        case User::UserSurname : baseField = b->field( Table_USERS, USER_SURNAME ); break;
        default: break;
        }
        if ( baseField.isEmpty() ) continue;
        filter += QString( "( `%1` %2 ) AND \n" ).arg( baseField, conditions.value( r ) );
    }
    filter.chop(5);
    QSqlTableModel::setFilter( filter );
    qWarning() << filter;
}

/** \brief Returns the number of user stored into the memory. */
int tkUserModel::numberOfUsersInMemory()
{
    return d->m_Uuid_UserList.count();
}

tkTextDocumentExtra *tkUserModel::currentUserDocument( const int column )
{
     tkUser *u = d->m_Uuid_UserList[d->m_CurrentUserUuid];
     return u->extraDocument(column);
}

void tkUserModel::setCurrentUserDocument( tkTextDocumentExtra *extra, const int column )
{
     tkUser *u = d->m_Uuid_UserList[d->m_CurrentUserUuid];
     u->setExtraDocument( extra, column);
}


/** \brief For debugging purpose only */
void tkUserModel::warn()
{
    if (!tkGlobal::isDebugCompilation())
        return;
    qWarning() << "tkUserModel Warning";
    qWarning() << d->m_CurrentUserUuid;
    qWarning() << d->m_Uuid_UserList;
}
