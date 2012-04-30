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
#ifndef SEGMENTEDBUTTON_H
#define SEGMENTEDBUTTON_H

#include <utils/global_exporter.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
class QHBoxLayout;
QT_END_NAMESPACE

namespace Utils {

class UTILS_EXPORT SegmentedButton : public QWidget
{
    Q_OBJECT
public:
    explicit SegmentedButton(QWidget *parent = 0);

    void setFirstButton(QPushButton *but);
    void addMiddleButton(QPushButton *but);
    void setLastButton(QPushButton *but);

    void setAutoExclusive(bool state);
    void computeSizes();

private:
    QHBoxLayout *_buttonLayout;
    QPushButton *_first, *_last;
    QList<QPushButton*> _buttons;
};

}

#endif // SEGMENTEDBUTTON_H
