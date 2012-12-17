/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class UserPlugin::Internal::CoreUserModelWrapper
 * Represents the Core::IUser model. The unique instance is created and
 * managed by the UserPlugin::UserCore.
 * \sa Core::IUser, UserPlugin::UserModel
 */

#include "coreusermodelwrapper.h"
#include "usermodel.h"

#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDebug>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace UserPlugin {
namespace Internal {
class CoreUserModelWrapperPrivate
{
public:
    CoreUserModelWrapperPrivate(CoreUserModelWrapper *parent) :
        _userModel(0),
        q(parent)
    {
    }
    
    ~CoreUserModelWrapperPrivate()
    {
    }
    
public:
    UserModel *_userModel;

private:
    CoreUserModelWrapper *q;
};
} // namespace Internal
} // end namespace UserPlugin

/*! Constructor of the UserPlugin::Internal::CoreUserModelWrapper class */
CoreUserModelWrapper::CoreUserModelWrapper(QObject *parent) :
    Core::IUser(parent),
    d(new CoreUserModelWrapperPrivate(this))
{
}

/*! Destructor of the UserPlugin::Internal::CoreUserModelWrapper class */
CoreUserModelWrapper::~CoreUserModelWrapper()
{
    // FIXME: remove this wrapper from the core implementation?
//    Core::ICore::instance()->setUser(0);
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool CoreUserModelWrapper::initialize(UserModel *model)
{
    d->_userModel = model;
    connect(d->_userModel, SIGNAL(userConnected(QString)), this, SLOT(newUserConnected(QString)));
    return true;
}

bool CoreUserModelWrapper::has(const int ref) const
{
    return IN_RANGE_STRICT_MAX(ref, 0, Core::IUser::NumberOfColumns);
}

bool CoreUserModelWrapper::hasCurrentUser() const
{
    return d->_userModel->hasCurrentUser();
}

QVariant CoreUserModelWrapper::value(const int ref) const
{
    return d->_userModel->currentUserData(ref);
}

bool CoreUserModelWrapper::setValue(const int ref, const QVariant &value)
{
    if (d->_userModel->setData(d->_userModel->index(d->_userModel->currentUserIndex().row(), ref), value)) {
        Q_EMIT this->userDataChanged(ref);
        return true;
    }
    return false;
}

bool CoreUserModelWrapper::saveChanges()
{
    if (d->_userModel) {
        return d->_userModel->submitUser(uuid());
    }
    return false;
}

QString CoreUserModelWrapper::fullNameOfUser(const QVariant &uid)
{
    if (d->_userModel) {
        QHash<QString, QString> s = d->_userModel->getUserNames(QStringList() << uid.toString());
        return s.value(uid.toString());
    }
    return QString();
}

void CoreUserModelWrapper::newUserConnected(const QString &uid)
{
    Q_UNUSED(uid);
    Q_EMIT userChanged();
}
