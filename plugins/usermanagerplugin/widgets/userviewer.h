/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKUSERVIEWER_H
#define TKUSERVIEWER_H

// include tkUser headers
#include <usermanagerplugin/usermanager_exporter.h>

// include Qt headers
#include <QWidget>
#include <QObject>

/**
 * \file userviewer.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.4
 * \date 18 Sept 2009
*/

namespace UserPlugin {
namespace Internal {
class UserViewerPrivate;
}  // End Internal

class USER_EXPORT UserViewer : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(UserViewer);

public:
    explicit UserViewer(QWidget *parent = 0); // work with model
    void changeUserTo(const int modelRow);

private:
    Internal::UserViewerPrivate *d;
};

}  // End UserPlugin

#endif // TKUSERVIEWER_H
