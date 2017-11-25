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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_APPLICATIONVERSIONINFORMATION_H
#define CORE_APPLICATIONVERSIONINFORMATION_H

#include <coreplugin/core_exporter.h>
#include <QObject>

/**
 * \file applicationversioninformation.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 11 May 2013
*/

namespace Core {
namespace Internal {
class ApplicationVersionInformationPrivate;
} // namespace Internal

class CORE_EXPORT ApplicationVersionInformation
{    
public:
//    enum License {
//        LGPLv2 = 0,
//        LGPLv2_1,
//        GPLv2,
//        GPLv3,
//        BSD,
//        BSD3Clause,
//        Unknown
//    };

    ApplicationVersionInformation();
    ApplicationVersionInformation(const ApplicationVersionInformation &copy);

    ~ApplicationVersionInformation();

    // Version update
    void setVersionUpdateDetected(bool updateDetected);
    bool isVersionUpdateDetected() const;

    void setPreviousVersion(const QString &version);
    void setCurrentVersion(const QString &version);
    const QString &previousVersion() const;
    const QString &currentVersion() const;

    void setUpdatePreferencesAccepted(bool updateAccepted);
    bool isUpdatePreferencesAccepted() const;

    void setUpdateFormsAccepted(bool updateAccepted);
    bool isUpdateFormsAccepted() const;

    void setUpdateTokenedDocumentsAccepted(bool updateAccepted);
    bool isUpdateTokenedDocumentsAccepted() const;

private:
    Internal::ApplicationVersionInformationPrivate *d;
};

} // namespace Core

#endif  // CORE_APPLICATIONVERSIONINFORMATION_H

