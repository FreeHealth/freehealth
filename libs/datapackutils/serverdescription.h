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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_SERVERDESCRIPTION_H
#define DATAPACK_SERVERDESCRIPTION_H

#include <datapackutils/datapack_exporter.h>
#include <utils/genericdescription.h>

#include <QVariant>
#include <QList>
QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file serverdescsription.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.2
 * \date 27 Apr 2012
*/

namespace DataPack {
namespace Internal {
}

class DATAPACK_EXPORT ServerDescription : public Utils::GenericDescription
{
public:
    enum ExtraNonTranslatedData {
        RecommendedUpdateFrequency = Utils::GenericDescription::NonTranslatableExtraData + 1,
        RequiereAuthentification
    };

    ServerDescription();
    ~ServerDescription();

    QString toXml() const;

private:
};

}  // End namespace DataPack


#endif // DATAPACK_SERVERDESCRIPTION_H
