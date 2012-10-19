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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BASIC_ITEM_EDITION_DIALOG_H
#define BASIC_ITEM_EDITION_DIALOG_H

#include <calendar/calendar_exporter.h>
#include <calendar/modelanditem/calendar_item.h>

#include <QDialog>
#include <QPushButton>

/**
 * \file basic_item_edition_dialog.h
 * \author Guillaume Denry, Eric Maeker
 * \version 0.6.0
 * \date 11 Jul 2011
*/

namespace Calendar {
class AbstractCalendarModel;
class ICalendarItemDataWidget;
class CalendarItem;
namespace Internal {
namespace Ui {
class BasicItemEditionDialog;
} // End namespace Ui
}  // End namespace Internal


class CALENDAR_EXPORT BasicItemEditorDialog : public QDialog
{
    Q_OBJECT
public:
    BasicItemEditorDialog(AbstractCalendarModel *model, QWidget *parent = 0);
    ~BasicItemEditorDialog();

    void addCalendarDataWidget(Calendar::ICalendarItemDataWidget *dataWidget);

    void init(const CalendarItem &item);

protected:
    void done(int r);

private Q_SLOTS:
    void onShowMoreTriggered();

private:
    AbstractCalendarModel *m_Model;
    Internal::Ui::BasicItemEditionDialog *ui;
    QPushButton *m_moreInfo;
    CalendarItem m_Item;
};

}  // End namespace Calendar

#endif
