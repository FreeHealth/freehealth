/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Patients::FilePhotoProvider
 * \brief Provides a Filedialog for selecting a patient photo
 *
 * This class implements the Core::IPhotoProvider plugin extension.
 * \sa Core::IPhotoProvider
 */

#include "filephotoprovider.h"
#include "constants_settings.h"

#include <translationutils/constants.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

#include <QDebug>
#include <QFileDialog>
#include <QApplication>

using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


/*! Constructor of the Patients::FilePhotoProvider class */
FilePhotoProvider::FilePhotoProvider(QObject *parent) :
    IPhotoProvider(parent)
{
}

/*! Destructor of the Patients::FilePhotoProvider class */
FilePhotoProvider::~FilePhotoProvider()
{
}

QString FilePhotoProvider::id() const
{
    return "file";
}

QString FilePhotoProvider::name() const
{
    return "file";
}

QString FilePhotoProvider::displayText() const
{
    return tr("Choose from file...");
}

/*! Returns always true */
bool FilePhotoProvider::isEnabled() const
{
    return true;
}

/*! Returns always true */
bool FilePhotoProvider::isActive() const
{
    return true;
}

/*! Returns a low priority to be listed at last/fallback position. */
int FilePhotoProvider::priority() const
{
    return 100;
}

void FilePhotoProvider::startReceivingPhoto()
{
    QString fileName = QFileDialog::getOpenFileName(QApplication::activeWindow(), tr("Choose a photo"),
                                                    QDir::homePath(),
                                                    "Image (*.png *.jpg *.jpeg *.gif *.tiff)");
    if (!fileName.isEmpty()) {
        QPixmap photo;
        photo.load(fileName);
        Q_EMIT photoReady(photo);
    }
}
