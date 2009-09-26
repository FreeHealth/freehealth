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
  \class UserModel
  \brief Users are represented into a table model. Each row represents a user, each column a value.
  \sa User, tkUserConstants
  \todo write documentation+++
  \todo code LOCKER
  \todo when QDataWidgetMapper (tkUserViewer) is setted, it calls ALL the datas of the user, even for the hidden widgets. This causes an important memory usage. This is to improve ++++
  Set filter with setFilter().
  \ingroup usertoolkit object_usertoolkit
  \ingroup usermanager
*/

#include "usermodel.h"

#include <utils/global.h>
#include <utils/log.h>

#include <coreplugin/translators.h>

#include <printerplugin/textdocumentextra.h>

#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/userdata.h>

#include <QApplication>
#include <QUuid>
#include <QColor>
#include <QByteArray>
#include <QFont>
#include <QHash>

using namespace UserPlugin;
using namespace UserPlugin::Constants;

namespace UserPlugin {
namespace Internal {

/**
  \brief Private Part.
  \internal
  \ingroup usertoolkit widget_usertoolkit usermanager
*/
class UserModelPrivate
{
public:
    UserModelPrivate() {}
    ~UserModelPrivate()
    {
        if (m_Uuid_UserList.count() > 0) {
            qDeleteAll(m_Uuid_UserList);
            m_Uuid_UserList.clear();
        }
    }

    /**
      \brief Retreive all users datas and store it to the cache of the model.
      \sa numberOfUsersInMemory(), m_Uuid_UserList
    */
    bool addUserFromDatabase(const QString &uuid)
    {
        // make sure it is not already in the hash
        if (m_Uuid_UserList.keys().contains(uuid))
            return true;

        // get user from database
        UserData *un = UserBase::instance()->getUserByUuid(uuid);
        m_Uuid_UserList.insert(uuid, un);
        return true;
    }

    /**
      \brief Retreive all users datas and store it to the cache of the model. Return the created uuid.
      \sa numberOfUsersInMemory(), m_Uuid_UserList
    */
    QString addUserFromDatabase(const QString &log64, const QString &pass64)
    {
        // get user from database
        QString uuid = UserBase::instance()->getUuid(log64, pass64);
        if (uuid.isEmpty())
            return uuid;
        // make sure it is not already in the hash
        if (m_Uuid_UserList.keys().contains(uuid)) {
            return uuid;
        }
        m_Uuid_UserList.insert(uuid, UserBase::instance()->getUserByUuid(uuid));
        return uuid;
    }

    /** \brief Create and empty user into the model. The uuid of the user is automatically setted and returned. */
    QString createNewEmptyUser(UserModel *model, const int createdRow)
    {
        // 1. create an empty user into the hash
        QString uuid = UserBase::instance()->createNewUuid();
        m_Uuid_UserList.insert(uuid, new UserData(uuid));
        return uuid;
    }

public:
    QHash<QString, UserData *> m_Uuid_UserList;
    QString m_CurrentUserUuid;
    User::UserRights m_CurrentUserRights;
};

}  // End Internal
}  // End UserPlugin



UserModel *UserModel::m_Instance = 0;

UserModel *UserModel::instance(QObject *parent)
{
    if (!m_Instance) {
        if (parent)
            m_Instance = new UserModel(parent);
        else
            m_Instance = new UserModel(qApp);
    }
    return m_Instance;
}

/** \brief Constructor */
UserModel::UserModel(QObject *parent)
        : QSqlTableModel(parent, Internal::UserBase::instance()->database()), d(0)
{
    setObjectName("UserModel");
    d = new Internal::UserModelPrivate();
    if (!parent)
        setParent(qApp);
    QSqlTableModel::setTable(Internal::UserBase::instance()->table(Table_USERS));
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
}

/** \brief Destructor */
UserModel::~UserModel()
{
    if (d) delete d;
    d=0;
}

/** \brief Should not be used. */
QModelIndex UserModel::createIndex (int /*row*/, int /*column*/, quint32 /*id*/) const
{
    qWarning() << "UserModel::createIndex (int row, int column, quint32 id) const  --> OBSOLETE";
    return QModelIndex();
}

/** \brief Should not be used. \sa index() */
QModelIndex UserModel::createIndex(int row, int col, void * /*ptr*/) const
{
    if ((col >= 0) && (col < User::MaxParam))
        return QAbstractItemModel::createIndex(row, col, 0);
    return QModelIndex();
}

/**
  \brief Defines the current user using its login and password. There can be only one current user.
  The date and time of loggin are trace into database.
  \sa tkUser::setLastLogin(), tkUser::addLoginToHistory(), UserBase::saveUser()
  \todo save ONLY the login trace to database, not the whole user.
*/
bool UserModel::setCurrentUser(const QString &log64, const QString &cryptpass64)
{
    d->m_CurrentUserUuid.clear();
    d->m_CurrentUserRights = User::NoRights;
    QString uuid = d->addUserFromDatabase(log64, cryptpass64);
    if (uuid.isEmpty()) {
        Utils::Log::addError(this, tr("Unable to retreive user into the model using login and password."));
        return false;
    }
    else
        Utils::Log::addMessage(this, tr("Setting current user uuid to %1").arg(uuid));
    // change current user
    d->m_CurrentUserUuid = uuid;
    foreach(Internal::UserData *u, d->m_Uuid_UserList.values())
        u->setCurrent(false);
    // trace log
    Internal::UserData *user = d->m_Uuid_UserList[d->m_CurrentUserUuid];
    user->setCurrent(true);
    user->setLastLogin(QDateTime::currentDateTime());
    user->addLoginToHistory();
    Internal::UserBase::instance()->saveUser(user);
    user->setModified(false);
    d->m_CurrentUserRights = User::UserRights(user->rightsValue(USER_ROLE_USERMANAGER).toInt());
    emit memoryUsageChanged();
    return true;
}

/** \brief Return true if a current user has been defined. */
bool UserModel::hasCurrentUser()
{
    return (!d->m_CurrentUserUuid.isEmpty());
}

/** \brief Return the index of the current user. */
QModelIndex UserModel::currentUserIndex() const
{
    qWarning() << "currentUserIndex" << rowCount() << d->m_CurrentUserUuid;
    if (d->m_CurrentUserUuid.isEmpty())
        return QModelIndex();
    QModelIndexList list = match(createIndex(0, User::Uuid), Qt::DisplayRole, d->m_CurrentUserUuid, 1);
    if (list.count() == 1) {
        return list.at(0);
    }
    return QModelIndex();
}

/** \brief Returns the current database */
QSqlDatabase UserModel::database() const
{
    return Internal::UserBase::instance()->database();
}

/** \brief Check login/password validity. \sa UserBase::checkLogin(). */
bool UserModel::isCorrectLogin(const QString &logbase64, const QString &cryptpassbase64)
{
    return Internal::UserBase::instance()->checkLogin(logbase64, cryptpassbase64);
}

/**
 \todo alert when user to delete is modified ?
 \todo pb when userviewer is showing the index to delete
*/
bool UserModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (! d->m_CurrentUserRights &User::Delete)
        return false;
    bool noError = true;
    beginRemoveRows(QModelIndex(), row, row+count);
    // TODO --> alert when user to delete is modified ?
    // TODO --> pb when userviewer is showing the index to delete
    int i = 0;
    for (i=0; i < count ; i++) {
        QString uuid = QSqlTableModel::index(row+i , USER_UUID).data().toString();

        if (uuid == d->m_CurrentUserUuid) {
            Utils::Log::addMessage(this, tr("Default user can not be deleted from database."));
            Utils::okCancelMessageBox(tr("User can not be deleted."),
                                          tr("You can not delete super-administrator user."),
                                          tr("The super-administrator user is defined by default, "
                                              "if you want to protect access to "
                                              "the super-administrator you'd better change its password."),
                                          qApp->applicationName());
            /** \todo add a button (change su password) */
            continue;
        }

        // Delete from QHash
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            if (d->m_Uuid_UserList.value(uuid)->isModified())  {
                Utils::Log::addError(this, tr("You can not delete a modified user, save it before."));
                noError = false;
            } else {
                delete d->m_Uuid_UserList[uuid];
                d->m_Uuid_UserList[uuid] = 0;
                d->m_Uuid_UserList.remove(uuid);
            }
        }

        // Delete from real database
        if (!Internal::UserBase::instance()->deleteUser(uuid)) {
            Utils::Log::addError(this, tr("User can not be deleted from database."));
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
bool UserModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (! d->m_CurrentUserRights &User::Create)
        return false;
    int i=0;
    for (i=0;i<count;i++)
    {
        // create a new empty tkUser
        if (! QSqlTableModel::insertRows(row + i, 1, parent)) {
            Utils::Log::addError(this, QString("Can not create a new user into SQL Table."));
            return i;
        }
        QString uuid = d->createNewEmptyUser(this, row+i);
        // feed the QSqlTableModel with uuid and crypted empty password
        QModelIndex newIndex = index(row+i, User::Uuid);
        if (! QSqlTableModel::setData(newIndex, uuid, Qt::EditRole)) {
            Utils::Log::addError(this, QString("Can not add user's uuid into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg(row+i).arg(uuid));
            return i;
        }
        newIndex = index(row+i, User::Password);
        if (! QSqlTableModel::setData(newIndex, UserPlugin::crypt(""), Qt::EditRole)) {
            Utils::Log::addError(this, QString("Can not add user's login into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg(row+i).arg(uuid));
            return i;
        }
    }
    emit memoryUsageChanged();
    return i;
}


/** \brief Define the datas of users. \sa User::Model */
bool UserModel::setData(const QModelIndex &item, const QVariant &value, int role)
{
    if (! value.isValid())
        return false;
    if(! item.isValid())
        return false;
    // work only for EditRole
    if (role != Qt::EditRole)
        return false;
    // get uuid from real database
    QString uuid = QSqlTableModel::data(QSqlTableModel::index(item.row(), USER_UUID), Qt::DisplayRole).toString();
    if (! d->m_Uuid_UserList.keys().contains(uuid)) {
        d->addUserFromDatabase(uuid);
        emit memoryUsageChanged();
    }
    Internal::UserData *user = d->m_Uuid_UserList[uuid];
    // check user write rights
    if (user->isCurrent()) {
        if (! d->m_CurrentUserRights &User::WriteOwn)
            return false;
    } else if (! d->m_CurrentUserRights &User::WriteAll)
        return false;
    // TODO --> if user if a delegate of current user

    // set datas directly into database using QSqlTableModel if possible
    if (item.column() < USER_MaxParam) {
        // prepare SQL update
        if (! QSqlTableModel::setData(item, value, role)) {
            Utils::Log::addError(this, QString("enable to setData to SqlModel. Row %1, col %2, data %3")
                             .arg(item.row()).arg(item.column()).arg(value.toString()));
            return false;
        }
        // poursue to feed UserData (need to know if it is modified)
    }

    switch (item.column())
    {
        case User::Id :  user->setId(value); break;
        case User::Uuid :  user->setUuid(value.toString()); break;
        case User::Validity :  user->setValidity(value); break;
        case User::Login :  user->setLogin(value); break;
        case User::DecryptedLogin : user->setLogin(value.toString().toAscii().toBase64()); break;
        case User::Password :  user->setCryptedPassword(value); break;
        case User::LastLogin :  user->setLastLogin(value); break;
        case User::GenderIndex : user->setGender(value); break;
        case User::TitleIndex : user->setTitle(value); break;
        case User::Name :  user->setName(value); break;
        case User::SecondName :  user->setSecondName(value); break;
        case User::Surname :  user->setSurname(value); break;
        case User::Mail :  user->setMail(value); break;
        case User::Language :  user->setLanguage(value); break;
        case User::LanguageIndex :
            {
                if (Core::Translators::availableLocales().count() < value.toInt())
                    user->setLanguage(Core::Translators::availableLocales().at(value.toInt()));
                break;
            }
        case User::Adress :  user->setAdress(value); break;
        case User::Zipcode :  user->setZipcode(value); break;
        case User::City :  user->setCity(value); break;
        case User::Country :  user->setCountry(value); break;
        case User::Tel1 :  user->setTel1(value); break;
        case User::Tel2 :  user->setTel2(value); break;
        case User::Tel3 :  user->setTel3(value); break;
        case User::Fax :  user->setFax(value); break;
        case User::PractitionerId :  user->setPractitionerIdentifiant(value.toStringList()); break;
        case User::Specialities :  user->setSpecialty(value.toStringList()); break;
        case User::Qualifications :  user->setQualification(value.toStringList()); break;
        case User::Preferences :  user->setPreferences(value); break;

        /** \todo Add Xml extra document to model */
        case User::GenericHeader : user->setExtraDocumentHtml(value, User::GenericHeader); break;
        case User::GenericFooter :  user->setExtraDocumentHtml(value, User::GenericFooter); break;
        case User::GenericWatermark :  user->setExtraDocumentHtml(value, User::GenericWatermark); break;
        case User::AdministrativeHeader : user->setExtraDocumentHtml(value, User::AdministrativeHeader); break;
        case User::AdministrativeFooter : user->setExtraDocumentHtml(value, User::AdministrativeFooter); break;
        case User::AdministrativeWatermark : user->setExtraDocumentHtml(value, User::AdministrativeWatermark); break;
        case User::PrescriptionHeader : user->setExtraDocumentHtml(value, User::AdministrativeHeader); break;
        case User::PrescriptionFooter : user->setExtraDocumentHtml(value, User::PrescriptionFooter); break;
        case User::PrescriptionWatermark : user->setExtraDocumentHtml(value, User::PrescriptionWatermark); break;

        case User::ManagerRights : user->setRights(USER_ROLE_USERMANAGER, User::UserRights(value.toInt())); break;
        case User::MedicalRights : user->setRights(USER_ROLE_MEDICAL, User::UserRights(value.toInt())); break;
        case User::DrugsRights : user->setRights(USER_ROLE_DOSAGES, User::UserRights(value.toInt())); break;
        case User::ParamedicalRights : user->setRights(USER_ROLE_PARAMEDICAL, User::UserRights(value.toInt())); break;
        case User::AdministrativeRights : user->setRights(USER_ROLE_ADMINISTRATIVE, User::UserRights(value.toInt())); break;
        default : return false;
    };
    emit dataChanged(index(item.row(), 0), index(item.row(), this->columnCount()));
    return true;
}

/** \brief Returns the datas of users. \sa User::Model */
QVariant UserModel::data(const QModelIndex &item, int role) const
{
    if (! item.isValid())
        return QVariant();

    // get user
    QString uuid = QSqlTableModel::data(QSqlTableModel::index(item.row(), USER_UUID), Qt::DisplayRole).toString();

    // First manage decoration WITHOUT retreiving any user from database
    if (role == Qt::FontRole)
    {
        QFont font;
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            if (d->m_Uuid_UserList.value(uuid)->isModified())
                 font.setBold(true);
             else
                 font.setBold(false);
         } else
             font.setBold(false);
        return font;
    }
    else if (role == Qt::BackgroundRole)
    {
        QColor c;
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            if (d->m_Uuid_UserList.value(uuid)->isModified())
                c = QColor(Qt::red);
             else
                 c = QColor(Qt::white);
         } else
            c = QColor(Qt::white);
         return c;
    }


    // Manage table USERS using the QSqlTableModel WITHOUT retreiving whole user from database
    if ((item.column() < User::LanguageIndex)) {
        // here we suppose that it is the currentUser the ask for datas
        // TODO had delegates rights
        if (d->m_CurrentUserRights &User::ReadAll)
            return QSqlTableModel::data(item, role);
        else if ((d->m_CurrentUserRights &User::ReadOwn) &&
                  (d->m_CurrentUserUuid == uuid))
            return QSqlTableModel::data(item, role);
    }

    // Here we must get values from complete user, so retreive it from database if necessary
    if (! d->m_Uuid_UserList.keys().contains(uuid)) {
        d->addUserFromDatabase(uuid);
        emit memoryUsageChanged();
    }
    const Internal::UserData *user = d->m_Uuid_UserList.value(uuid);
    // check user write rights
    if (user->isCurrent()) {
        if (! d->m_CurrentUserRights &User::ReadOwn)
            return QVariant();
    } else if (! d->m_CurrentUserRights &User::ReadAll)
            return QVariant();
    // TODO --> if user if a delegate of current user

    // get the requiered data
    QVariant toReturn;

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        // get datas directly from database using QSqlTableModel if possible
        if (item.column() < USER_LANGUAGE)
            return QSqlTableModel::data(item, role);

        switch (item.column())
        {
            case User::Id : toReturn = user->id(); break;
            case User::Uuid : toReturn = user->uuid(); break;
            case User::Validity : toReturn = user->validity(); break;
            case User::Login : toReturn = user->login(); break;
            case User::DecryptedLogin : toReturn = user->decryptedLogin(); break;
            case User::Password : toReturn = user->cryptedPassword(); break;
            case User::LastLogin : toReturn = user->lastLogin(); break;
            case User::GenderIndex : toReturn = user->gender(); break;
            case User::TitleIndex : toReturn = user->title(); break;
            case User::Gender : toReturn = genders().at(user->gender()); break;
            case User::Title : toReturn = titles().at(user->title()); break;
            case User::Name : toReturn = user->name(); break;
            case User::SecondName : toReturn = user->secondName(); break;
            case User::Surname : toReturn = user->surname(); break;
            case User::Mail : toReturn = user->mail(); break;
            case User::Language : toReturn = user->language(); break;
            case User::LanguageIndex : toReturn = Core::Translators::availableLocales().indexOf(user->language()); break;
            case User::Adress : toReturn = user->adress(); break;
            case User::Zipcode : toReturn = user->zipcode(); break;
            case User::City : toReturn = user->city(); break;
            case User::Country : toReturn = user->country(); break;
            case User::Tel1 : toReturn = user->tels().at(0); break;
            case User::Tel2 : toReturn = user->tels().at(1); break;
            case User::Tel3 : toReturn = user->tels().at(2); break;
            case User::Fax : toReturn = user->fax(); break;
            case User::PractitionerId : toReturn = user->practitionerId(); break;
            case User::Specialities : toReturn = user->specialty(); break;
            case User::Qualifications : toReturn = user->qualifications(); break;
            case User::Preferences : toReturn = user->preferences(); break;

            case User::GenericHeader : toReturn = user->extraDocumentHtml(User::GenericHeader); break;
            case User::GenericFooter : toReturn = user->extraDocumentHtml(User::GenericFooter); break;
            case User::GenericWatermark :  toReturn = user->extraDocumentHtml(User::GenericWatermark); break;
            case User::AdministrativeHeader : toReturn = user->extraDocumentHtml(User::AdministrativeHeader); break;
            case User::AdministrativeFooter : toReturn = user->extraDocumentHtml(User::AdministrativeFooter); break;
            case User::AdministrativeWatermark : toReturn = user->extraDocumentHtml(User::AdministrativeWatermark); break;
            case User::PrescriptionHeader : toReturn = user->extraDocumentHtml(User::AdministrativeHeader); break;
            case User::PrescriptionFooter : toReturn = user->extraDocumentHtml(User::PrescriptionFooter); break;
            case User::PrescriptionWatermark : toReturn = user->extraDocumentHtml(User::PrescriptionWatermark); break;

            case User::IsModified : toReturn = user->isModified(); break;
            case User::ManagerRights : toReturn = user->rightsValue(USER_ROLE_USERMANAGER); break;
            case User::MedicalRights : toReturn = user->rightsValue(USER_ROLE_MEDICAL); break;
            case User::DrugsRights : toReturn = user->rightsValue(USER_ROLE_DOSAGES); break;
            case User::ParamedicalRights : toReturn = user->rightsValue(USER_ROLE_PARAMEDICAL); break;
            case User::AdministrativeRights : toReturn = user->rightsValue(USER_ROLE_ADMINISTRATIVE); break;
            case User::LoginHistory : toReturn = user->loginHistory(); break;
            case User::Warn : user->warn(); break;
            case User::WarnText : toReturn = user->warnText(); break;
            default : toReturn = QVariant();
        };
    }

//    qWarning() << "end data" << toReturn;

    return toReturn;
}

/** Not implemented */
void UserModel::setSort(int /*column*/, Qt::SortOrder /*order*/)
{
}
/** Not implemented */
void UserModel::sort(int /*column*/, Qt::SortOrder /*order*/)
{
}
/** Not implemented */
void UserModel::setTable(const QString &/*tableName*/)
{
}
/** Not implemented */
void UserModel::setFilter(const QString &/*filter*/)
{
}

/** \brief Returns true if model has dirty rows that need to be saved into database. */
bool UserModel::hasUserToSave()
{
    foreach(const Internal::UserData *u, d->m_Uuid_UserList.values())
        if (u->isModified())
            return true;
    return false;
}

/** \brief Submit all changes of the model into database */
bool UserModel::submitAll()
{
    bool toReturn = true;
    int i = 0;
    foreach(const QString &s, d->m_Uuid_UserList.keys()) {
        if (! submitUser(s))
            toReturn = false;
        else
            i++;
    }
    emit memoryUsageChanged();
    return toReturn;
}

/** \brief Submit only one user changes of the model into database */
bool UserModel::submitUser(const QString &uuid)
{
    bool toReturn = true;
    QModelIndexList list = match(createIndex(0, User::Uuid), Qt::DisplayRole, uuid, 1);
    if (list.count() != 1)
        return false;
    // act only on modified users
    if (d->m_Uuid_UserList.value(uuid)->isModified()) {
        Internal::UserData * user = d->m_Uuid_UserList[uuid];
        // check user write rights
        if ((user->isCurrent()) &&
             (! d->m_CurrentUserRights &User::WriteOwn))
            toReturn = false;
        else if ((! user->isCurrent()) &&
                  (! d->m_CurrentUserRights &User::WriteAll))
            toReturn = false;
        else if (!Internal::UserBase::instance()->saveUser(user))
            toReturn = false;
    }
    emit dataChanged(index(list.at(0).row(),0) , index(list.at(0).row(), User::MaxParam));
    return toReturn;
}

/** \brief Reverts the model. */
bool UserModel::revertAll()
{
    // TODO ?? --> ASSERT failure in QSqlTableModelPrivate::revertCachedRow(): "Invalid entry in cache map", file models\qsqltablemodel.cpp, line 151
    int i = 0;
    for(i=0; i < rowCount() ; i++)
        revertRow(i);
    select();
    reset();
    return true;
}

/** \brief Revert a row */
void UserModel::revertRow(int row)
{
    QString uuid = QSqlTableModel::index(row, USER_UUID).data().toString();
    QSqlTableModel::revertRow(row);
    if (d->m_Uuid_UserList.keys().contains(uuid))
    {
        delete d->m_Uuid_UserList[uuid];
        d->m_Uuid_UserList[uuid] = 0;
        d->m_Uuid_UserList.remove(uuid);
    }
//    select();
    reset();
    emit memoryUsageChanged();
}

/**
  \brief Define the filter of the model.
  \sa Utils::Database::getWhereClause()
*/
void UserModel::setFilter (const QHash<int,QString> &conditions)
{
    // TODO filter by name AND surname at the same time
    QString filter = "";
    const Internal::UserBase *b = Internal::UserBase::instance();
    foreach(const int r, conditions.keys()) {
        QString baseField = "";
        switch (r)
        {
        case User::Name : baseField = b->field(Table_USERS, USER_NAME); break;
        case User::Surname : baseField = b->field(Table_USERS, USER_SURNAME); break;
        default: break;
        }
        if (baseField.isEmpty()) continue;
        filter += QString("(`%1` %2) AND \n").arg(baseField, conditions.value(r));
    }
    filter.chop(5);
    QSqlTableModel::setFilter(filter);
    qWarning() << filter;
}

/** \brief Returns the number of user stored into the memory. */
int UserModel::numberOfUsersInMemory()
{
    return d->m_Uuid_UserList.count();
}

/** \brief For debugging purpose only */
void UserModel::warn()
{
    if (!Utils::isDebugCompilation())
        return;
    qWarning() << "UserModel Warning";
    qWarning() << d->m_CurrentUserUuid;
    qWarning() << d->m_Uuid_UserList;
}
