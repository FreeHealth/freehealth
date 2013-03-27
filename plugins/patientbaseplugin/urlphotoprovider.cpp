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
#include "urlphotoprovider.h"
#include "urlphotodialog.h"

namespace Patients {
namespace Internal {

UrlPhotoProvider::UrlPhotoProvider(QObject *parent) :
    Core::IPhotoProvider(parent)
{
}

/*! Deletes this Provider from the pool */
UrlPhotoProvider::~UrlPhotoProvider()
{
}

QString UrlPhotoProvider::id() const
{
    return "url";
}

/*! \brief Returns a translated name for this provider. */
QString UrlPhotoProvider::name() const
{
    return "Url";
}

/*! Return the translateable text of the object. */
QString UrlPhotoProvider::displayText() const
{
    return QString("Download from a URL...");
}

/*! \brief Returns the active state of the provider. */
bool UrlPhotoProvider::isActive() const
{
    //TODO: check if there is a network connection!
    return true;
}

/*! \brief Returns the enabled state of the provider. This can be changed in user settings. */
bool UrlPhotoProvider::isEnabled() const
{
    return isActive();
}

/*! \brief Returns a middle-low priority. */
int UrlPhotoProvider::priority() const
{
    return 30;
}

void UrlPhotoProvider::startReceivingPhoto()
{
    UrlPhotoDialog dialog;
    QPixmap photo;
    if (dialog.exec() == QDialog::Accepted) {
        photo = dialog.photo();
        Q_EMIT photoReady(photo);
    }
}

} // namespace Internal
} // namespace Patients
