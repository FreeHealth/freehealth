/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
  The current user represents the actually logged user. Before the current user can be changed (disconnection,
  another user connection) IUserListener registered in the PluginManager are asked.
  Set filter with setFilter().

  \sa UserData
  \sa IUserListener

  \todo write documentation+++
  \todo code LOCKER
  \todo when QDataWidgetMapper (UserViewer) is setted, it calls ALL the datas of the user, even for the hidden widgets. This causes an important memory usage. This is to improve ++++

  \ingroup usertoolkit object_usertoolkit
  \ingroup usermanager
*/

#include "usermodel.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>

#include <printerplugin/textdocumentextra.h>

#include <usermanagerplugin/database/userbase.h>
#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/iuserlistener.h>

#include <extensionsystem/pluginmanager.h>

#include <QApplication>
#include <QUuid>
#include <QColor>
#include <QByteArray>
#include <QFont>
#include <QHash>

using namespace UserPlugin;
using namespace UserPlugin::Constants;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return UserPlugin::Internal::UserBase::instance();}


namespace UserPlugin {
namespace Internal {

/** \brief UserModel wrapper can be accessed using Core::ICore::instance()->user() */
class UserModelWrapper : public Core::IUser
{
public:
    UserModelWrapper(UserModel *model) :
            Core::IUser(), m_Model(model) {}

    ~UserModelWrapper() {}

    // IPatient interface
    void clear() {}
    bool has(const int ref) const {return (ref>=0 && ref<Core::IUser::NumberOfColumns);}

    QVariant value(const int ref) const {return m_Model->currentUserData(ref);}
    bool setValue(const int ref, const QVariant &value) {return m_Model->setData(m_Model->index(m_Model->currentUserIndex().row(), ref), value);}

    /** \todo Is this needed in freemedforms ? */
    QString toXml() const {return QString();}
    bool fromXml(const QString &xml) {return true;}

private:
    UserModel *m_Model;
};



/**
  \brief Private Part.
  \internal
  \ingroup usertoolkit widget_usertoolkit usermanager
*/
class UserModelPrivate
{
public:
    UserModelPrivate(UserModel *parent) : m_UserModelWrapper(new UserModelWrapper(parent)) {}
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
    UserModelWrapper *m_UserModelWrapper;
    QHash<QString, UserData *> m_Uuid_UserList;
    QString m_CurrentUserUuid;
    Core::IUser::UserRights m_CurrentUserRights;
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
    d = new Internal::UserModelPrivate(this);

    // install the Core Patient wrapper
    Core::ICore::instance()->setUser(d->m_UserModelWrapper);

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
    if ((col >= 0) && (col < Core::IUser::NumberOfColumns))
        return QAbstractItemModel::createIndex(row, col, 0);
    return QModelIndex();
}

/**
  \brief Defines the current user using its login and password. There can be only one current user.
  The date and time of loggin are trace into database.

  \todo Create a UserChangerListener +++ instead of using sig/slot

*/
bool UserModel::setCurrentUser(const QString &log64, const QString &cryptpass64)
{
    QList<IUserListener *> listeners = pluginManager()->getObjects<IUserListener>();

    // 1. Ask all listeners to prepare the current user disconnection
    foreach(IUserListener *l, listeners) {
        if (!l->userAboutToChange())
            return false;
    }

    // 2. Get user from Database
    QString uuid = d->addUserFromDatabase(log64, cryptpass64);
    if (uuid.isEmpty()) {
        Utils::Log::addError(this, tr("Unable to retreive user into the model using login and password."));
        return false;
    }

    // 3. Ask all listeners for the current user disconnection
    foreach(IUserListener *l, listeners) {
        if (!l->currentUserAboutToDisconnect())
            return false;
    }

    // 4. Connect new user
    Utils::Log::addMessage(this, tr("Setting current user uuid to %1").arg(uuid));
    if (!d->m_CurrentUserUuid.isEmpty()) {
        Q_EMIT userAboutToDisconnect(d->m_CurrentUserUuid);
    }
    d->m_CurrentUserUuid.clear();
    d->m_CurrentUserRights = Core::IUser::NoRights;
    d->m_CurrentUserUuid = uuid;
    foreach(Internal::UserData *u, d->m_Uuid_UserList.values())
        u->setCurrent(false);

    Q_EMIT(userAboutToConnect(uuid));
    // trace log
    Internal::UserData *user = d->m_Uuid_UserList[d->m_CurrentUserUuid];
    user->setCurrent(true);
    user->setLastLogin(QDateTime::currentDateTime());
    user->addLoginToHistory();
    Internal::UserBase::instance()->saveUser(user);
    user->setModified(false);
    d->m_CurrentUserUuid = uuid;
    d->m_CurrentUserRights = Core::IUser::UserRights(user->rightsValue(USER_ROLE_USERMANAGER).toInt());
    Q_EMIT memoryUsageChanged();
    Q_EMIT userConnected(uuid);
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
//    qWarning() << "currentUserIndex" << rowCount() << d->m_CurrentUserUuid;
    if (d->m_CurrentUserUuid.isEmpty())
        return QModelIndex();
    QModelIndexList list = match(createIndex(0, Core::IUser::Uuid), Qt::DisplayRole, d->m_CurrentUserUuid, 1);
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
    if (! d->m_CurrentUserRights & Core::IUser::Delete)
        return false;
    bool noError = true;
    beginRemoveRows(QModelIndex(), row, row+count);
    /** \todo alert when user to delete is modified ? */
    /** \todo pb when userviewer is showing the index to delete */
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
    if (! d->m_CurrentUserRights & Core::IUser::Create)
        return false;
    int i=0;
    for (i=0;i<count;i++)
    {
        // create a new empty tkUser
        if (!QSqlTableModel::insertRows(row + i, 1, parent)) {
            Utils::Log::addError(this, QString("Can not create a new user into SQL Table."));
            return i;
        }
        // feed the QSqlTableModel with uuid and crypted empty password
        QString uuid = d->createNewEmptyUser(this, row+i);
        QModelIndex newIndex = index(row+i, Core::IUser::Uuid);
        if (!QSqlTableModel::setData(newIndex, uuid, Qt::EditRole)) {
            Utils::Log::addError(this, QString("Can not add user's uuid into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg(row+i).arg(uuid));
            return i;
        }
        newIndex = index(row+i, Core::IUser::Password);
        if (!QSqlTableModel::setData(newIndex, UserPlugin::crypt(""), Qt::EditRole)) {
            Utils::Log::addError(this, QString("Can not add user's login into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg(row+i).arg(uuid));
            return i;
        }
        // define a lkid for this user
        int maxLkId = userBase()->getMaxLinkId();
        qWarning() << maxLkId;
        /** \todo user already have a lkid ? --> manage this */
        QSqlQuery query(database());
        query.prepare(userBase()->prepareInsertQuery(Constants::Table_USER_LK_ID));
        query.bindValue(Constants::LK_ID, QVariant());
        query.bindValue(Constants::LK_GROUP_UUID, QVariant());
        query.bindValue(Constants::LK_USER_UUID, uuid);
        query.bindValue(Constants::LK_LKID, maxLkId + 1);
        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        userBase()->updateMaxLinkId(maxLkId + 1);
    }
    emit memoryUsageChanged();
    return i;
}

int UserModel::columnCount(const QModelIndex &)
{
    return Core::IUser::NumberOfColumns;
}

/** \brief Define the datas of users.  */
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
        if (! d->m_CurrentUserRights & Core::IUser::WriteOwn)
            return false;
    } else if (! d->m_CurrentUserRights & Core::IUser::WriteAll)
        return false;
    /** \todo if user if a delegate of current user */

    // set datas directly into database using QSqlTableModel if possible
    if (item.column() < USER_MaxParam) {
        // prepare SQL update
        if (!QSqlTableModel::setData(item, value, role)) {
            Utils::Log::addError(this, QString("enable to setData to SqlModel. Row %1, col %2, data %3")
                             .arg(item.row()).arg(item.column()).arg(value.toString()));
            return false;
        }
        // poursue to feed UserData (need to know if it is modified)
    }

    switch (item.column())
    {
        case Core::IUser::Id :  user->setId(value); break;
        case Core::IUser::Uuid :  user->setUuid(value.toString()); break;
        case Core::IUser::Validity :  user->setValidity(value); break;
        case Core::IUser::Login :  user->setLogin(value); break;
        case Core::IUser::DecryptedLogin : user->setLogin(value.toString().toAscii().toBase64()); break;
        case Core::IUser::Password :  user->setCryptedPassword(value); break;
        case Core::IUser::LastLogin :  user->setLastLogin(value); break;
        case Core::IUser::GenderIndex : user->setGender(value); break;
        case Core::IUser::TitleIndex : user->setTitle(value); break;
        case Core::IUser::Name :  user->setName(value); break;
        case Core::IUser::SecondName :  user->setSecondName(value); break;
        case Core::IUser::Surname :  user->setSurname(value); break;
        case Core::IUser::Mail :  user->setMail(value); break;
        case Core::IUser::Language :  user->setLanguage(value); break;
        case Core::IUser::LanguageIndex :
            {
                if (Core::Translators::availableLocales().count() < value.toInt())
                    user->setLanguage(Core::Translators::availableLocales().at(value.toInt()));
                break;
            }
        case Core::IUser::Adress :  user->setAdress(value); break;
        case Core::IUser::Zipcode :  user->setZipcode(value); break;
        case Core::IUser::City :  user->setCity(value); break;
        case Core::IUser::Country :  user->setCountry(value); break;
        case Core::IUser::Tel1 :  user->setTel1(value); break;
        case Core::IUser::Tel2 :  user->setTel2(value); break;
        case Core::IUser::Tel3 :  user->setTel3(value); break;
        case Core::IUser::Fax :  user->setFax(value); break;
        case Core::IUser::PractitionerId :  user->setPractitionerIdentifiant(value.toStringList()); break;
        case Core::IUser::Specialities :  user->setSpecialty(value.toStringList()); break;
        case Core::IUser::Qualifications :  user->setQualification(value.toStringList()); break;
        case Core::IUser::Preferences :  user->setPreferences(value); break;

        /** \todo Add Xml extra document to model */
        case Core::IUser::GenericHeader : user->setExtraDocumentHtml(value, Core::IUser::GenericHeader); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::GenericFooter :  user->setExtraDocumentHtml(value, Core::IUser::GenericFooter); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::GenericWatermark :  user->setExtraDocumentHtml(value, Core::IUser::GenericWatermark); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::AdministrativeHeader : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeHeader); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::AdministrativeFooter : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeFooter); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::AdministrativeWatermark : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeWatermark); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::PrescriptionHeader : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeHeader); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::PrescriptionFooter : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionFooter); Q_EMIT(userDocumentsChanged()); break;
        case Core::IUser::PrescriptionWatermark : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionWatermark); Q_EMIT(userDocumentsChanged()); break;

        case Core::IUser::ManagerRights : user->setRights(USER_ROLE_USERMANAGER, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::MedicalRights : user->setRights(USER_ROLE_MEDICAL, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::DrugsRights : user->setRights(USER_ROLE_DOSAGES, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::ParamedicalRights : user->setRights(USER_ROLE_PARAMEDICAL, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::AdministrativeRights : user->setRights(USER_ROLE_ADMINISTRATIVE, Core::IUser::UserRights(value.toInt())); break;
        default : return false;
    };
    emit dataChanged(index(item.row(), 0), index(item.row(), this->columnCount()));
    return true;
}

/** \brief Returns the datas of users. \sa Core::IUser::Model */
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
    if ((item.column() < Core::IUser::LanguageIndex)) {
        // here we suppose that it is the currentUser the ask for datas
        /** \todo had delegates rights */
        if (d->m_CurrentUserRights & Core::IUser::ReadAll)
            return QSqlTableModel::data(item, role);
        else if ((d->m_CurrentUserRights & Core::IUser::ReadOwn) &&
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
        if (!d->m_CurrentUserRights & Core::IUser::ReadOwn)
            return QVariant();
    } else if (!d->m_CurrentUserRights & Core::IUser::ReadAll)
            return QVariant();
    /** \todo if user if a delegate of current user */

    // get the requiered data
    QVariant toReturn;

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        // get datas directly from database using QSqlTableModel if possible
        if (item.column() < USER_LANGUAGE)
            return QSqlTableModel::data(item, role);

        switch (item.column())
        {
            case Core::IUser::Id : toReturn = user->id(); break;
            case Core::IUser::Uuid : toReturn = user->uuid(); break;
            case Core::IUser::Validity : toReturn = user->validity(); break;
            case Core::IUser::Login : toReturn = user->login(); break;
            case Core::IUser::DecryptedLogin : toReturn = user->decryptedLogin(); break;
            case Core::IUser::Password : toReturn = user->cryptedPassword(); break;
            case Core::IUser::LastLogin : toReturn = user->lastLogin(); break;
            case Core::IUser::GenderIndex : toReturn = user->genderIndex(); break;
            case Core::IUser::TitleIndex : toReturn = user->title(); break;
            case Core::IUser::Gender : toReturn = genders().at(user->genderIndex()); break;
            case Core::IUser::Title : toReturn = titles().at(user->title()); break;
            case Core::IUser::Name : toReturn = user->name(); break;
            case Core::IUser::SecondName : toReturn = user->secondName(); break;
            case Core::IUser::Surname : toReturn = user->surname(); break;
            case Core::IUser::FullName :
                {
                    QString r = user->title() + " " + user->name() + " " + user->secondName() + " " + user->surname();
                    r.replace("  ", " ");
                    toReturn = r;
                    break;
                }
            case Core::IUser::Mail : toReturn = user->mail(); break;
            case Core::IUser::Language : toReturn = user->language(); break;
            case Core::IUser::LanguageIndex : toReturn = Core::Translators::availableLocales().indexOf(user->language()); break;
            case Core::IUser::Adress : toReturn = user->adress(); break;
            case Core::IUser::Zipcode : toReturn = user->zipcode(); break;
            case Core::IUser::City : toReturn = user->city(); break;
            case Core::IUser::Country : toReturn = user->country(); break;
            case Core::IUser::Tel1 : toReturn = user->tels().at(0); break;
            case Core::IUser::Tel2 : toReturn = user->tels().at(1); break;
            case Core::IUser::Tel3 : toReturn = user->tels().at(2); break;
            case Core::IUser::Fax : toReturn = user->fax(); break;
            case Core::IUser::PractitionerId : toReturn = user->practitionerId(); break;
            case Core::IUser::Specialities : toReturn = user->specialty(); break;
            case Core::IUser::Qualifications : toReturn = user->qualifications(); break;
            case Core::IUser::Preferences : toReturn = user->preferences(); break;

            case Core::IUser::GenericHeader : toReturn = user->extraDocumentHtml(Core::IUser::GenericHeader); break;
            case Core::IUser::GenericFooter : toReturn = user->extraDocumentHtml(Core::IUser::GenericFooter); break;
            case Core::IUser::GenericWatermark :  toReturn = user->extraDocumentHtml(Core::IUser::GenericWatermark); break;
            case Core::IUser::AdministrativeHeader : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeHeader); break;
            case Core::IUser::AdministrativeFooter : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeFooter); break;
            case Core::IUser::AdministrativeWatermark : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeWatermark); break;
            case Core::IUser::PrescriptionHeader : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeHeader); break;
            case Core::IUser::PrescriptionFooter : toReturn = user->extraDocumentHtml(Core::IUser::PrescriptionFooter); break;
            case Core::IUser::PrescriptionWatermark : toReturn = user->extraDocumentHtml(Core::IUser::PrescriptionWatermark); break;

            case Core::IUser::IsModified : toReturn = user->isModified(); break;
            case Core::IUser::ManagerRights : toReturn = user->rightsValue(USER_ROLE_USERMANAGER); break;
            case Core::IUser::MedicalRights : toReturn = user->rightsValue(USER_ROLE_MEDICAL); break;
            case Core::IUser::DrugsRights : toReturn = user->rightsValue(USER_ROLE_DOSAGES); break;
            case Core::IUser::ParamedicalRights : toReturn = user->rightsValue(USER_ROLE_PARAMEDICAL); break;
            case Core::IUser::AdministrativeRights : toReturn = user->rightsValue(USER_ROLE_ADMINISTRATIVE); break;
            case Core::IUser::LoginHistory : toReturn = user->loginHistory(); break;
            case Core::IUser::Warn : user->warn(); break;
            case Core::IUser::WarnText : toReturn = user->warnText(); break;
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
    QModelIndexList list = match(createIndex(0, Core::IUser::Uuid), Qt::DisplayRole, uuid, 1);
    if (list.count() != 1)
        return false;
    // act only on modified users
    if (d->m_Uuid_UserList.value(uuid)->isModified()) {
        Internal::UserData * user = d->m_Uuid_UserList[uuid];
        // check user write rights
        if ((user->isCurrent()) &&
             (! d->m_CurrentUserRights & Core::IUser::WriteOwn))
            toReturn = false;
        else if ((! user->isCurrent()) &&
                  (! d->m_CurrentUserRights & Core::IUser::WriteAll))
            toReturn = false;
        else if (!Internal::UserBase::instance()->saveUser(user))
            toReturn = false;
    }
    emit dataChanged(index(list.at(0).row(),0) , index(list.at(0).row(), Core::IUser::NumberOfColumns));
    return toReturn;
}

bool UserModel::submitRow(const int row)
{
    return submitUser(index(row, Core::IUser::Uuid).data().toString());
}

/** \brief Reverts the model. */
bool UserModel::revertAll()
{
    /** \todo ASSERT failure in QSqlTableModelPrivate::revertCachedRow(): "Invalid entry in cache map", file models\qsqltablemodel.cpp, line 151 */
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
    if (d->m_Uuid_UserList.keys().contains(uuid)) {
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
    /** \todo filter by name AND surname at the same time */
    QString filter = "";
    const Internal::UserBase *b = Internal::UserBase::instance();
    foreach(const int r, conditions.keys()) {
        QString baseField = "";
        switch (r)
        {
        case Core::IUser::Name : baseField = b->field(Table_USERS, USER_NAME); break;
        case Core::IUser::Surname : baseField = b->field(Table_USERS, USER_SURNAME); break;
        default: break;
        }
        if (baseField.isEmpty()) continue;
        filter += QString("(`%1` %2) AND \n").arg(baseField, conditions.value(r));
    }
    filter.chop(5);
    QSqlTableModel::setFilter(filter);
//    qWarning() << filter;
}

QList<int> UserModel::practionnerLkIds(const QString &uid)
{
    /** \todo manage user's groups */
    if (d->m_Uuid_UserList.keys().contains(uid)) {
        Internal::UserData *user = d->m_Uuid_UserList.value(uid);
        return user->linkIds();
    }
    QList<int> lk_ids;
    if (uid.isEmpty())
        return lk_ids;

    QHash<int, QString> where;
    where.clear();
    where.insert(Constants::LK_USER_UUID, QString("='%1'").arg(uid));
    QString req = userBase()->select(Constants::Table_USER_LK_ID, Constants::LK_LKID, where);
    QSqlQuery query(req, userBase()->database());
    if (query.isActive()) {
        while (query.next())
            lk_ids.append(query.value(0).toInt());
    } else {
        Utils::Log::addQueryError("UserModel", query);
    }
    return lk_ids;
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
    qWarning() << "  * Current user uuid" << d->m_CurrentUserUuid;
    qWarning() << "  * Current users list" << d->m_Uuid_UserList;
}
