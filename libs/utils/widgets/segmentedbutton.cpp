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
 *  Main Developers:                                                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Utils::SegmentedButton
  The segmented button is a multi-button control shown has a unique button (in Mas style).
  You firstly need to add your buttons using the setFirstButton(), addMiddleButton(), setLastButton().\n
  Then you need to prepare the buttons with computeSizes(). Each times the content of the QPushButton changes (text, tooltip)
  you have to recall computeSizes().\n
  You can set the button autoexclusives (after adding all your buttons) with the setAutoExclusive().
*/

#include "segmentedbutton.h"

#include <utils/global.h>

#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>

#include <QDebug>

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
        "radius: 1.35, stop: 0 #fff, stop: 1 #eee);"
        "color: #333;"
        "%1"
        "padding: 3px;"
        "}"

        "QPushButton:hover {"
        "background: qradialgradient(cx: 0.4, cy: -0.1,"
        "fx: 0.4, fy: -0.1,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #ededed);"
        "}"

        "QPushButton:pressed {"
        "border: 1px inset #666;"
        "background: qradialgradient(cx: 0.3, cy: -0.4,"
        "fx: 0.3, fy: -0.4,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #aaa);"
        "}"

        "QPushButton:checked {"
        "border: 1px inset #666;"
        "background: qradialgradient(cx: 0.3, cy: -0.4,"
        "fx: 0.3, fy: -0.4,"
        "radius: 1.35, stop: 0 #fff, stop: 1 #bbb);"
        "color: darkBlue;"
        "}"

        ;

}

/** Create an empty segmented button */
SegmentedButton::SegmentedButton(QWidget *parent) :
    QWidget(parent),
    _first(0),
    _last(0)
{
    QHBoxLayout *lay = _buttonLayout = new QHBoxLayout(this);
    lay->setMargin(0);
    if (Utils::isRunningOnMac())
        lay->setSpacing(11);
    else if (Utils::isRunningOnLinux() || Utils::isRunningOnFreebsd())
        lay->setSpacing(0);
    else
        lay->setSpacing(0);
    setLayout(lay);
}

/** Define the first button of the segmented button. */
void SegmentedButton::setFirstButton(QPushButton *but)
{
    but->setFocusPolicy(Qt::NoFocus);
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::FIRST_BUTTON));
    _buttonLayout->addWidget(but);
    _first = but;
}
/** Add middle button in the segmented button. */
void SegmentedButton::addMiddleButton(QPushButton *but)
{
    but->setFocusPolicy(Qt::NoFocus);
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::MIDDLE_BUTTON));
    _buttonLayout->addWidget(but);
    _buttons << but;
}
/** Define the last button of the segmented button. */
void SegmentedButton::setLastButton(QPushButton *but)
{
    but->setFocusPolicy(Qt::NoFocus);
    but->setStyleSheet(QString(::BUTTON_CSS).arg(::LAST_BUTTON));
    _buttonLayout->addWidget(but);
    _last = but;
}

/** Set the buttons autoExclusive according to the \e state. Call this function after all your buttons are registered. */
void SegmentedButton::setAutoExclusive(bool state)
{
    if (_first)
        _first->setAutoExclusive(state);
    if (_last)
        _last->setAutoExclusive(state);
    for(int i=0; i < _buttons.count(); ++i)
        _buttons.at(i)->setAutoExclusive(state);
}

/** Recomputes the sizes and the presentation of each registered buttons. Call this function each time the content of the button changes (text or tooltip). */
void SegmentedButton::computeSizes()
{
    // get max width
    int width = 0;
    if (_first)
        width = _first->width();
    if (_last)
        width = qMax(width, _last->width());
    for(int i=0; i < _buttons.count(); ++i) {
        width = qMax(width, _buttons.at(i)->width());
    }

    // apply to all buttons
    if (_first)
        _first->setMinimumWidth(width);
    if (_last)
        _last->setMinimumWidth(width);
    for(int i=0; i < _buttons.count(); ++i)
        _buttons.at(i)->setMinimumWidth(width);
}
