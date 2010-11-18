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
 ***************************************************************************/
#ifndef ICDCENTRALWIDGET_H
#define ICDCENTRALWIDGET_H

#include <QWidget>

namespace ICD {
namespace Internal {
class IcdCentralWidgetPrivate;
}  // End namespace Internal

namespace Ui {
    class IcdCentralWidget;
}

class IcdCentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IcdCentralWidget(QWidget *parent = 0);
    ~IcdCentralWidget();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void TEST_icdDialog(const QVariant &SID);

private:
    Ui::IcdCentralWidget *ui;
    Internal::IcdCentralWidgetPrivate *d;
};

}  // End namespace ICD

#endif // ICDCENTRALWIDGET_H
