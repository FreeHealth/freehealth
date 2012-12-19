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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AVAILABILITYCREATORDIALOG_H
#define AVAILABILITYCREATORDIALOG_H

#include <QDialog>
#include <QList>
#include <agendaplugin/usercalendar.h>

namespace Agenda {
namespace Ui {
    class AvailabilityCreatorDialog;
}

class AvailabilityEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AvailabilityEditDialog(QWidget *parent = 0);
    ~AvailabilityEditDialog();

    QList<DayAvailability> getAvailability() const;

private Q_SLOTS:
    void on_startTime_timeChanged(const QTime &from);

private:
    Ui::AvailabilityCreatorDialog *ui;
};

}  // End namespace Agenda

#endif // AVAILABILITYCREATORDIALOG_H
