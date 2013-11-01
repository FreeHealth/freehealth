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
 * \class DDI::ServerUpdateManager
 */

#include "serverupdatemanager.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace DDI {
namespace Internal {
class ServerUpdateManagerPrivate
{
public:
    ServerUpdateManagerPrivate(ServerUpdateManager *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~ServerUpdateManagerPrivate()
    {
    }

    // Wiki tokens:
    // {{~ServerDate~}} -> current server date
    void createWikiContent()
    {}
    
public:
    // Put your data here
    
private:
    ServerUpdateManager *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::ServerUpdateManager class */
ServerUpdateManager::ServerUpdateManager(QObject *parent) :
    QObject(parent),
    d(new ServerUpdateManagerPrivate(this))
{
}

/*! Destructor of the DDI::ServerUpdateManager class */
ServerUpdateManager::~ServerUpdateManager()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ServerUpdateManager::initialize()
{
    return true;
}

