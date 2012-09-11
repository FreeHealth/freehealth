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
@if "%Doxygen%" == "true"
@if "%Internal%" == "true"
/*!
 * \class %PluginNamespace:c%::Internal::%ClassName:c%
 * \brief short description of class
 *
 * Long description of class
 * \sa %PluginNamespace:c%::
 */
@else
/*!
 * \class %PluginNamespace:c%::%ClassName:c%
 * \brief short description of class
 *
 * Long description of class
 * \sa %PluginNamespace:c%::
 */
@endif
@endif


#include "%ClassName:l%.h"

#include <QDebug>

using namespace %PluginNamespace:c%;
using namespace Internal;

@if "%PIMPL%" == "true"
namespace %PluginNamespace:c% {
namespace Internal {
@if "%Doxygen%" == "true"
/*!
 * \class %PluginNamespace:c%::Internal::%ClassName:c%Private
 * \brief Private implementation of the %PluginNamespace:c%::%ClassName:c% class.
 *
 * documentation here
 */
@endif
class %ClassName:c%Private
{
public:
    %ClassName:c%Private(%ClassName:c% *parent) : q(parent) {}

private:
    %ClassName:c% *q;
};
}  // namespace Internal
} // end namespace %PluginNamespace:c%
@endif

@if "%Doxygen%" == "true"
/*! Constructor of the %ClassName:c% class */
@endif
%ClassName:c%::%ClassName:c%()
{
}

@if "%Doxygen%" == "true"
/*! Destructor of the %ClassName:c% class */
@endif
%ClassName:c%::~%ClassName:c%()
{
}

@if "%Doxygen%" == "true"
/*! initializes the %ClassName:c% instance with default values */
@endif
bool %ClassName:c%::initialize()
{
}
