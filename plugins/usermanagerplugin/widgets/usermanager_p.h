/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERMANAGER_P_H
#define USERMANAGER_P_H

#include <QWidget>

class QModelIndex;
class QTableView;
class QToolButton;
class QLabel;
class QWidget;
class QGroupBox;

#include "ui_usermanager.h"

namespace UserPlugin {
namespace Internal {
class UserManagerContext;

class UserManagerWidget : public QWidget, private Ui::UserManager
{
    Q_OBJECT
    Q_DISABLE_COPY(UserManagerWidget)

public:
    explicit UserManagerWidget(QWidget *parent); // work with usermodel
    ~UserManagerWidget();
    bool initialize();

    bool canCloseParent();

private Q_SLOTS:
    void on_searchLineEdit_textchanged();
    void on_m_SearchToolButton_triggered( QAction * act );
    void on_saveAct_triggered();
    void on_userTableView_activated( const QModelIndex & );
    void on_createNewUserAct_triggered();
    void on_clearModificationsAct_triggered();
    void on_deleteUserAct_triggered();
    void updateStatusBar();

    void showUserDebugDialog( const QModelIndex &id );

private:
    void analyseCurrentUserRights();
    void selectUserTableView( int row );
    void changeEvent(QEvent *e);
    void retranslate();

Q_SIGNALS:
    void closeRequested();

private:
    bool         m_CanModify, m_CanCreate, m_CanViewAllUsers, m_CanViewRestrictedDatas, m_CanDelete;
    int          m_EditingRow;
    int          m_SearchBy;
    QWidget     *m_Parent;
    QToolBar    *m_ToolBar;
    QToolButton *m_SearchToolBut;
    QAction     *searchByNameAct, *searchByFirstnameAct, *searchByNameAndFirstnameAct, *searchByCityAct;
    QLabel      *m_PermanentUserName;
    QWidget     *m_PermanentWidget;

public:
    UserManagerContext *m_Context;
};

}  // End Internal
}  // End UserPlugin


#endif // TKUSERMANAGER_P_H
