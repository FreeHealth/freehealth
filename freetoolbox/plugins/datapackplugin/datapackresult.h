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
#ifndef DATAPACKPLUGIN_DATAPACKRESULT_H
#define DATAPACKPLUGIN_DATAPACKRESULT_H

#include <datapackplugin/datapack_exporter.h>
#include <QString>

/**
 * \file datapackresult.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 10 Nov 2012
*/

namespace DataPackPlugin {

class DATAPACK_PLUGIN_EXPORT DataPackResult
{
public:
    DataPackResult();

    void setPackDescriptionXmlContent(const QString &content) {_xmlContent = content;}
    const QString &packDescriptionXmlContent() const {return _xmlContent;}

    void setContentFileServerPath(const QString &relPath) {_contentServerPath = relPath;}
    const QString &contentFileServerPath() const {return _contentServerPath;}

private:
    QString _xmlContent, _contentServerPath;
};

} // namespace DataPackPlugin

#endif // DATAPACKPLUGIN_DATAPACKRESULT_H
