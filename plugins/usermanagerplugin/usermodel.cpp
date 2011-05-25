/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
/**
  \class UserPlugin::UserModel
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

namespace {
    const char * const SERVER_ADMINISTRATOR_UUID = "serverAdmin";
}

namespace UserPlugin {
namespace Internal {

UserModelWrapper::UserModelWrapper(UserModel *model) :
        Core::IUser(model), m_Model(model)
{
    connect(model, SIGNAL(userConnected(QString)), this, SLOT(newUserConnected(QString)));
}

UserModelWrapper::~UserModelWrapper()
{
    // remove this wrapper from the core implementation
    Core::ICore::instance()->setUser(0);
}

// IPatient interface
bool UserModelWrapper::hasCurrentUser() const {return m_Model->hasCurrentUser();}

QVariant UserModelWrapper::value(const int ref) const {return m_Model->currentUserData(ref);}

bool UserModelWrapper::setValue(const int ref, const QVariant &value)
{
    if (m_Model->setData(m_Model->index(m_Model->currentUserIndex().row(), ref), value)) {
        Q_EMIT this->userDataChanged(ref);
        return true;
    }
    return false;
}

bool UserModelWrapper::saveChanges()
{
    if (m_Model) {
        return m_Model->submitUser(value(Core::IUser::Uuid).toString());
    }
    return false;
}

void UserModelWrapper::newUserConnected(const QString &uid)
{
    Q_EMIT userChanged();
}


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
        UserData *un = userBase()->getUserByUuid(uuid);
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
        QString uuid = userBase()->getUuid(log64, pass64);
        if (uuid.isEmpty())
            return QString();
        // make sure it is not already in the hash
        if (m_Uuid_UserList.keys().contains(uuid)) {
            return uuid;
        }
        m_Uuid_UserList.insert(uuid, userBase()->getUserByUuid(uuid));
        return uuid;
    }

    /** \brief Create and empty user into the model. The uuid of the user is automatically setted and returned. */
    QString createNewEmptyUser(UserModel *model, const int createdRow)
    {
        // 1. create an empty user into the hash
        QString uuid = userBase()->createNewUuid();
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
        : QSqlTableModel(parent, userBase()->database()), d(0)
{
    setObjectName("UserModel");
    d = new Internal::UserModelPrivate(this);

    // install the Core Patient wrapper
    Core::ICore::instance()->setUser(d->m_UserModelWrapper);

    if (!parent)
        setParent(qApp);
    QSqlTableModel::setTable(userBase()->table(Table_USERS));
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
}

/** \brief Destructor */
UserModel::~UserModel()
{
    if (d) delete d;
    d=0;
}

/**
  \brief Defines the current user using its login and password. There can be only one current user.
  The date and time of loggin are trace into database.
  \todo Create a UserChangerListener +++ instead of using sig/slot
*/
bool UserModel::setCurrentUser(const QString &log64, const QString &cryptpass64, bool refreshCache)
{
//    qWarning() << Q_FUNC_INFO << log64 << cryptpass64;
    QList<IUserListener *> listeners = pluginManager()->getObjects<IUserListener>();

    // 1. Ask all listeners to prepare the current user disconnection
    foreach(IUserListener *l, listeners) {
        if (!l->userAboutToChange())
            return false;
    }

    // 2. Check "in memory" users
    QString uuid;
    foreach(Internal::UserData *u, d->m_Uuid_UserList.values()) {
        if (u->login()==log64 && u->cryptedPassword()==cryptpass64) {
            if (!refreshCache) {
                uuid = u->uuid();
                break;
            } else {
                d->m_Uuid_UserList.remove(u->uuid());
                delete u;
                u = 0;
                break;
            }
        }
    }

    // 3. Get user from Database
    if (uuid.isEmpty()) {
        uuid = d->addUserFromDatabase(log64, cryptpass64);
    }
    if (uuid.isEmpty()) {
        LOG_ERROR(tr("Unable to retreive user into the model using login and password."));
        return false;
    }

    // 3. Ask all listeners for the current user disconnection
    foreach(IUserListener *l, listeners) {
        if (!l->currentUserAboutToDisconnect())
            return false;
    }

    // 4. Connect new user
    LOG(tr("Setting current user uuid to %1").arg(uuid));
    if (!d->m_CurrentUserUuid.isEmpty()) {
        Q_EMIT userAboutToDisconnect(d->m_CurrentUserUuid);
    }
    d->m_CurrentUserUuid.clear();
    d->m_CurrentUserRights = Core::IUser::NoRights;
    d->m_CurrentUserUuid = uuid;
    foreach(Internal::UserData *u, d->m_Uuid_UserList.values())
        u->setCurrent(false);

    // 5. If precedent currentUser was SERVER_ADMINISTRATOR_UUID --> remove it from the cache
    /** \todo code here */

    Q_EMIT(userAboutToConnect(uuid));

    // trace log
    Internal::UserData *user = d->m_Uuid_UserList[d->m_CurrentUserUuid];
    user->setCurrent(true);
    user->setLastLogin(QDateTime::currentDateTime());
    user->addLoginToHistory();
    userBase()->saveUser(user);
    user->setModified(false);
    d->m_CurrentUserUuid = uuid;
    /** \todo this is not the usermanger role if asked uuid == currentuser */
    d->m_CurrentUserRights = Core::IUser::UserRights(user->rightsValue(USER_ROLE_USERMANAGER).toInt());

    LOG(tkTr(Trans::Constants::CONNECTED_AS_1).arg(user->fullName()));
    Q_EMIT memoryUsageChanged();
    Q_EMIT userConnected(uuid);

    return true;
}

/**
  Define the current user as the database server manager. The unique user that can create
  users at startup time.
  \sa UserPlugin::UserCreationPage
*/
bool UserModel::setCurrentUserIsServerManager()
{
    if (userBase()->database().driverName()=="QSQLITE") {
        return false;
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(database().connectionName().arg(database().lastError().text())));
            return false;
        }
    }
//    qWarning() << Q_FUNC_INFO << log64 << cryptpass64;
        QList<IUserListener *> listeners = pluginManager()->getObjects<IUserListener>();

        // 1. Ask all listeners to prepare the current user disconnection
        foreach(IUserListener *l, listeners) {
            if (!l->userAboutToChange())
                return false;
        }

        // 2. Check "in memory" users
        QString uuid = ::SERVER_ADMINISTRATOR_UUID;
        Internal::UserData *u = d->m_Uuid_UserList.value(uuid);
        if (!u) {
            u = new Internal::UserData(uuid);
            u->setName(tr("Database server administrator"));
            u->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::AllRights);
            d->m_Uuid_UserList.insert(uuid, u);
        }

        // 3. Ask all listeners for the current user disconnection
        foreach(IUserListener *l, listeners) {
            if (!l->currentUserAboutToDisconnect())
                return false;
        }

        // 4. Connect new user
        LOG(tr("Setting current user uuid to %1").arg(uuid));
        if (!d->m_CurrentUserUuid.isEmpty()) {
            Q_EMIT userAboutToDisconnect(d->m_CurrentUserUuid);
        }
        d->m_CurrentUserUuid.clear();
        d->m_CurrentUserRights = Core::IUser::AllRights;
        d->m_CurrentUserUuid = uuid;
        foreach(Internal::UserData *user, d->m_Uuid_UserList.values())
            user->setCurrent(false);
        u->setCurrent(true);

        Q_EMIT(userAboutToConnect(uuid));

        u->setModified(false);

        LOG(tkTr(Trans::Constants::CONNECTED_AS_1).arg(u->fullName()));
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
//    qWarning() << Q_FUNC_INFO << rowCount() << d->m_CurrentUserUuid;
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
    return userBase()->database();
}

/** \brief Clears the content of the model. Silently save users if needed. */
void UserModel::clear()
{
    submitAll();
    d->m_CurrentUserRights = 0;
    d->m_CurrentUserUuid.clear();
    qDeleteAll(d->m_Uuid_UserList);
    d->m_Uuid_UserList.clear();
}

/** \brief Check login/password validity. \sa UserBase::checkLogin(). */
bool UserModel::isCorrectLogin(const QString &logbase64, const QString &cryptpassbase64)
{
    return userBase()->checkLogin(logbase64, cryptpassbase64);
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
            LOG(tr("Default user can not be deleted from database."));
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
               LOG_ERROR(tr("You can not delete a modified user, save it before."));
                noError = false;
            } else {
                delete d->m_Uuid_UserList[uuid];
                d->m_Uuid_UserList[uuid] = 0;
                d->m_Uuid_UserList.remove(uuid);
            }
        }

        // Delete from real database
        if (!userBase()->deleteUser(uuid)) {
           LOG_ERROR(tr("User can not be deleted from database."));
            noError = false;
        }
    }
    endRemoveRows();
    QSqlTableModel::select();
    reset(); // needed
    Q_EMIT memoryUsageChanged();
    return noError;
}

/** \brief Create a new row (new user) into the model. */
bool UserModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!d->m_CurrentUserRights & Core::IUser::Create)
        return false;
    int i=0;
    for (i=0;i<count;i++) {
        // create a new empty tkUser
        if (!QSqlTableModel::insertRows(row + i, 1, parent)) {
           LOG_ERROR(QString("Can not create a new user into SQL Table."));
            return i;
        }
        // feed the QSqlTableModel with uuid and crypted empty password
        QString uuid = d->createNewEmptyUser(this, row+i);
        Internal::UserData *user = d->m_Uuid_UserList.value(uuid);
        QModelIndex newIndex = index(row+i, Core::IUser::Uuid);
        if (!QSqlTableModel::setData(newIndex, uuid, Qt::EditRole)) {
           LOG_ERROR(QString("Can not add user's uuid into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg(row+i).arg(uuid));
            return i;
        }
        newIndex = index(row+i, Core::IUser::Password);
        if (!QSqlTableModel::setData(newIndex, UserPlugin::crypt(""), Qt::EditRole)) {
           LOG_ERROR(QString("Can not add user's login into the new user into SQL Table. Row = %1 , UUID = %2 ")
                             .arg(row+i).arg(uuid));
            return i;
        }
        // define a lkid for this user
        int maxLkId = userBase()->getMaxLinkId();
        /** \todo user already have a lkid ? --> manage this */
        QSqlQuery query(database());
        query.prepare(userBase()->prepareInsertQuery(Constants::Table_USER_LK_ID));
        query.bindValue(Constants::LK_ID, QVariant());
        query.bindValue(Constants::LK_GROUP_UUID, QVariant());
        query.bindValue(Constants::LK_USER_UUID, uuid);
        query.bindValue(Constants::LK_LKID, maxLkId + 1);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
        }
        userBase()->updateMaxLinkId(maxLkId + 1);
        user->setLkIds(QList<int>() << maxLkId+1);
    }
    Q_EMIT memoryUsageChanged();
    return i;
}

int UserModel::columnCount(const QModelIndex &)
{
    return Core::IUser::NumberOfColumns;
}

/** \brief Define the datas of users.  */
bool UserModel::setData(const QModelIndex &item, const QVariant &value, int role)
{
    if (!value.isValid())
        return false;

    if(!item.isValid())
        return false;

    // work only for EditRole
    if (role != Qt::EditRole)
        return false;

    // get uuid from real database
    QString uuid = QSqlTableModel::data(QSqlTableModel::index(item.row(), USER_UUID), Qt::DisplayRole).toString();
    if (!d->m_Uuid_UserList.keys().contains(uuid)) {
        d->addUserFromDatabase(uuid);
        Q_EMIT memoryUsageChanged();
    }

    Internal::UserData *user = d->m_Uuid_UserList[uuid];
    // check user write rights
    if (user->isCurrent()) {
        if (!d->m_CurrentUserRights & Core::IUser::WriteOwn)
            return false;
    } else if (!d->m_CurrentUserRights & Core::IUser::WriteAll)
        return false;
    /** \todo if user if a delegate of current user */

    // set datas directly into database using QSqlTableModel if possible
    if (item.column() < USER_MaxParam) {
        // prepare SQL update
        if (!QSqlTableModel::setData(item, value, role)) {
           LOG_ERROR(QString("enable to setData to SqlModel. Row %1, col %2, data %3")
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
        case Core::IUser::Firstname :  user->setFirstname(value); break;
        case Core::IUser::Mail :  user->setMail(value); break;
        case Core::IUser::Language :  user->setLanguage(value); break;
        case Core::IUser::LanguageIndex :
            {
                if (value.toInt() < Core::Translators::availableLocales().count())
                    user->setLanguage(Core::Translators::availableLocales().at(value.toInt()));
                break;
            }
        case Core::IUser::LocaleCodedLanguage: user->setLanguage(QLocale(QLocale::Language(value.toInt())).name().left(2)); break;
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
    case Core::IUser::GenericHeaderPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::GenericHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericFooterPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::GenericFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::GenericWatermarkPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::GenericWatermark); Q_EMIT(userDocumentsChanged()); break;

    case Core::IUser::AdministrativeHeader : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeFooter : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeWatermark : user->setExtraDocumentHtml(value, Core::IUser::AdministrativeWatermark); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeHeaderPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::AdministrativeHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeFooterPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::AdministrativeFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::AdministrativeWatermarkPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::AdministrativeWatermark); Q_EMIT(userDocumentsChanged()); break;

    case Core::IUser::PrescriptionHeader : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionFooter : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionWatermark : user->setExtraDocumentHtml(value, Core::IUser::PrescriptionWatermark); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionHeaderPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::PrescriptionHeader); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionFooterPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::PrescriptionFooter); Q_EMIT(userDocumentsChanged()); break;
    case Core::IUser::PrescriptionWatermarkPresence : user->setExtraDocumentPresence(value.toInt(), Core::IUser::PrescriptionWatermark); Q_EMIT(userDocumentsChanged()); break;

        case Core::IUser::ManagerRights : user->setRights(USER_ROLE_USERMANAGER, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::MedicalRights : user->setRights(USER_ROLE_MEDICAL, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::DrugsRights : user->setRights(USER_ROLE_DOSAGES, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::ParamedicalRights : user->setRights(USER_ROLE_PARAMEDICAL, Core::IUser::UserRights(value.toInt())); break;
        case Core::IUser::AdministrativeRights : user->setRights(USER_ROLE_ADMINISTRATIVE, Core::IUser::UserRights(value.toInt())); break;
        default : return false;
    };
    Q_EMIT dataChanged(index(item.row(), 0), index(item.row(), this->columnCount()));
    return true;
}

/** \brief Returns the datas of users. \sa Core::IUser::Model */
QVariant UserModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    // get user
    QString uuid = QSqlTableModel::data(QSqlTableModel::index(item.row(), USER_UUID), Qt::DisplayRole).toString();
    QVariant toReturn;

    // First manage decoration WITHOUT retreiving any user from database
    if (role == Qt::FontRole)
    {
        QFont font;
        if (d->m_Uuid_UserList.keys().contains(uuid)) {
            Internal::UserData *user = d->m_Uuid_UserList.value(uuid);
            if (user->isModified())
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
    else if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
    {
        // Manage table USERS using the QSqlTableModel WITHOUT retreiving whole user from database
        if ((item.column() < Core::IUser::LanguageIndex)) {
            // here we suppose that it is the currentUser the ask for datas
//            qWarning() << (bool)(d->m_CurrentUserRights & Core::IUser::ReadAll) << (bool)(d->m_CurrentUserRights & Core::IUser::ReadOwn) << (d->m_CurrentUserUuid == uuid);
            /** \todo had delegates rights */
            if (d->m_CurrentUserRights & Core::IUser::ReadAll)
                return QSqlTableModel::data(item, role);
            else if (d->m_CurrentUserUuid == uuid)
                return QSqlTableModel::data(item, role);
        }

        // Here we must get values from complete user, so retreive it from database if necessary
        if (!d->m_Uuid_UserList.keys().contains(uuid)) {
            d->addUserFromDatabase(uuid);
            Q_EMIT memoryUsageChanged();
        }
        const Internal::UserData *user = d->m_Uuid_UserList.value(uuid);
        // check user write rights
        if (user->isCurrent()) {
            if (!d->m_CurrentUserRights & Core::IUser::ReadOwn)
                return QVariant();
        } else if (!d->m_CurrentUserRights & Core::IUser::ReadAll)
            return QVariant();
        /** \todo if user if a delegate of current user */

        // get datas directly from database using QSqlTableModel if possible
        if (item.column() < USER_LANGUAGE)
            return QSqlTableModel::data(item, role);

        switch (item.column())
        {
        case Core::IUser::Id : toReturn = user->id(); break;
        case Core::IUser::PersonalLinkId: toReturn = user->personalLinkId(); break;
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
        case Core::IUser::Firstname : toReturn = user->firstname(); break;
        case Core::IUser::FullName : toReturn = user->fullName(); break;
        case Core::IUser::Mail : toReturn = user->mail(); break;
        case Core::IUser::Language : toReturn = user->language(); break;
        case Core::IUser::LanguageIndex : toReturn = Core::Translators::availableLocales().indexOf(user->language()); break;
        case Core::IUser::LocaleCodedLanguage: toReturn = QLocale(user->language()).language(); break;
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

        case Core::IUser::GenericHeaderPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::GenericHeader);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::GenericFooterPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::GenericFooter);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::GenericWatermarkPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::GenericWatermark);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::GenericWatermarkAlignement :
            {
                /** \todo return Watermark alignement */
                return Qt::AlignCenter;
            }

        case Core::IUser::AdministrativeHeader : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeHeader); break;
        case Core::IUser::AdministrativeFooter : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeFooter); break;
        case Core::IUser::AdministrativeWatermark : toReturn = user->extraDocumentHtml(Core::IUser::AdministrativeWatermark); break;
        case Core::IUser::AdministrativeWatermarkPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::AdministrativeWatermark);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::AdministrativeWatermarkAlignement :
            {
                /** \todo return Watermark alignement */
                return Qt::AlignCenter;
            }

        case Core::IUser::PrescriptionHeader : toReturn = user->extraDocumentHtml(Core::IUser::PrescriptionHeader); break;
        case Core::IUser::PrescriptionFooter : toReturn = user->extraDocumentHtml(Core::IUser::PrescriptionFooter); break;
        case Core::IUser::PrescriptionWatermark : toReturn = user->extraDocumentHtml(Core::IUser::PrescriptionWatermark); break;
        case Core::IUser::PrescriptionHeaderPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::PrescriptionHeader);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::PrescriptionFooterPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::PrescriptionFooter);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::PrescriptionWatermarkPresence :
            {
                Print::TextDocumentExtra *doc = user->extraDocument(Core::IUser::PrescriptionWatermark);
                if (doc)
                    return doc->presence();
                return Print::Printer::EachPages;
            }
        case Core::IUser::PrescriptionWatermarkAlignement :
            {
                /** \todo return Watermark alignement */
                return Qt::AlignCenter;
            }

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

bool UserModel::setPaper(const QString &uuid, const int ref, Print::TextDocumentExtra *extra)
{
    Internal::UserData *user = d->m_Uuid_UserList[uuid];
    if (!user)
        return false;
    user->setExtraDocument(extra, ref);
    user->setModified(true);
//    foreach(const Internal::UserDynamicData* data, user->modifiedDynamicDatas())
//        qWarning() << data->name() << data->isDirty();
//    qWarning();
    return true;
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
        if (!submitUser(s))
            toReturn = false;
        else
            i++;
    }
    Q_EMIT memoryUsageChanged();
    return toReturn;
}

/** \brief Submit only one user changes of the model into database according to the current user rights. */
bool UserModel::submitUser(const QString &uuid)
{
    bool toReturn = true;
    QModelIndexList list = match(index(0, Core::IUser::Uuid), Qt::DisplayRole, uuid, 1);
    if (list.count() != 1)
        return false;
    // act only on modified users
    if (d->m_Uuid_UserList.value(uuid)->isModified()) {
        Internal::UserData *user = d->m_Uuid_UserList.value(uuid);
        // check user write rights
        if ((user->isCurrent()) &&
            (!d->m_CurrentUserRights & Core::IUser::WriteOwn)) {
            toReturn = false;
        } else if ((!user->isCurrent()) &&
                   (!d->m_CurrentUserRights & Core::IUser::WriteAll)) {
            toReturn = false;
        } else if (!userBase()->saveUser(user)) {
            toReturn = false;
        }
    }
    Q_EMIT dataChanged(index(list.at(0).row(),0) , index(list.at(0).row(), Core::IUser::NumberOfColumns));
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
    Q_EMIT memoryUsageChanged();
}

/**
  \brief Define the filter of the model.
  \sa Utils::Database::getWhereClause()
*/
void UserModel::setFilter (const QHash<int,QString> &conditions)
{
    /** \todo filter by name AND Firstname at the same time */
    QString filter = "";
    const Internal::UserBase *b = userBase();
    foreach(const int r, conditions.keys()) {
        QString baseField = "";
        switch (r)
        {
        case Core::IUser::Name : baseField = b->fieldName(Table_USERS, USER_NAME); break;
        case Core::IUser::Firstname : baseField = b->fieldName(Table_USERS, USER_FIRSTNAME); break;
        default: break;
        }
        if (baseField.isEmpty()) continue;
        filter += QString("(`%1` %2) AND \n").arg(baseField, conditions.value(r));
    }
    filter.chop(5);
    QSqlTableModel::setFilter(filter);
//    qWarning() << filter;
}

/** Return the LinkId for the user with uuid \e uid */
int UserModel::practionnerLkId(const QString &uid)
{
    /** \todo manage user's groups */
    if (d->m_Uuid_UserList.keys().contains(uid)) {
        Internal::UserData *user = d->m_Uuid_UserList.value(uid);
//        qWarning() << "xxxxxxxxxxxxx memory" << uid << user->linkIds();
        return user->personalLinkId();
    }
    int lk_id = -1;
    if (uid.isEmpty())
        return lk_id;

    QHash<int, QString> where;
    where.clear();
    where.insert(Constants::LK_USER_UUID, QString("='%1'").arg(uid));
    QString req = userBase()->select(Constants::Table_USER_LK_ID, Constants::LK_LKID, where);
    QSqlQuery query(req, userBase()->database());
    if (query.isActive()) {
        if (query.next())
            return query.value(0).toInt();
    } else {
        Utils::Log::addQueryError("UserModel", query);
    }
//    qWarning() << "xxxxxxxxxxxxx database" << uid << lk_ids;
    return lk_id;
}

QList<int> UserModel::practionnerLkIds(const QString &uid)
{
//    qWarning() << "\n\n" << Q_FUNC_INFO << uid;
    /** \todo manage user's groups */
    if (d->m_Uuid_UserList.keys().contains(uid)) {
        Internal::UserData *user = d->m_Uuid_UserList.value(uid);
//        qWarning() << "xxxxxxxxxxxxx memory" << uid << user->linkIds();
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
        LOG_QUERY_ERROR(query);
    }
//    qWarning() << "xxxxxxxxxxxxx database" << uid << lk_ids << "\n\n";
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

/** Used by UserManagerPlugin to inform the currently connected user after Core is opened. */
void UserModel::emitUserConnected() const
{
    Q_EMIT userConnected(d->m_CurrentUserUuid);
}
