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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef WEBCAMPHOTOPROVIDER_H
#define WEBCAMPHOTOPROVIDER_H

class WebcamDevice;

#include <QObject>
#include <QPixmap>

#include <coreplugin/iphotoprovider.h>

namespace Webcam {
/*!
 * \brief The WebcamPhotoProvider class
 *
 * It implements the Core::IPhotoProvider interface. For every Webcam that is detected there should be one WebcamPhotoProvider.
 */
class WebcamPhotoProvider : public Core::IPhotoProvider
{
    Q_OBJECT
public:
    explicit WebcamPhotoProvider(int device, QObject *parent = 0);
    ~WebcamPhotoProvider();

    QString id() const;
    int device() const;
    QString name() const;
    QString displayText() const;
    bool isEnabled() const;
    bool isActive() const;
    int priority() const;

    static QList<WebcamPhotoProvider *> getProviders();

public Q_SLOTS:
    void startReceivingPhoto();

private:
    int m_deviceId;
    static QMap<int, WebcamPhotoProvider*> m_webcamsPool;
};

} // end Webcam

#endif // WEBCAMPHOTOPROVIDER_H
