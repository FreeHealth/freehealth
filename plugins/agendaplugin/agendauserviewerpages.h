/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef AGENDAUSERVIEWERPAGES_H
#define AGENDAUSERVIEWERPAGES_H

#include <usermanagerplugin/widgets/iuserviewerpage.h>

#include <QWidget>
#include <QPointer>
class QDataWidgetMapper;

/**
 * \file agendauserviewerpages.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 12 Jul 2011
*/

namespace Agenda {
namespace Internal {
namespace Ui {
class AgendaUserViewer;
}

class AgendaUserIdentityWidget : public QWidget
{
    Q_OBJECT
public:
    AgendaUserIdentityWidget(QWidget *parent = 0);
    ~AgendaUserIdentityWidget();

    void setUserModel(UserPlugin::UserModel *model);
    void setUserIndex(const int index);

    void clear();
    bool submit();

//private Q_SLOTS:
//    void on_but_changePassword_clicked();
//    void on_but_viewHistory_clicked();

private:
    Ui::AgendaUserViewer *ui;
    QDataWidgetMapper *m_Mapper;
    UserPlugin::UserModel *m_Model;
};

class AgendaUserViewerPage : public UserPlugin::IUserViewerPage
{
    Q_OBJECT
public:
    AgendaUserViewerPage(QObject *parent = 0);
    ~AgendaUserViewerPage();

    QString id() const;
    QString name() const;
    QString category() const;
    QString title() const;

    QWidget *createPage(QWidget *parent);

    void setUserModel(UserPlugin::UserModel *model);
    void setUserIndex(const int index);
    bool clear();
    bool submit();

private:
    QPointer<AgendaUserIdentityWidget> m_Widget;
    UserPlugin::UserModel *m_Model;
};


}  // End namespace Internal
}  // End namespace Agenda

#endif // AGENDAUSERVIEWERPAGES_H
