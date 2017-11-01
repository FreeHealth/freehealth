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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_HPRIMINTEGRATORDIALOG_H
#define TOOLS_INTERNAL_HPRIMINTEGRATORDIALOG_H

#include <QDialog>

/**
 * \file hprimintegratordialog.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 26 May 2013
*/

namespace Tools {
namespace Internal {
class HprimIntegratorDialogPrivate;

class HprimIntegratorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HprimIntegratorDialog(QWidget *parent = 0);
    ~HprimIntegratorDialog();
    bool initialize(const QString &hprimContent);
    
Q_SIGNALS:
    
public Q_SLOTS:
    void done(int r);

private Q_SLOTS:
    void onPatientSelected(const QString &fullName, const QString &uid);

private:
    HprimIntegratorDialogPrivate *d;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_INTERNAL_HPRIMINTEGRATORDIALOG_H

