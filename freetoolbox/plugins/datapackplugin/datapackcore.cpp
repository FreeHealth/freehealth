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
 * \class DataPackPlugin::DataPackCore
 * \brief short description of class
 *
 * Long description of class
 * \sa DataPackPlugin::
 */

#include "datapackcore.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

DataPackCore *DataPackCore::_instance = 0;

namespace DataPackPlugin {
namespace Internal {
class DataPackCorePrivate
{
public:
    DataPackCorePrivate(DataPackCore *parent) :
        q(parent)
    {
    }
    
    ~DataPackCorePrivate()
    {
    }
    
private:
    DataPackCore *q;
};
}  // namespace Internal
} // end namespace DataPackPlugin

/*! Constructor of the DataPackPlugin::DataPackCore class */
DataPackCore::DataPackCore(QObject *parent) :
    QObject(parent),
    d(new DataPackCorePrivate(this))
{
    _instance = this;
}

/*! Destructor of the DataPackPlugin::DataPackCore class */
DataPackCore::~DataPackCore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DataPackCore::initialize()
{
}

