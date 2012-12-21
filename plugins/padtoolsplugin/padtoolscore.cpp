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
 * \class PadTools::Internal::PadToolsCore
 * \brief short description of class
 *
 * Long description of class
 * \sa PadTools::
 */

#include "padtoolscore.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace PadTools {
namespace Internal {
/*!
 * \class PadTools::Internal::PadToolsCorePrivate
 * \brief Private implementation of the PadTools::PadToolsCore class.
 *
 * documentation here
 */
class PadToolsCorePrivate
{
public:
    PadToolsCorePrivate(PadToolsCore *parent) :
        q(parent)
    {
    }
    
    ~PadToolsCorePrivate()
    {
    }
    
public:
    // Put your data here
    
private:
    PadToolsCore *q;
};
} // namespace Internal
} // end namespace PadTools

// Singleton initialization
PadToolsCore *PadToolsCore::_instance = 0;

/**
 * Singleton access. This object creates its instance in the Ctor. So you should never
 * request the ctor more than once.
 */
PadTools::PadToolsCore &PadTools::PadToolsCore::instance() // static
{
    Q_ASSERT(_instance);
    return *_instance;
}

/*! Constructor of the PadTools::Internal::PadToolsCore class */
PadToolsCore::PadToolsCore(QObject *parent) :
    QObject(parent),
    d(new PadToolsCorePrivate(this))
{
    _instance = this;
}

/*! Destructor of the PadTools::Internal::PadToolsCore class */
PadToolsCore::~PadToolsCore()
{
    _instance = 0;
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PadToolsCore::initialize()
{
    return true;
}

