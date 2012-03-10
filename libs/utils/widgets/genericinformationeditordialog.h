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
 *   Main Developers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef UTILS_GENERICINFORMATIONEDITORDIALOG_H
#define UTILS_GENERICINFORMATIONEDITORDIALOG_H

#include <utils/global_exporter.h>
#include <utils/genericdescription.h>

#include <QDialog>

/**
 * \file genericdescsriptioneditordialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 28 Nov 2011
*/

namespace Utils {

namespace Ui {
    class GenericInformationEditorDialog;
}

class UTILS_EXPORT GenericInformationEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenericInformationEditorDialog(QWidget *parent = 0);
    ~GenericInformationEditorDialog();

    void setDescription(const Utils::GenericDescription &desc);

private:
    Utils::GenericDescription submit();
    void done(int r);

private:
    Ui::GenericInformationEditorDialog *ui;
    Utils::GenericDescription m_desc;
};


} // namespace Utils


#endif // UTILS_GENERICINFORMATIONEDITORDIALOG_H
