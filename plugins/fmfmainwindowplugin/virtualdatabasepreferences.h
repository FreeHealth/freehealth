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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef VIRTUALDATABASEPREFERENCES_H
#define VIRTUALDATABASEPREFERENCES_H

#include <QWidget>

#include "ui_virtualbasepage.h"

/**
 * \file virtualdatabasepreferences.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 10 June 2010
*/

namespace Core {
class ISettings;
}

namespace MainWin {
namespace Internal {

class VirtualDatabasePreferences : public QWidget, private Ui::VirtualDatabasePreferences
{
    Q_OBJECT
    Q_DISABLE_COPY(VirtualDatabasePreferences)

public:
    VirtualDatabasePreferences(QWidget *parent);

    static void writeDefaultSettings(Core::ISettings *s = 0);

public Q_SLOTS:
    void saveToSettings(Core::ISettings *) {}
    void on_populateDb_clicked();
    void on_populateEpisodes_clicked();
    void on_populateUsers_clicked();

protected:
    virtual void changeEvent(QEvent *e);
};

}  // End namespace MainWin
}  // End namespace Internal

#endif // VIRTUALDATABASEPREFERENCES_H
