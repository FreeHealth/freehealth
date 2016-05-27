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
#ifndef PMHCREATORDIALOG_H
#define PMHCREATORDIALOG_H

#include <QDialog>
class QAbstractButton;

/**
 * \file pmhcreatordialog.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 25 July 2012
*/

namespace Category {
class CategoryItem;
}

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

    void setCategory(Category::CategoryItem *category);

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
