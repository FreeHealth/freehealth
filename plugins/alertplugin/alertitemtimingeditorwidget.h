/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef ALERT_ALERTITEMTIMINGEDITORWIDGET_H
#define ALERT_ALERTITEMTIMINGEDITORWIDGET_H

#include <alertplugin/alertplugin_exporter.h>
#include <QWidget>

namespace Alert {
class AlertItem;
class AlertTiming;
namespace Internal {
namespace Ui {
class AlertItemTimingEditorWidget;
}
}

class ALERT_EXPORT AlertItemTimingEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlertItemTimingEditorWidget(QWidget *parent = 0);
    ~AlertItemTimingEditorWidget();

public Q_SLOTS:
    void clear();
    void setAlertItem(const AlertItem &item);
    bool submit(AlertItem &item);

private Q_SLOTS:
    void cycleComboChanged(int index);
    void checkDates();
    void startPeriodSelected(int period, int value);
    void endPeriodSelected(int period, int value);

private:
    void cyclingToUi(const Alert::AlertTiming &timing);
    void cyclingFromUi(Alert::AlertTiming &timing);

private:
    Internal::Ui::AlertItemTimingEditorWidget *ui;
    bool _periodicalCycling;
};

} // namespace Alert

#endif // ALERT_ALERTITEMTIMINGEDITORWIDGET_H
