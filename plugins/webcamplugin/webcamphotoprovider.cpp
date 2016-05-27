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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "webcamphotoprovider.h"
//#include "webcamdialog.h"
#include "webcamdialog.h"
#include "webcamconstants.h"

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

#include <extensionsystem/pluginmanager.h>

#include <QDebug>

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

using namespace Webcam;

/*! Initializes the Provider with the device number and adds it to the pool */
WebcamPhotoProvider::WebcamPhotoProvider(int device, QObject *parent) :
    IPhotoProvider(parent)
{
    m_deviceId = device;
}

/*! Deletes this Provider from the pool */
WebcamPhotoProvider::~WebcamPhotoProvider()
{
}

/*!
 * Returns an machine readable uid name for the webcam device
 * \sa name(), device()
 */
QString WebcamPhotoProvider::id() const
{
    return QString("webcam%1").arg(m_deviceId);
}

/*!
 * Returns the OpenCV device number for the webcam device
 * \sa id(), name()
 */
int WebcamPhotoProvider::deviceId() const
{
    return m_deviceId;
}

/*!
 * Returns a human readable name for the webcam device
 * \sa id(), device()
 */
QString WebcamPhotoProvider::name() const
{
    //TODO: return webcam vendor/model name
    return tr("Webcam device %1").arg(m_deviceId);
}

/*!
 * Return the translatable text of the object
 */
QString WebcamPhotoProvider::displayText() const
{
    return tr("Take photo with %1...").arg(name());
}

/*!
 * \brief Opens the webcam dialog and emits photoReady() when finished.
 *
 * When no photo was selected, then emitted pixmap is an empty QPixmap().
 *\sa photoReady()
 */
void  WebcamPhotoProvider::startReceivingPhoto()
{
    WebcamDialog dialog;
    QPixmap photo;
    if(dialog.exec() == QDialog::Accepted) {
        photo = dialog.photo();
        Q_EMIT photoReady(photo);
    }
}

bool WebcamPhotoProvider::isEnabled() const
{
    return isActive() ;//&& settings()->value(Constants::S_WEBCAM_ENABLED).toBool();
}

bool WebcamPhotoProvider::isActive() const
{
    return false;
}

/*! Returns a high priority, webcams should be listed first. */
int WebcamPhotoProvider::priority() const
{
    return 10;
}

/*!
 * Returns the list of the currently existing WebcamPhotoProviders.
 * This member just asks the plugin manager object pool.
 * \sa PluginManager::addObject(), PluginManager::removeObject(), PluginManager::getObjects(),
 */
QList<WebcamPhotoProvider *> WebcamPhotoProvider::getProviders() //static
{
    QList<WebcamPhotoProvider*> objects = pluginManager()->getObjects<WebcamPhotoProvider>();
    return objects;
}


