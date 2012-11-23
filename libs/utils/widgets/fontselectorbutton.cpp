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
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "fontselectorbutton.h"

#include <QApplication>
#include <QFontDialog>
#include <QFontMetrics>
#include <QAction>
#include <QEvent>
#include <QHelpEvent>
#include <QTextDocument>
#include <QCursor>

#include <QDebug>

using namespace Utils;

FontSelectorButton::FontSelectorButton(QWidget *parent) :
    QToolButton(parent),
    _fontChanged(false),
    _currentDefined(false)
{
    aEditFont = new QAction(this);
    aResetToDefault = new QAction(this);
    retranslate();
    addAction(aEditFont);
    addAction(aResetToDefault);
    setDefaultAction(aEditFont);
    connect(aResetToDefault, SIGNAL(triggered()), this, SLOT(resetToDefaultFont()));
    connect(aEditFont, SIGNAL(triggered()), this, SLOT(editFont()));
}

void FontSelectorButton::editFont()
{
    QWidget *focused = QApplication::activeWindow();
    QFontDialog dlg;
    QFont useMe;
    if (_currentDefined)
        useMe = _current;
    else
        useMe = _default;
    dlg.setCurrentFont(useMe);
    if (dlg.exec()==QDialog::Accepted) {
        if (useMe!=dlg.currentFont()) {
            _fontChanged = true;
            _current = dlg.currentFont();
            applyFont(_current);
        }
    }
    QApplication::setActiveWindow(focused);
}

void FontSelectorButton::setDefaultFont(const QFont &font)
{
    _default = font;
    applyFont(font);
}

void FontSelectorButton::setCurrentFont(const QFont &font)
{
    _currentDefined = true;
    _current = font;
    applyFont(font);
}

QFont FontSelectorButton::currentFont()
{
    if (_currentDefined)
        return _current;
    else
        return _default;
}

void FontSelectorButton::resetToDefaultFont()
{
    _currentDefined = true;
    _current = _default;
    applyFont(_current);
}

void FontSelectorButton::applyFont(const QFont &font)
{
//    setFont(font);
    QTextDocument doc(this);
    doc.setDefaultFont(font);
    doc.setPlainText(this->text());
    setToolTip(doc.toHtml());
}

void FontSelectorButton::retranslate()
{
    aEditFont->setText(tr("Edit font"));
    aResetToDefault->setText(tr("Reset to default"));
    aEditFont->setToolTip(aEditFont->text());
    aResetToDefault->setToolTip(aResetToDefault->text());
}

void FontSelectorButton::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        retranslate();
    }
}
