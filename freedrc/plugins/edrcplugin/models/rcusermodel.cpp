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
 * \class eDRC::RcUserModel
 * Stores and access to any user recorded RC.
 */

#include "rcusermodel.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace eDRC {
namespace Internal {
class RcUserModelPrivate
{
public:
    RcUserModelPrivate(RcUserModel *parent) :
        q(parent)
    {
    }
    
    ~RcUserModelPrivate()
    {
    }
    
public:
    // Put your data here
    
private:
    RcUserModel *q;
};
} // namespace Internal
} // end namespace eDRC

/*! Constructor of the eDRC::RcUserModel class */
RcUserModel::RcUserModel(QObject *parent) :
    QObject(parent),
    d(new RcUserModelPrivate(this))
{
}

/*! Destructor of the eDRC::RcUserModel class */
RcUserModel::~RcUserModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool RcUserModel::initialize()
{
    return true;
}

