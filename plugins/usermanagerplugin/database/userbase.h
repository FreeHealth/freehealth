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
#ifndef USERBASE_H
#define USERBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <usermanagerplugin/usermanager_exporter.h>
#include <usermanagerplugin/constants.h>
#include <usermanagerplugin/global.h>

#include <QObject>

/**
 * \file userbase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.10
 * \date 15 Nov 2009
*/

namespace UserPlugin {
class UserModel;

namespace Internal {
class UserData;

class USER_EXPORT UserBase : public Utils::Database
{
    Q_OBJECT
    friend void initLib();
//    friend void tkUser::warn() const;
    friend class UserPlugin::UserModel;
    friend class UserModelPrivate;
    friend class UserData;
    friend class UserIdentifier;

protected:
    UserBase(QObject *parent = 0);

    // Constructor
    static UserBase *instance();
    virtual ~UserBase() {}

    // initialize
    bool initialize(Core::ISettings *settings);
    bool isNewlyCreated() const;

    // retreivers
    UserData *getUserById(const QVariant &_id) const;  // not used
    UserData *getUserByLoginPassword(const QVariant &login, const QVariant &cryptedPassword) const;
    UserData *getUserByUuid(const QString &uuid) const;

    // datas retrievers
    QString getUuid(const QString &log64, const QString &cryptpass64);
    QString createNewUuid();

    // savers
    bool     saveUser(UserData *user);
    bool     deleteUser(const QString &uuid);

    // datas checkers
    bool      checkLogin(const QString &login, const QString &cryptedPassword) const;
    QDateTime recordLastLogin(const QString &log, const QString &pass);


private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          CreationOption createOption
                         );
//    void setConnectionName(const QString &c);

    // privates retreivers
    UserData *getUser(const QHash<int, QString> &conditions) const;

private:
    static bool m_initialized;
    static QString  m_LastUuid, m_LastLogin, m_LastPass;
    static UserBase * m_Instance;
    bool m_IsNewlyCreated;
};

}  // End namespace Internal
}  // End namespace UserPlugin

#endif // USERBASE_H
