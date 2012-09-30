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
#include "webcamphotoprovider.h"
//#include "webcamdialog.h"
#include "webcamdialog.h"
#include "webcamconstants.h"


#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

#include <QDebug>

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

using namespace Webcam;

QMap<int, WebcamPhotoProvider*> WebcamPhotoProvider::m_webcamsPool = QMap<int, WebcamPhotoProvider*>();

/*! Initializes the Provider with the device number and adds it to the pool */
WebcamPhotoProvider::WebcamPhotoProvider(int device, QObject *parent) :
    IPhotoProvider(parent)
{
    m_device = device;
    m_webcamsPool[device] = this;
}

/*! Deletes this Provider from the pool */
WebcamPhotoProvider::~WebcamPhotoProvider()
{
    if (WebcamPhotoProvider::getProviders().remove(this->device()) == 0)
        qWarning() << QString("WebcamPhotoProvider: error removing device %1").arg(device());
}

/*!
 * Returns an machine readable uid name for the webcam device
 * \sa name(), device()
 */
QString WebcamPhotoProvider::id() const
{
    return QString("webcam%1").arg(m_device);
}

/*!
 * Returns the OpenCV device number for the webcam device
 * \sa id(), name()
 */
int WebcamPhotoProvider::device() const
{
    return m_device;
}

/*!
 * Returns a human readable name for the webcam device
 * \sa id(), device()
 */
QString WebcamPhotoProvider::name() const
{
    //TODO: return webcam vendor/model name
    return tr("Webcam device %1").arg(m_device);
}

QString WebcamPhotoProvider::displayText() const
{
    return QString("Take photo with %1...").arg(name());
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
    if(dialog.exec() != QDialog::Accepted) {
        photo = dialog.photo();
    }
    Q_EMIT photoReady(photo);
}

bool WebcamPhotoProvider::isEnabled() const
{
    return isActive() ;//&& settings()->value(Constants::S_WEBCAM_ENABLED).toBool();
}

bool WebcamPhotoProvider::isActive() const
{
    return false;
}

int WebcamPhotoProvider::priority() const
{
    return 70;
}

/*!
 * \brief Returns the list of WebcamPhotoProviders that are created
 *
 * The \e key of the QMap (\e int) is the OpenCV device number, and the \value is a
 * pointer to a WebcamPhotoProvider.
 */
QMap<int, WebcamPhotoProvider *> WebcamPhotoProvider::getProviders()
{
    return m_webcamsPool;
}


