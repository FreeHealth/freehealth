/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef PACKDESCRIPTION_H
#define PACKDESCRIPTION_H

#include <datapackutils/datapack_exporter.h>
#include <utils/genericupdateinformation.h>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file packdescription.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 25 Nov 2011
*/

namespace DataPack {
namespace Internal {
}

class DATAPACK_EXPORT PackDescription
{
public:
    enum DataRepresentation {
        Uuid= 0,
        URL,
        Vendor,
        Validity,
        Category,
        ServerVersion,
        FreeMedFormsCompatVersion,
        FreeDiamsCompatVersion,
        FreeAccountCompatVersion,
        CreationDate,
        LastModificationDate,
        ShortDescription,
        HtmlDescription,
        HtmlSynthesis,
        GlobalLicense,
        GeneralIcon,
        MaxParam
    };

    PackDescription();
//    virtual ~PackDescription();

//    QVariant data(const int ref, const QString &lang = QString::null) const;
//    bool setData(const int ref, const QVariant &value, const QString &lang = QString::null);

//    void toTreeWidget(QTreeWidget *tree) const;

private:
//    Internal::ServerDescriptionPrivate *d;
    QList<Utils::GenericUpdateInformation> m_UpdateInfos;
};




}  // End namespace DataPack

#endif // PACKDESCRIPTION_H
