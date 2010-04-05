/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERMANAGER_P_H
#define USERMANAGER_P_H

class QMainWindow;
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

class UserManagerPrivate : public QObject, private Ui::UserManager
{
    Q_OBJECT
    Q_DISABLE_COPY(UserManagerPrivate)

public:
    explicit UserManagerPrivate( QMainWindow *parent ); // work with usermodel
    ~UserManagerPrivate();
    bool initialize();

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

private:
    bool         m_CanModify, m_CanCreate, m_CanViewAllUsers, m_CanViewRestrictedDatas, m_CanDelete;
    int          m_EditingRow;
    int          m_SearchBy;
    QMainWindow *m_Parent;
    QToolButton *m_SearchToolBut;
    QAction     *searchByNameAct, *searchBySurnameAct, *searchByNameAndSurnameAct, *searchByCityAct;
    QLabel      *m_PermanentUserName;
    QWidget     *m_PermanentWidget;

public:
    UserManagerContext *m_Context;
};

}  // End Internal
}  // End UserPlugin


#endif // TKUSERMANAGER_P_H
