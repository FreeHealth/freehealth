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
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.0
 * \date 04 Mar 2012
*/

namespace Views {
class StringListView;
class LanguageComboBox;
}

namespace Utils {
class LineEditEchoSwitcher;
class UpperCaseValidator;
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

    bool validatePage();

private Q_SLOTS:
    void checkLogin();
    void checkLoginAfterEdition();
    void checkControlPassword(const QString &text);
    void on_leName_textChanged(const QString &text);
    void on_leFirstName_textChanged(const QString &text);
    void on_leLogin_textChanged(const QString &text);
    void onNamesEditionFinished();

private:
    void changeEvent(QEvent *e);
    void retranslate();
    void toggleErrorLabels();

private:
    Internal::Ui::UserIdentityAndLoginPage *ui;
    bool _showErrorLabels;
};

} // namespace Internal
} // namespace UserPlugin

#endif // USERIDENTITYANDLOGINPAGE_H
