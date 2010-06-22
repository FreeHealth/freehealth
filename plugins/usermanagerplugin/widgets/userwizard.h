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
        SpecialiesQualificationsPage,
        RightsPage,
        PaperGenericPage,
        PaperAdministrativePage,
        PaperPrescriptionsPage
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

class UserPaperPage: public QWizardPage
{
    Q_OBJECT
public:
    UserPaperPage(const QString &paperName, QWidget *parent = 0);
    bool validatePage();

    int nextId() const;

private:
    Print::TextDocumentExtra *header, *footer, *wm;
    Print::PrinterPreviewer *previewer;
    QString type;
};

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
