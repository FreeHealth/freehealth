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
 *  This program is distributed in the hope that it will be useful, *
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
#ifndef EDRC_INTERNAL_SFMGABOUTDIALOG_H
#define EDRC_INTERNAL_SFMGABOUTDIALOG_H

#include <QDialog>

/**
 * \file sfmgaboutdialog.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 18 June 2013
*/

namespace eDRC {
namespace Internal {

namespace Ui {
class SfmgAboutDialog;
}

class SfmgAboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SfmgAboutDialog(QWidget *parent = 0);
    ~SfmgAboutDialog();
    
private:
    Ui::SfmgAboutDialog *ui;
};

} // namespace Internal
} // namespace eDRC

#endif // EDRC_INTERNAL_SFMGABOUTDIALOG_H
