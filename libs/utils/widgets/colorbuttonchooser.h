/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Code inspired of fresh/pColorButton : MonkeyStudio                    *
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef COLORBUTTONCHOOSER_H
#define COLORBUTTONCHOOSER_H

#include <utils/global_exporter.h>

#include <QPushButton>
#include <QColor>

/**
 * \file colorbuttonchooser.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.2
 * \date 24 Oct 2009
*/

namespace Utils {

class UTILS_EXPORT ColorButtonChooser : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    ColorButtonChooser(QWidget *parent = 0);
    ColorButtonChooser(const QColor &color, QWidget *parent = 0);
    virtual ~ColorButtonChooser();

    const QColor &color() const;

public Q_SLOTS:
    void setColor(const QColor &color);

protected Q_SLOTS:
    void onClicked();

private:
    QColor m_Color;
};

}  // End namespace Utils

#endif // COLORBUTTONCHOOSER_H
