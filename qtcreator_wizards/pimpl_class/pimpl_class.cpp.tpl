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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
@if "%Doxygen%" == "true"
/*!
@if "%Internal%" == "true"
 * \class %PluginNamespace:c%::Internal::%ClassName:c%
@else
 * \class %PluginNamespace:c%::%ClassName:c%
@endif
 * \brief short description of class
 *
 * Long description of class
 * \sa %PluginNamespace:c%::
 */
@endif

#include "%ClassName:l%.h"

@if "%Translations%" == "true"
#include <translationutils/constants.h>

@endif
#include <QDebug>

using namespace %PluginNamespace:c%;
@if "%Internal%" == "true"
using namespace Internal;
@else
@if "%PIMPL%" == "true"
using namespace Internal;
@endif
@endif
@if "%Translations%" == "true"
using namespace Trans::ConstantTranslations;
@endif

@if "%PIMPL%" == "true"
namespace %PluginNamespace:c% {
namespace Internal {
@if "%Doxygen%" == "true"
/*!
@if "%Internal%" == "true"
 * \class %PluginNamespace:c%::Internal::%ClassName:c%Private
@else
 * \class %PluginNamespace:c%::%ClassName:c%Private
@endif
 * \brief Private implementation of the %PluginNamespace:c%::%ClassName:c% class.
 *
 * documentation here
 */
@endif
class %ClassName:c%Private
{
public:
    %ClassName:c%Private(%ClassName:c% *parent) :
        _initialized(false),
        q(parent)
    {
    }

    ~%ClassName:c%Private()
    {
    }

public:
    bool _initialized;
    // Put your data here

private:
    %ClassName:c% *q;
};
} // namespace Internal
} // end namespace %PluginNamespace:c%
@endif

@if "%Singleton%" == "true"
// Singleton initialization
%ClassName:c% *%ClassName:c%::_instance = 0;

/**
 * Singleton access. This object creates its instance in the Ctor. So you should never
 * request the ctor more than once.
 */
@if "%Internal%" == "true"
%PluginNamespace:c%::Internal::%ClassName:c% &%PluginNamespace:c%::Internal::%ClassName:c%::instance() // static
@else
%PluginNamespace:c%::%ClassName:c% &%PluginNamespace:c%::%ClassName:c%::instance() // static
@endif
{
    Q_ASSERT(_instance);
    return *_instance;
}
@endif

@if "%Doxygen%" == "true"
@if "%Internal%" == "true"
/*! Constructor of the %PluginNamespace%::Internal::%ClassName:c% class */
@else
/*! Constructor of the %PluginNamespace%::%ClassName:c% class */
@endif
@endif
%ClassName:c%::%ClassName:c%(QObject *parent) :
@if "%PIMPL%" == "true"
    QObject(parent),
    d(new %ClassName:c%Private(this))
@else
    QObject(parent)
@endif
{
@if "%Singleton%" == "true"
    _instance = this;
@endif
}

@if "%Doxygen%" == "true"
@if "%Internal%" == "true"
/*! Destructor of the %PluginNamespace%::Internal::%ClassName:c% class */
@else
/*! Destructor of the %PluginNamespace%::%ClassName:c% class */
@endif
@endif
%ClassName:c%::~%ClassName:c%()
{
@if "%Singleton%" == "true"
    _instance = 0;
@endif
@if "%PIMPL%" == "true"
    if (d)
        delete d;
    d = 0;
@endif
}

@if "%Doxygen%" == "true"
/*! Initializes the object with the default values. Return true if initialization was completed. */
@endif
bool %ClassName:c%::initialize()
{
    if (d->_initialized)
        return true;
    return true;
}
