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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PERIODSELECTORTOOLBUTTON_H
#define PERIODSELECTORTOOLBUTTON_H

#include <utils/global_exporter.h>
#include <QToolButton>

namespace Utils {
namespace Internal {
class PeriodSelectorToolButtonPrivate;
}  // namespace Internal

class UTILS_EXPORT PeriodSelectorToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit PeriodSelectorToolButton(QWidget *parent = 0);
    ~PeriodSelectorToolButton();

    void setMainMenuTitle(const QString &translatableTitle, const QString &translationContext);
    void setStartPeriodsAt(const int transConstantsTimeEnumValue);

Q_SIGNALS:
    void periodSelected(int period, int value);

private Q_SLOTS:
    void _actionTriggered(QAction *a);

private:
    void changeEvent(QEvent *e);

private:
    Internal::PeriodSelectorToolButtonPrivate *d;
};

}  // namespace Utils

#endif // PERIODSELECTORTOOLBUTTON_H
