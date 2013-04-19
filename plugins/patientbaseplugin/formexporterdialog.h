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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_INTERNAL_FORMEXPORTERDIALOG_H
#define PATIENTS_INTERNAL_FORMEXPORTERDIALOG_H

#include <QDialog>

/**
 * \file formexporterdialog.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 19 Apr 2013
*/

namespace Patients {
namespace Internal {
class FormExporterDialogPrivate;

class FormExporterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FormExporterDialog(QWidget *parent = 0);
    ~FormExporterDialog();
    
    bool initialize();
    
Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    FormExporterDialogPrivate *d;
};

} // namespace Internal
} // namespace Patients

#endif // PATIENTS_INTERNAL_FORMEXPORTERDIALOG_H

