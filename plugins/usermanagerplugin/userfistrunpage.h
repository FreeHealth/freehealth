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
#ifndef USERFISTRUNPAGE_H
#define USERFISTRUNPAGE_H

#include <coreplugin/ifirstconfigurationpage.h>

#include <QWizardPage>

namespace UserPlugin {

namespace Ui {
    class UserConnectionPage;
    class FirstRunUserCreationWidget;
}

class UserConnectionPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit UserConnectionPage(QWidget *parent = 0);
    ~UserConnectionPage();

    void initializePage();
    bool validatePage();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::UserConnectionPage *ui;
};

class UserCreationPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit UserCreationPage(QWidget *parent = 0);
    ~UserCreationPage();

    void initializePage();
    bool validatePage();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void userManager();
    void userWizard();

private:
    Ui::FirstRunUserCreationWidget *ui;
};

class FirstRun_UserConnection : public Core::IFirstConfigurationPage
{
public:
    FirstRun_UserConnection(QObject *parent = 0) : Core::IFirstConfigurationPage(parent) {}
    ~FirstRun_UserConnection() {}
    int id() const {return Core::IFirstConfigurationPage::UserDbConnection;}
    QWizardPage *createPage(QWidget *parent) {return new UserPlugin::UserConnectionPage(parent);}
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


#endif // USERFISTRUNPAGE_H
