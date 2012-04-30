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
 *   Main Developers:                                                      *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "segmentedbutton.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>

using namespace Utils;

namespace {

const char *const FIRST_BUTTON =
        "border-top-left-radius: 8px;"
        "border-bottom-left-radius: 8px;"
        ;

const char *const MIDDLE_BUTTON = ""
//        "border-left: 1px;"
        ;

const char *const LAST_BUTTON =
        "border-top-right-radius: 8px;"
        "border-bottom-right-radius: 8px;"
        ;

const char *const BUTTON_CSS =
        "QPushButton {"
        "border: 1px outset #777;"
        "background: qradialgradient(cx: 0.3, cy: -0.4,"
        "fx: 0.3, fy: -0.4,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #ccc);"
        "color: #333;"
        "%1"
        "padding: 5px;"
        "min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "background: qradialgradient(cx: 0.4, cy: -0.1,"
        "fx: 0.4, fy: -0.1,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #ddd);"
        "}"
        "QPushButton:pressed {"
        "border: 1px inset #666;"
        "background: qradialgradient(cx: 0.3, cy: -0.4,"
        "fx: 0.3, fy: -0.4,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #888);"
        "}"
        ;

}

SegmentedButton::SegmentedButton(QWidget *parent) :
    QWidget(parent),
    _first(0),
    _last(0)
{
    QHBoxLayout *lay = _buttonLayout = new QHBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);
    setLayout(lay);
}

void SegmentedButton::setFirstButton(QPushButton *but)
{
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::FIRST_BUTTON));
    _buttonLayout->addWidget(but);
    _first = but;
}
void SegmentedButton::addMiddleButton(QPushButton *but)
{
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::MIDDLE_BUTTON));
    _buttonLayout->addWidget(but);
    _buttons << but;
}
void SegmentedButton::setLastButton(QPushButton *but)
{
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::LAST_BUTTON));
    _buttonLayout->addWidget(but);
    _last = but;
}
