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
 *  Main Developers:                                                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FONTANDCOLORSSELECTORWIDGET_H
#define FONTANDCOLORSSELECTORWIDGET_H

#include <utils/global_exporter.h>
#include <QWidget>
#include <QLabel>

namespace Utils {
class FontSelectorButton;
class ColorButtonChooser;

class UTILS_EXPORT FontAndColorsSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FontAndColorsSelectorWidget(QWidget *parent = 0);

    void setLabelText(const QString &unTranslatedText, const QString &translationContext);
    void setDefaultFont(const QFont &font);
    void setCurrentFont(const QFont &font);
    void setDefaultColor(const QColor &color);
    void setCurrentColor(const QColor &color);

    QFont currentFont() const;
    QColor currentColor() const;

private:
    void retranslate();
    void changeEvent(QEvent *event);

private:
    FontSelectorButton *_fontButton;
    QLabel *_label;
    ColorButtonChooser *_colorButton;
    QString _unTrLabel, _trContext;
};

}  // namespace Utils

#endif // FONTANDCOLORSSELECTORWIDGET_H
