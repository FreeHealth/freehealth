/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef PMHCREATORDIALOG_H
#define PMHCREATORDIALOG_H

#include <QDialog>
class QAbstractButton;


namespace PMH {
namespace Ui {
    class PmhCreatorDialog;
}

class PmhCreatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PmhCreatorDialog(QWidget *parent = 0);
    ~PmhCreatorDialog();

private Q_SLOTS:
    void on_buttonBox_clicked(QAbstractButton *button);
    void helpRequested();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PmhCreatorDialog *ui;
};

}

#endif // PMHCREATORDIALOG_H
