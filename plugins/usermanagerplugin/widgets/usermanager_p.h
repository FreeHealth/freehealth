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
#ifndef USERMANAGERWIDGET_P_H
#define USERMANAGERWIDGET_P_H

#include <QWidget>

class QModelIndex;
class QTableView;
class QToolButton;
class QToolBar;
class QLabel;
class QWidget;
class QGroupBox;

namespace UserPlugin {
namespace Internal {
class UserManagerContext;

namespace Ui {
    class UserManagerWidget;
}

class UserManagerWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(UserManagerWidget)

public:
    explicit UserManagerWidget(QWidget *parent = 0); // work with usermodel
    ~UserManagerWidget();
    bool initialize();

    bool canCloseParent();

private Q_SLOTS:
    void onCurrentUserChanged();
    void onSearchRequested();
    void onSearchToolButtonTriggered(QAction *act);
    void onSaveRequested();
    void onUserClicked(const QModelIndex &index);
    void onCreateUserRequested();
    void onClearModificationRequested();
    void onDeleteUserRequested();
    void toggleSearchView(bool checked);

    void showUserDebugDialog(const QModelIndex &id);

private:
    void analyseCurrentUserRights();
    void selectUserTableView(int row);
    void changeEvent(QEvent *e);
    void retranslate();

Q_SIGNALS:
    void closeRequested();

private:
    Ui::UserManagerWidget *ui;
    bool m_CanModify, m_CanCreate, m_CanViewAllUsers, m_CanViewRestrictedData, m_CanDelete;
    int m_EditingRow;
    int m_SearchBy;
    QToolBar *m_ToolBar;
    QToolButton *m_SearchToolButton;
    QAction *aCreateUser;
    QAction *aModifyUser;
    QAction *aSave;
    QAction *aRevert;
    QAction *aDeleteUser;
    QAction *aQuit;
    QAction *aToggleSearchView;
    QAction *searchByNameAct, *searchByFirstnameAct, *searchByNameAndFirstnameAct, *searchByCityAct;

public:
    UserManagerContext *m_Context;
};

}  // End Internal
}  // End UserPlugin


#endif // USERMANAGER_P_H
