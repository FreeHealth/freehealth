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
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACKPLUGIN_DATAPACKSERVERQUERY_H
#define DATAPACKPLUGIN_DATAPACKSERVERQUERY_H

#include <datapackplugin/datapack_exporter.h>
#include <datapackplugin/datapackquery.h>
#include <QList>

/**
 * \file datapackserverquery.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 20 Nov 2012
*/

namespace DataPackPlugin {

class DATAPACK_PLUGIN_EXPORT DataPackServerQuery
{
public:
    DataPackServerQuery();
    bool isValid() const;

    void setServerInternalUuid(const QString &uid) {_internalUid = uid;}
    const QString &serverInternalUuid() const {return _internalUid;}

    void setOriginalDescriptionFileAbsolutePath(const QString &absPath) {_descrAbsPath = absPath;}
    const QString &originalDescriptionFileAbsolutePath() const {return _descrAbsPath;}

    void setOutputServerAbsolutePath(const QString &absPath) {_absPathServer=absPath;}
    const QString &outputServerAbsolutePath() const {return _absPathServer;}

    void setAutoVersion(bool autoVersion) {_autoVersion = autoVersion;}
    bool autoVersion() const {return _autoVersion;}

private:
    bool createDirs() const;

private:
    QString _internalUid, _descrAbsPath, _absPathServer;
    bool _autoVersion;
};

} // namespace DataPackPlugin

#endif // DATAPACKPLUGIN_DATAPACKSERVERQUERY_H
