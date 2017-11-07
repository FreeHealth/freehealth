/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERPLUGIN_INTERNAL_USERBASE_H
#define USERPLUGIN_INTERNAL_USERBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <usermanagerplugin/usermanager_exporter.h>

#include <QObject>

/**
 * \file userbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.8
 * \date 17 Sept 2012
*/

namespace UserPlugin {
class UserCore;
class UserModel;
class UserCreationPage;

namespace Internal {
class UserData;

class USER_EXPORT UserBase :  public QObject, public Utils::Database
{
    Q_OBJECT
    friend class UserPlugin::UserModel;
    friend class UserPlugin::UserCreationPage;
    friend class UserPlugin::UserCore;

protected:
    UserBase(QObject *parent = 0);
    bool initialize();
    void onCoreDatabaseServerChanged();

protected Q_SLOTS:
    void onCoreFirstRunCreationRequested();

public:
    virtual ~UserBase() {}

    bool isInitialized() const;
//    bool checkDatabaseVersion();
    bool updateDatabase() const;
    bool isNewlyCreated() const;

    bool createDefaultUser();
    bool createVirtualUser(const QString &uid, const QString &name, const QString &firstName, int title, int gender,
                           const QStringList &specialties, const QStringList &qualifications,
                           int medicalRights = 0, int secretaryRights = 0, int userRights = 0, int agendaRights = 0, int paramedicRights = 0,
                           QLocale::Language lang = QLocale().language());

    // retrievers
    UserData *getUserById(const QVariant &_id) const;  // not used
    UserData *getUserByLoginPassword(const QVariant &login, const QVariant &cryptedPassword) const;
    UserData *getUserByUuid(const QString &uuid) const;

    // data retrievers
    QString getUuid(const QString &log64, const QString &cryptpass64);
    QString createNewUuid();
    QString getLogin64(const QString &uuid);
    QString getCryptedPassword(const QString &clearLogin);
    QString getUserDynamicData(const QString &userUid, const QString &dynDataUuid);

    // savers
    bool createUser(UserData *user);
    bool saveUser(UserData *user);
    bool purgeUser(const QString &uuid);
    bool saveUserPreferences(const QString &uid, const QString &content);
    bool savePapers(UserData *user);
    bool changeUserPassword(UserData *user, const QString &clearPassword);
    bool saveUserDynamicData(const QString &userUid, const QString &dynDataUuid, const QVariant &value);

    // data checkers
    bool checkLogin(const QString &clearLogin, const QString &clearPassword);
    bool isLoginAlreadyExists(const QString &clearLogin) const;
    bool userExists(const QString &clearLogin) const;
    QDateTime recordLastLoggedIn(const QString &log, const QString &pass);

    // Linkers
    int getMaxLinkId();
    bool updateMaxLinkId(const int max);

    void toTreeWidget(QTreeWidget *tree) const;

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                        const QString &pathOrHostName,
                        TypeOfAccess access, AvailableDrivers driver,
                        const QString &login, const QString &pass,
                        const int port,
                        CreationOption createOption
                       );
    bool updateLastloginTypeToTimeStamp();
    // privates retrievers
    UserData *getUser(const QHash<int, QString> &conditions) const;
    QString databaseAndQtVersion() const;
//    QString getDatabaseQtVersion() const;
//    QString getDatabaseFmfVersion() const;

private:
    bool m_initialized;
    mutable QString  m_LastUuid, m_LastLogin, m_LastPass;
    bool m_IsNewlyCreated;
};

}  // namespace Internal
}  // namespace UserPlugin

#endif // USERPLUGIN_INTERNAL_USERBASE_H
