/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 ***************************************************************************/
#ifndef LINEEDITECHOSWITCHER_H
#define LINEEDITECHOSWITCHER_H

#include <utils/global_exporter.h>
#include <utils/widgets/qbuttonlineedit.h>

/**
 * \file lineeditechoswitcher.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.4
 * \date 17 Feb 2012
*/

#include <QWidget>
#include <QLineEdit>
#include <QEvent>
#include <QIcon>

namespace Utils {
namespace Internal {
class LineEditEchoSwitcherPrivate;
}

class UTILS_EXPORT LineEditEchoSwitcher : public QButtonLineEdit
{
    Q_OBJECT
    Q_PROPERTY( QString text READ text WRITE setText USER true)

public:
    LineEditEchoSwitcher(QWidget *parent = 0);
    ~LineEditEchoSwitcher();

    void setIcon(const QIcon &icon);

public Q_SLOTS:
    void toogleEchoMode();

protected:
    void changeEvent(QEvent *e);

private:
    Internal::LineEditEchoSwitcherPrivate *d;
};

}  // End Utils

#endif // LINEEDITECHOSWITCHER_H
