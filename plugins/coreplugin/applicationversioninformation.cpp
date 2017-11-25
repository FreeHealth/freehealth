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
 * \class Core::ApplicationVersionInformation
 * Owns all information about the application version update.
 */

#include "applicationversioninformation.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace Core;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Core {
namespace Internal {
class ApplicationVersionInformationPrivate
{
public:
    ApplicationVersionInformationPrivate(ApplicationVersionInformation *parent) :
        _versionUpdateDetected(false),
        _updatePrefences(false),
        _updateForms(false),
        _updateTokenedDocs(false),
        q(parent)
    {
    }
    
    ~ApplicationVersionInformationPrivate()
    {
    }
    
public:
    bool _versionUpdateDetected, _updatePrefences, _updateForms, _updateTokenedDocs;
    QString _previousVersion, _currentVersion;

private:
    ApplicationVersionInformation *q;
};
} // namespace Internal
} // end namespace Core

/*! Constructor of the Core::ApplicationVersionInformation class */
ApplicationVersionInformation::ApplicationVersionInformation() :
    d(new ApplicationVersionInformationPrivate(this))
{
    d->_currentVersion = PACKAGE_VERSION;
}

ApplicationVersionInformation::ApplicationVersionInformation(const ApplicationVersionInformation &copy) :
    d(new ApplicationVersionInformationPrivate(this))
{
    // TODO: Copy all data of the private part
}

/*! Destructor of the Core::ApplicationVersionInformation class */
ApplicationVersionInformation::~ApplicationVersionInformation()
{
    if (d)
        delete d;
    d = 0;
}

void ApplicationVersionInformation::setVersionUpdateDetected(bool updateDetected)
{
    d->_versionUpdateDetected = updateDetected;
}

bool ApplicationVersionInformation::isVersionUpdateDetected() const
{
    return d->_versionUpdateDetected;
}

void ApplicationVersionInformation::setPreviousVersion(const QString &version)
{
    d->_previousVersion = version;
}

void ApplicationVersionInformation::setCurrentVersion(const QString &version)
{
    d->_currentVersion = version;
}

const QString &ApplicationVersionInformation::previousVersion() const
{
    return d->_previousVersion;
}

const QString &ApplicationVersionInformation::currentVersion() const
{
    return d->_currentVersion;
}

void ApplicationVersionInformation::setUpdatePreferencesAccepted(bool updateAccepted)
{
    d->_updatePrefences = updateAccepted;
}


bool ApplicationVersionInformation::isUpdatePreferencesAccepted() const
{
    return d->_updatePrefences;
}

void ApplicationVersionInformation::setUpdateFormsAccepted(bool updateAccepted)
{
    d->_updateForms = updateAccepted;
}

bool ApplicationVersionInformation::isUpdateFormsAccepted() const
{
    return d->_updateForms;
}

void ApplicationVersionInformation::setUpdateTokenedDocumentsAccepted(bool updateAccepted)
{
    d->_updateTokenedDocs = updateAccepted;
}

bool ApplicationVersionInformation::isUpdateTokenedDocumentsAccepted() const
{
    return d->_updateTokenedDocs;
}

