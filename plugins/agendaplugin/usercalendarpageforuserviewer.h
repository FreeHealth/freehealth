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
#ifndef USERCALENDARPAGEFORUSERVIEWER_H
#define USERCALENDARPAGEFORUSERVIEWER_H

#include <usermanagerplugin/widgets/iuserviewerpage.h>

#include <QWidget>
#include <QPointer>
class QDataWidgetMapper;

/**
 * \file usercalendarpageforuserviewer.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 30 Jul 2011
*/

namespace Agenda {
class UserCalendar;
class UserCalendarModel;
class UserCalendarModelFullEditorWidget;

namespace Internal {

class UserCalendarPageForUserViewerWidget : public UserPlugin::IUserViewerWidget
{
    Q_OBJECT
public:
    UserCalendarPageForUserViewerWidget(QWidget *parent = 0);
    ~UserCalendarPageForUserViewerWidget();

    void setUserModel(UserPlugin::UserModel *model);
    void setUserIndex(const int index);
    void clear();
    bool submit();

private Q_SLOTS:
    void userChanged();

private:
    UserCalendarModelFullEditorWidget *m_Widget;
    UserPlugin::UserModel *m_UserModel;
};

class UserCalendarPageForUserViewer : public UserPlugin::IUserViewerPage
{
    Q_OBJECT
public:
    UserCalendarPageForUserViewer(QObject *parent = 0);
    ~UserCalendarPageForUserViewer();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;
    int sortIndex() const;

    QWidget *createPage(QWidget *parent);
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // USERCALENDARPAGEFORUSERVIEWER_H
