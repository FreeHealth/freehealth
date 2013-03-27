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
 *   Main developers : Christian A Reiter                                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_URLPHOTOPROVIDER_H
#define PATIENTS_URLPHOTOPROVIDER_H

#include <coreplugin/iphotoprovider.h>
#include <QString>

/**
 * \file urlphotoprovider.h
 * \author Christian A Reiter
 * \version 0.8.0
 * \date 05 Dec 2012
*/

namespace Patients {
namespace Internal {
class UrlPhotoProvider : public Core::IPhotoProvider
{
    Q_OBJECT
public:
    explicit UrlPhotoProvider(QObject *parent = 0);
    ~UrlPhotoProvider();

    QString id() const;
    QString name() const;
    QString displayText() const;
    bool isActive() const;
    bool isEnabled() const;
    int priority() const;

public Q_SLOTS:
    void startReceivingPhoto();
};

} // namespace Internal
} // namespace Patients

#endif // PATIENTS_URLPHOTOPROVIDER_H
