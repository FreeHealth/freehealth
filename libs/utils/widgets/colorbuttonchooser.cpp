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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \class Utils::ColorButtonChooser
  Simple button for color selection.
*/

#include "colorbuttonchooser.h"

#include <QColorDialog>

using namespace Utils;

ColorButtonChooser::ColorButtonChooser(QWidget *parent)
        : QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
    setMinimumSize(22,22);
    setMaximumSize(22,22);
    setColor(QColor(Qt::black));
}

ColorButtonChooser::ColorButtonChooser(const QColor& color, QWidget* parent)
        : QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
    setColor(color);
}

ColorButtonChooser::~ColorButtonChooser()
{
}


const QColor &ColorButtonChooser::color() const
{
    return m_Color;
}

void ColorButtonChooser::setColor(const QColor &color)
{
    m_Color = color;
    QPixmap pixmap(iconSize());
    pixmap.fill(m_Color);
    setIcon(QIcon(pixmap));
}

void ColorButtonChooser::onClicked()
{
    bool ok;
    const QRgb rgb = QColorDialog::getRgba(m_Color.rgba(), &ok, window());
    if (ok) {
        QColor color = QColor::fromRgba(rgb);
        setColor(color);
    }
}
