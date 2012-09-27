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
#ifndef CORE_IPHOTOPROVIDER_H
#define CORE_IPHOTOPROVIDER_H

#include <coreplugin/core_exporter.h>
#include <QObject>

class QPixmap;

namespace Core {
/*!
 * \interface Patients::IPhotoProvider
 * \brief Provides a hook for plugins that return a photo path.
 *
 * Implement this interface if you want to provide another way of
 * fetching a foto for the user instead of the default (searching for it
 * in the files system).
 * Possibilities are webcams, scanners, other databases, LDAP, etc.
 *
 * The recievePhotoFile() method must return a QPixmap - if there was an error
 * it should return an empty QPixmap().
 */

class CORE_EXPORT IPhotoProvider : public QObject
{
    Q_OBJECT
public:
    explicit IPhotoProvider(QObject *parent = 0) : QObject(parent) {}
    virtual ~IPhotoProvider() {}

    /*! \brief Returns a translated name for this provider.
     *
     * This could be the vendor and model name of a webcam, or another identifier. */
    virtual QString name() const = 0;

    /*! Returns a photo as a Pixmap. In case or errors return a QPixmap() */
    virtual QPixmap recievePhoto() = 0;

    /*! \brief Returns the active state of the provider.
     *
     * \e true means that there is "physically" an e.g. connected webcam. */
    virtual bool isActive() const = 0;

    /*! \brief Returns the enabled state of the provider. This can be changed in e.g. user settings. */
    virtual bool isEnabled() const = 0;

    /*! \brief Returns the priority which is used to order a provider list.
     *
     * 0 means no priority, 100 means absolute priority. Feel free to choose any value in between. */
    virtual uint priority() const = 0;

    /*! \brief This function is implemented because of sorting capabilities of the IPhotoProvider. It cannot be overloaded. */
    bool operator<(const IPhotoProvider *other) { return this->priority() < other->priority(); }
};

} // end namespace Core
#endif // CORE_IPHOTOPROVIDER_H
