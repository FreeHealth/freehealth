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
#ifndef USERMANAGER_MAINWINDOW_H
#define USERMANAGER_MAINWINDOW_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <QMainWindow>
/**
 * \file usermanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 19 Sept 2009
*/

namespace UserPlugin {
namespace Internal {
class UserManagerPrivate;
}  // End Internal

class USER_EXPORT UserManager : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(UserManager)
public:
    explicit UserManager(QWidget *parent = 0); // work with tkUserModel
    ~UserManager();

    virtual bool initialize();

protected:
    void closeEvent(QCloseEvent *event);
    
private:
    Internal::UserManagerPrivate *d;
};

}  // End UserPlugin

#endif // USERMANAGER_MAINWINDOW_H
