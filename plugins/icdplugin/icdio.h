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
 ***************************************************************************/
#ifndef ICDIO_H
#define ICDIO_H

#include <icdplugin/icd_exporter.h>
#include <QtGlobal>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

/**
 * \file icdio.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 20 Nov 2010
*/

namespace ICD {
class IcdCollectionModel;
namespace Internal {
class IcdIOPrivate;
}  // End namespace Internal


class ICD_EXPORT IcdIO
{
public:
    enum ModelManagement {
        ReplaceModelContent = 0,
        AddToModel
    };

    IcdIO();
    ~IcdIO();

    static bool isDatabaseInitialized();

    QString icdCollectionToXml(const IcdCollectionModel *model);
    bool icdCollectionFromXml(IcdCollectionModel *model, const QString &xml, const ModelManagement management = ReplaceModelContent);

    QString icdCollectionToHtml(const IcdCollectionModel *model);

private:
    Internal::IcdIOPrivate *d;
};

}  // End namespace ICD

#endif // ICDIO_H
