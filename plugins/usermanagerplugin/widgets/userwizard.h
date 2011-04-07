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
#ifndef USERWIZARD_H
#define USERWIZARD_H

#include <usermanagerplugin/usermanager_exporter.h>

#include <QObject>
#include <QWidget>
#include <QWizardPage>
#include <QWizard>
#include <QHash>
#include <QString>
class QLabel;
class QEvent;
class QLineEdit;
class QPushButton;
class QCheckBox;

/**
 * \file userwizard.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 16 June 2010
*/

namespace Views {
class StringListView;
}

namespace Utils {
class LineEditEchoSwitcher;
}

namespace Print {
class PrinterPreviewer;
class TextDocumentExtra;
}


namespace UserPlugin {
namespace Internal {
class UserRightsWidget;
}

/** \todo create a new wizard manager in Core */

class USER_EXPORT UserWizard : public QWizard
{
    Q_OBJECT
public:
    enum Pages {
        LanguageSelectorPage,
        LoginPasswordPage,
        IdentityPage,
        AdressPage,
        TelsAndMailPage,
        ProfilPage,
        RightsPage,
        SpecialiesQualificationsPage
    };

    UserWizard(QWidget *parent = 0);

    void setModelRow(const int row)        { m_Row = row; }
    void createUser(bool state = true)     { m_CreateUser = state; }

    bool setCreatedUserAsCurrent() const;

    static void setUserPaper(const int ref, const QString &xml) {m_Papers.insert(ref, xml);}
    static void setUserRights(const int role, const int value) {m_Rights.insert(role, value);}
    static int userRights(const int role) {return m_Rights.value(role,0);}

protected Q_SLOTS:
    void done(int r);

private:
    int m_Row;
    bool m_Saved, m_CreateUser;
    static QHash<int, QString> m_Papers;
    static QHash<int, int> m_Rights;
};


class UserLanguageSelectorPage: public QWizardPage
{
    Q_OBJECT
public:
    UserLanguageSelectorPage(QWidget *parent = 0);
private:
    void changeEvent(QEvent *e);
    void retranslate();
    QLabel * lbl;
};

class UserIdentityPage: public QWizardPage
{
    Q_OBJECT
public:
    UserIdentityPage(QWidget *parent = 0);
    bool validatePage();
};

class UserAdressPage: public QWizardPage
{
    Q_OBJECT
public:
    UserAdressPage(QWidget *parent = 0);
};

class UserTelsAndMailPage: public QWizardPage
{
    Q_OBJECT
public:
    UserTelsAndMailPage(QWidget *parent = 0);
    bool validatePage();
};

class UserLoginPasswordPage: public QWizardPage
{
    Q_OBJECT
public:
    UserLoginPasswordPage(QWidget *parent = 0);
    bool validatePage();
private:
    QPushButton *echoConfirmPass, *echoPass;
    Utils::LineEditEchoSwitcher *leLogin, *lePassword, *lePasswordConfirm;
};

class UserProfilPage : public QWizardPage
{
    Q_OBJECT
public:
    UserProfilPage(QWidget *parent = 0);
    bool validatePage();

    int nextId() const {return next;}

private:
    Views::StringListView *view;
    QCheckBox *box;
    int next;
};

class UserSpecialiesQualificationsPage: public QWizardPage
{
    Q_OBJECT
public:
    UserSpecialiesQualificationsPage(QWidget *parent = 0);
};

class UserRightsPage: public QWizardPage
{
    Q_OBJECT
public:
    UserRightsPage(QWidget *parent = 0);
    void initializePage();
    bool validatePage();

private:
    Internal::UserRightsWidget *um, *drugs, *med, *paramed, *administ;
};

//class UserPaperPage: public QWizardPage
//{
//    Q_OBJECT
//public:
//    UserPaperPage(const QString &paperName, int nextPage = -1, QWidget *parent = 0);
//    bool validatePage();

//    int nextId() const {return m_Next;}

//private:
//    Print::TextDocumentExtra *header, *footer, *wm;
//    Print::PrinterPreviewer *previewer;
//    QString type;
//    int m_Next;
//};

//class UserPrescriptionsPage: public QWizardPage
//{
//    Q_OBJECT
//public:
//    UserPrescriptionsPage(QWidget *parent = 0);
//};
//
//class UserAdministrativePage: public QWizardPage
//{
//    Q_OBJECT
//public:
//    UserAdministrativePage(QWidget *parent = 0);
//};

}  // End UserPlugin


#endif // TKUSERWIZARD_H
