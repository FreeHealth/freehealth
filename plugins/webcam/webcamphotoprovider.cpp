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
#include <QtGui/QDialog>
#include <QtGui/QPushButton>

#include <coreplugin/itheme.h>
#include <coreplugin/icore.h>
#include <utils/global.h>

#include <webcamdialog.h>

using namespace cv;

namespace Webcam {
namespace Internal {

WebcamPhotoProvider::WebcamPhotoProvider()
{
}

/*!
  \brief returns Photo that is captured by the selected webcam.

  All of the code is in this function, there is no async calling, because
  the dialog should be modal and no other functions should be possible.
*/
QString WebcamPhotoProvider::recievePhotoFile()
{
    WebcamDialog dialog(0);

    // Escape button pressed, etc.
    if(dialog.exec() != QDialog::Accepted) {
        return QString();
    }
    // ok, here we go:
    return QString();
    // return &dialog.photo();
}


WebcamPhotoProvider::~WebcamPhotoProvider()
{
}

} // end Internal
} // end Webcam
