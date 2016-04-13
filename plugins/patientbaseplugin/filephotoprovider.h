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
 *   Main developers : Christian A. Reiter <christian.a.reiter@gmail.com>
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_FILEPHOTOPROVIDER_H
#define PATIENTS_FILEPHOTOPROVIDER_H

#include <coreplugin/iphotoprovider.h>

/**
 * \file filephotoprovider.h
 * \author Christian A. Reiter <christian.a.reiter@gmail.com>
 * \version 0.8.0
 * \date 2012-09-29
*/

namespace Patients {

class FilePhotoProvider : public Core::IPhotoProvider
{
    Q_OBJECT
public:
    explicit FilePhotoProvider(QObject *parent);
    ~FilePhotoProvider();

    QString id() const;
    QString name() const;
    QString displayText() const;
    bool isEnabled() const;
    bool isActive() const;
    int priority() const;

public Q_SLOTS:
    void startReceivingPhoto();

};

} // namespace Patients

#endif  // PATIENTS_FILEPHOTOPROVIDER_H

