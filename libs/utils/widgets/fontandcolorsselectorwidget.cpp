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
#include "fontandcolorsselectorwidget.h"

#include <utils/widgets/fontselectorbutton.h>
#include <utils/widgets/colorbuttonchooser.h>

#include <QCoreApplication>
#include <QHBoxLayout>

using namespace Utils;

FontAndColorsSelectorWidget::FontAndColorsSelectorWidget(QWidget *parent) :
    QWidget(parent),
    _fontButton(0),
    _label(0),
    _colorButton(0)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    setLayout(lay);
    lay->setMargin(0);
    _label = new QLabel(this);
    _fontButton = new FontSelectorButton(this);
    _colorButton = new ColorButtonChooser(this);
    lay->addWidget(_label);
    lay->addWidget(_fontButton);
    lay->addWidget(_colorButton);
}

void FontAndColorsSelectorWidget::setLabelText(const QString &unTranslatedText, const QString &translationContext)
{
    _unTrLabel = unTranslatedText;
    _trContext = translationContext;
    retranslate();
}

void FontAndColorsSelectorWidget::setDefaultFont(const QFont &font)
{
    _fontButton->setDefaultFont(font);
}

void FontAndColorsSelectorWidget::setCurrentFont(const QFont &font)
{
    _fontButton->setCurrentFont(font);
}

void FontAndColorsSelectorWidget::setDefaultColor(const QColor &color)
{
    Q_UNUSED(color);
}

void FontAndColorsSelectorWidget::setCurrentColor(const QColor &color)
{
    _colorButton->setColor(color);
}

QFont FontAndColorsSelectorWidget::currentFont() const
{
    return _fontButton->currentFont();
}

QColor FontAndColorsSelectorWidget::currentColor() const
{
    return _colorButton->color();
}

void FontAndColorsSelectorWidget::retranslate()
{
    _label->setText(QCoreApplication::translate(_trContext.toUtf8(), _unTrLabel.toUtf8()));
}

void FontAndColorsSelectorWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWidget::changeEvent(event);
}
