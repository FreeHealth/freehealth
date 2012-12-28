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
#ifndef USERFIRSTRUNPAGE_H
#define USERFIRSTRUNPAGE_H

#include <coreplugin/ifirstconfigurationpage.h>

#include <QWizardPage>

namespace UserPlugin {
class UserManagerDialog;

namespace Ui {
    class FirstRunUserCreationWidget;
}

class UserCreationPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit UserCreationPage(QWidget *parent = 0);
    ~UserCreationPage();

    void initializePage();
    bool validatePage();

protected:
    void retranslate();
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void userManager();
    void userWizard();

private:
    Ui::FirstRunUserCreationWidget *ui;
    UserManagerDialog *_userManagerDialog;
};

class FirstRun_UserCreation : public Core::IFirstConfigurationPage
{
public:
    FirstRun_UserCreation(QObject *parent = 0) : Core::IFirstConfigurationPage(parent) {}
    ~FirstRun_UserCreation() {}
    int id() const {return Core::IFirstConfigurationPage::UserCreation;}
    QWizardPage *createPage(QWidget *parent) {return new UserPlugin::UserCreationPage(parent);}
};


}  // End namespace UserPlugin


#endif // USERFIRSTRUNPAGE_H
