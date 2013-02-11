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
 *       Christian A. Reiter <christian.a.reiter@gmail.com>>               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef USERIDENTITYANDLOGINPAGE_H
#define USERIDENTITYANDLOGINPAGE_H

#include <usermanagerplugin/iuserwizardpage.h>

#include <QWidget>

/**
 * \file useridentityandloginpage.h
 * \author Eric MAEKER
 * \version 0.8.2
 * \date 18 Dec 2012
*/

namespace Identity {
class IdentityEditorWidget;
}

namespace UserPlugin {
namespace Internal {
namespace Ui {
class UserIdentityAndLoginPage;
}

class UserIdentityAndLoginPage: public QWizardPage
{
    Q_OBJECT

public:
    explicit UserIdentityAndLoginPage(QWidget *parent = 0);
    ~UserIdentityAndLoginPage();

    void initializePage();
    bool isComplete() const;
    bool validatePage();

private Q_SLOTS:
    void checkLoginAfterEdition();
    void onPasswordConfirmed();
//    void onNamesEditionFinished();

private:
    bool checkLogin() const;
    void changeEvent(QEvent *e);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void retranslate();

private:
    Identity::IdentityEditorWidget *_identity;
    bool _showErrorLabels;
};

} // namespace Internal
} // namespace UserPlugin

#endif // USERIDENTITYANDLOGINPAGE_H
