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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DDI::Internal::ServerUpdateModel
 * \brief short description of class
 *
 * Long description of class
 * \sa DDI::
 */

#include "serverupdatemodel.h"
#include "serverupdate.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace DDI {
namespace Internal {
/*!
 * \class DDI::Internal::ServerUpdateModelPrivate
 * \brief Private implementation of the DDI::ServerUpdateModel class.
 *
 * documentation here
 */
class ServerUpdateModelPrivate
{
public:
    ServerUpdateModelPrivate(ServerUpdateModel *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~ServerUpdateModelPrivate()
    {
    }
    
public:
    // Put your data here
    
private:
    ServerUpdateModel *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::Internal::ServerUpdateModel class */
ServerUpdateModel::ServerUpdateModel(QObject *parent) :
    QObject(parent),
    d(new ServerUpdateModelPrivate(this))
{
}

/*! Destructor of the DDI::Internal::ServerUpdateModel class */
ServerUpdateModel::~ServerUpdateModel()
{
    if (d)
        delete d;
    d = 0;
}

/*!
 * Initializes the object with the server values.
 * Return true if initialization was completed.
 */
bool ServerUpdateModel::initialize(const QList<ServerUpdate> &updates)
{
    return true;
}

