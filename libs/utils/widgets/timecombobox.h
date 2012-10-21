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
 *   Main developers: Christian A. Reiter                                  *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef VIEWS_DATETIMEEDIT_H
#define VIEWS_DATETIMEEDIT_H

#include <utils/global_exporter.h>
#include <QWidget>
#include <QComboBox>

#include <QTime>

/**
 * \file timecombobox.h
 * \author Christian A. Reiter
 * \version 0.8.0
 * \date 2012-10-19
*/
namespace Views {
namespace Internal {
class TimeComboBoxPrivate;
}

class UTILS_EXPORT TimeComboBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(bool editable READ editable WRITE setEditable)
public:
    explicit TimeComboBox(QWidget *parent = 0);
    explicit TimeComboBox(QTime &time, QWidget *parent = 0);
    ~TimeComboBox();

    bool initialize();
    QTime time() const;
    bool editable() const;

Q_SIGNALS:
    void timeChanged(const QTime &time);
    void dateTimeChanged(const QDateTime &dateTime);


public Q_SLOTS:
    void setTime(const QTime &time);
    void setEditable (bool editable);
    void setInterval(int interval);

private:
    Internal::TimeComboBoxPrivate *d;

private Q_SLOTS:
    void on_combo_currentIndexChanged(const int index);
};

} // namespace Views

#endif  // VIEWS_DATETIMEEDIT_H

