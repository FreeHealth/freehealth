/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_SERVERENGINESTATUS_H
#define DATAPACK_SERVERENGINESTATUS_H

#include <datapackutils/datapack_exporter.h>

#include <QStringList>

namespace DataPack {

struct DATAPACK_EXPORT ServerEngineStatus {
    ServerEngineStatus() :
        hasError(false),
        isSuccessful(false),
        isLicenseAccepted(true),
        downloadCorrectlyFinished(false),
        proxyIdentificationError(false),
        serverIdentificationError(false)
    {}

    bool hasError, isSuccessful, isLicenseAccepted;
    bool downloadCorrectlyFinished;
    bool proxyIdentificationError, serverIdentificationError;
    QStringList errorMessages, engineMessages;
};

} // End namespace DataPack

#endif // DATAPACK_SERVERENGINESTATUS_H
