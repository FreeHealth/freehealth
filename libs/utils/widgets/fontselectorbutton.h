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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                      *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FONTSELECTORBUTTON_H
#define FONTSELECTORBUTTON_H

#include <utils/global_exporter.h>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

#include <QToolButton>

namespace Utils {

class UTILS_EXPORT FontSelectorButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FontSelectorButton(QWidget *parent = 0);

public Q_SLOTS:
    void setDefaultFont(const QFont &font);
    void setCurrentFont(const QFont &font);
    QFont currentFont();
    bool fontChanged() {return _fontChanged;}

    void resetToDefaultFont();

private Q_SLOTS:
    void editFont();

private:
    void applyFont(const QFont &font);
    void retranslate();
    void changeEvent(QEvent *event);

private:
    QAction *aEditFont, *aResetToDefault;
    bool _fontChanged, _currentDefined;
    QFont _default;
    QFont _current;
};

}  // end Utils

#endif // FONTSELECTORBUTTON_H
