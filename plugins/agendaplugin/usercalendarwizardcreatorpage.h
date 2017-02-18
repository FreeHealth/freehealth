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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_USERCALENDARWIZARDCREATORPAGE_H
#define AGENDA_USERCALENDARWIZARDCREATORPAGE_H

#include <usermanagerplugin/iuserwizardpage.h>

#include <QWidget>

/**
 * \file usercalendarwizardcreatorpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.2
 * \date 22 May 2012
*/

namespace Agenda {
class UserCalendar;

namespace Internal {

namespace Ui {
class UserCalendarWizardCreatorWidget;
}

class UserCalendarWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit UserCalendarWizardPage(QWidget *parent = 0);
    ~UserCalendarWizardPage();

    UserCalendar *getUserCalendar(const QString &userUid);

    bool validatePage();

protected Q_SLOTS:
    void toggleAgendaEditing(bool state);

private:
    void retranslate();
    void changeEvent(QEvent *event);

    Ui::UserCalendarWizardCreatorWidget *ui;
};


class UserCalendarWizardCreatorPage : public UserPlugin::IUserWizardPage
{
    Q_OBJECT

public:
    explicit UserCalendarWizardCreatorPage(QObject *parent = 0);
    ~UserCalendarWizardCreatorPage();

    // Core::IGenericPage
    QString id() const {return objectName();}
    QString displayName() const;
    QString title() const;
    QString category() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent = 0) {Q_UNUSED(parent); return 0;}

    // UserPlugin::IUserWizardPage
    QWizardPage *createWizardPage(QWidget *parent);
    void submit(const QString &userUid);

private:
    UserCalendarWizardPage *page;
};

}  // End namespace Internal
}  // End namespace Agenda


#endif // AGENDA_USERCALENDARWIZARDCREATORPAGE_H
