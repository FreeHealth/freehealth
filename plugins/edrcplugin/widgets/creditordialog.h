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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef EDRC_INTERNAL_CREDITORDIALOG_H
#define EDRC_INTERNAL_CREDITORDIALOG_H

#include <QDialog>

/**
 * \file creditordialog.h
 * \author Eric Maeker
 * \version 0.9.2
 * \date 26 Sept 2013
*/

namespace eDRC {
namespace Internal {
class ConsultResult;
class CrEditorDialogPrivate;

class CrEditorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CrEditorDialog(QWidget *parent = 0);
    ~CrEditorDialog(); 
    bool initialize(const ConsultResult &cr);
    
    ConsultResult submit() const;
    
private Q_SLOTS:
    void onSmfgAboutClicked();
    void onArgumentsClicked();

private:
    CrEditorDialogPrivate *d;
};

} // namespace Internal
} // namespace EDRC

#endif // EDRC_INTERNAL_CREDITORDIALOG_H

